/* -*- c++ -*- */
/*
 * Copyright 2015 André Løfaldli.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "ngham_decoder_impl.h"

#include <volk/volk.h>

#include "fec-3.0.1/fec.h"
#include "fec-3.0.1/char.h"
#include "fec-3.0.1/rs-common.h"

#include "ngham.h"
#include "reed_solomon.h"

#define STATE_SYNC_SEARCH 0
#define STATE_LOAD_SIZE_TAG 1
#define STATE_SIZE_TAG_COMPARE 2
#define STATE_CODEWORD 3

namespace gr {
  namespace nuts {

    ngham_decoder::sptr
    ngham_decoder::make(const std::string& len_tag_key, int threshold, bool rs_decode, bool descramble, bool printing, bool verbose)
    {
      return gnuradio::get_initial_sptr(new ngham_decoder_impl(len_tag_key, threshold, rs_decode, descramble, printing, verbose));
    }

    struct rs rs_cb_rx[NGHAM_SIZES];
    uint32_t sync_word, size_tag[NGHAM_SIZES];

    /*
     * The private constructor
     */
    ngham_decoder_impl::ngham_decoder_impl(const std::string& len_tag_key, int threshold, bool rs_decode, bool descramble, bool printing, bool verbose)
      : gr::sync_block("ngham_decoder",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(0, 0, 0)),
      d_len_tag_key(pmt::string_to_symbol(len_tag_key)),
      d_threshold(threshold),
      d_rs_decode(rs_decode),
      d_descramble(descramble),
      d_printing(printing),
      d_verbose(verbose),
      d_num_packets(0)
    {
      message_port_register_out(pmt::mp("out"));

      enter_sync_search();

      // load sync word into a 32 bit variable
      sync_word = 0;
      for (int j=0; j<NGHAM_SYNC_SIZE; j++) {
          sync_word = (sync_word << 8) | (NGHAM_SYNC[j] & 0xff);
      }

      // load size tags into array of 32 bit variables
      for (int size_index=0; size_index<NGHAM_SIZES; size_index++) {
          size_tag[size_index] = 0;
          for (int j=0; j<NGHAM_SIZE_TAG_SIZE; j++) {
              size_tag[size_index] = (size_tag[size_index] << 8) | (NGHAM_SIZE_TAG[size_index][j] & 0xff);
          }
      }

      // initialize rs tables
      for (uint8_t i=0; i<NGHAM_SIZES; i++) {
          d_rs[i] = new reed_solomon(NGHAM_RS_PARITY_SIZE[i], 255-NGHAM_RS_CODEWORD_SIZE[i]);
      }
    }

    /*
     * Our virtual destructor.
     */
    ngham_decoder_impl::~ngham_decoder_impl() {
        for (uint8_t i=0; i<NGHAM_SIZES; i++) {
            delete d_rs[i];
        }
    }

    void ngham_decoder_impl::enter_sync_search() {
        if (d_verbose) printf("enter_sync_search\n");
        d_decoder_state = STATE_SYNC_SEARCH;
        d_data_reg = 0;
    }
    void ngham_decoder_impl::enter_load_size_tag() {
        if (d_verbose) printf("enter_load_size_tag\n");
        d_decoder_state = STATE_LOAD_SIZE_TAG;
        d_bit_counter = 0;
    }
    void ngham_decoder_impl::enter_size_tag_compare() {
        if (d_verbose) printf("enter_size_tag_compare\n");
        d_decoder_state = STATE_SIZE_TAG_COMPARE;
    }
    void ngham_decoder_impl::enter_codeword() {
        if (d_verbose) printf("enter_codeword\n");
        d_decoder_state = STATE_CODEWORD;
        d_codeword_length = 0;
        d_bit_counter = 0;
    }

    uint32_t ngham_decoder_impl::correlate_32u(uint32_t x, uint32_t y, uint32_t mask) {
        uint32_t ndifferent;
        uint32_t different_bits = (x ^ y) & mask;
        volk_32u_popcnt(&ndifferent, different_bits);

        return ndifferent;
    }

    bool ngham_decoder_impl::decode_packet() {
        // descramble data
        if (d_descramble) {
           for (int i=0; i<d_codeword_length; i++) d_codeword[i] ^= ccsds_poly[i];
        }

        // decode parity data
        int nerrors = 0;
        if (d_rs_decode) {
            nerrors = d_rs[d_size_index]->decode(d_codeword);
        }

        // check if packet was decoded correctly
        if (nerrors == -1) {
            if (d_verbose)
                printf("\tcould not decode reed solomon\n");
            return false;
        }

        if (d_verbose)
            printf("\tdecoded reed solomon with %i errors\n", nerrors);

        uint8_t padding_size = d_codeword[0] & NGHAM_PADDING_SIZE_MASK;
//        uint8_t ngham_flags = d_codeword[0] & NGHAM_FLAGS_MASK; TODO
        d_payload_len = NGHAM_RS_DATA_SIZE[d_size_index] - padding_size;


        // check crc
        uint16_t received_crc = (d_codeword[d_payload_len+1] << 8) | d_codeword[d_payload_len+2];
        uint16_t calculated_crc = calc_crc(d_codeword, d_payload_len+1);
        if (calculated_crc != received_crc) {
            if (d_verbose)
                printf("crc failed\n");
            return false;
        }

        return true;
    }

    int ngham_decoder_impl::work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) {
        const uint8_t *in = (const uint8_t *) input_items[0];
        int count = 0;
        uint32_t nwrong;
        while (count < noutput_items) {

            switch (d_decoder_state) {

                case STATE_SYNC_SEARCH:
                    d_data_reg = (d_data_reg << 1) | ((in[count++]) & 0x01);
                    nwrong = correlate_32u(d_data_reg, sync_word, 0xffffffff);
                    if (nwrong <= d_threshold) {
                        if (d_verbose)
                            printf("received sync word %x, %i bits are wrong\n", d_data_reg, nwrong);
                        enter_load_size_tag();
                    }
                    break;

                case STATE_LOAD_SIZE_TAG:
                    d_data_reg = (d_data_reg << 1) | ((in[count++]) & 0x01);
                    d_bit_counter++;
                    if (d_bit_counter == NGHAM_SIZE_TAG_SIZE * 8) {
                        enter_size_tag_compare();
                    }
                    break;

                case STATE_SIZE_TAG_COMPARE:
                    for (d_size_index=0; d_size_index<NGHAM_SIZES; d_size_index++) {
                        nwrong = correlate_32u(d_data_reg, size_tag[d_size_index], 0x00ffffff);
                        if (d_verbose)
                            printf("\tcomparing %x and %x, %i bits are different\n", d_data_reg & 0x00ffffff, size_tag[d_size_index], nwrong);
                        if (nwrong <= d_threshold) {
                            enter_codeword();
                            break;
                        }
                    }
                    // size tag was not found TODO try all sizes???
                    if (d_size_index == NGHAM_SIZES)
                        enter_sync_search();
                    break;

                case STATE_CODEWORD:
                    d_codeword[d_codeword_length] = (d_codeword[d_codeword_length] << 1) | (in[count++] & 0x01);
                    d_bit_counter++;
                    if (d_bit_counter == 8) {
                        d_codeword_length++;
                        d_bit_counter = 0;
                    }
                    if (d_codeword_length == NGHAM_RS_CODEWORD_SIZE[d_size_index]) {
                        if (d_verbose)
                            printf("\tloaded codeword of length %i\n", d_codeword_length);

                        bool success = decode_packet();
                        if (success) {
                            // post payload data to message queue
                            pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL, pmt::make_blob(d_codeword+1, d_payload_len)));
                            message_port_pub(pmt::mp("out"), pdu);

                            d_num_packets++;
                            if (d_printing)
                                print_bytes(d_codeword, d_codeword_length);
                        }
                        enter_sync_search();
                        break;
                    }
            }
        }
        return noutput_items;
    }
  } /* namespace nuts */
} /* namespace gr */

