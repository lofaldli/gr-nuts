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
      d_data_reg(0),
      d_size_index(0),
      d_bit_counter(0),
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
      struct rs* rs_32 = (struct rs*)init_rs_char(8, 0x187, 112, 11, 32, 0);
      memcpy( (void*)&rs_cb_rx[6], (void*)rs_32, sizeof(rs_cb_rx[6]) );
      memcpy( (void*)&rs_cb_rx[5], (void*)rs_32, sizeof(rs_cb_rx[5]) );
      memcpy( (void*)&rs_cb_rx[4], (void*)rs_32, sizeof(rs_cb_rx[4]) );
      memcpy( (void*)&rs_cb_rx[3], (void*)rs_32, sizeof(rs_cb_rx[3]) );

      struct rs* rs_16 = (struct rs*)init_rs_char(8, 0x187, 112, 11, 16, 0);
      memcpy( (void*)&rs_cb_rx[2], (void*)rs_16, sizeof(rs_cb_rx[2]) );
      memcpy( (void*)&rs_cb_rx[1], (void*)rs_16, sizeof(rs_cb_rx[1]) );
      memcpy( (void*)&rs_cb_rx[0], (void*)rs_16, sizeof(rs_cb_rx[0]) );

      rs_cb_rx[6].pad = 255-NGHAM_CODEWORD_SIZE[6];
      rs_cb_rx[5].pad = 255-NGHAM_CODEWORD_SIZE[5];
      rs_cb_rx[4].pad = 255-NGHAM_CODEWORD_SIZE[4];
      rs_cb_rx[3].pad = 255-NGHAM_CODEWORD_SIZE[3];
      rs_cb_rx[2].pad = 255-NGHAM_CODEWORD_SIZE[2];
      rs_cb_rx[1].pad = 255-NGHAM_CODEWORD_SIZE[1];
      rs_cb_rx[0].pad = 255-NGHAM_CODEWORD_SIZE[0];

      // free temporary rs tables
      delete rs_32;
      delete rs_16;
    }

    /*
     * Our virtual destructor.
     */
    ngham_decoder_impl::~ngham_decoder_impl() {
      // free rs tables
      delete [] rs_cb_rx[6].alpha_to;
      delete [] rs_cb_rx[6].index_of;
      delete [] rs_cb_rx[6].genpoly;
      delete [] rs_cb_rx[0].alpha_to;
      delete [] rs_cb_rx[0].index_of;
      delete [] rs_cb_rx[0].genpoly;
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
            nerrors = decode_rs_char(&rs_cb_rx[d_size_index], d_codeword, 0, 0);
        }

        // check if packet was decoded correctly
        if (nerrors == -1) {
            if (d_verbose)
                printf("\tcould not decode reed solomon\n");
            return false;
        }

        if (d_verbose)
            printf("\tdecoded reed solomon with %i errors\n", nerrors);

        d_payload_len = NGHAM_PL_SIZE[d_size_index] - (d_codeword[0] & 0x1f);

        // calculate crc
        uint16_t crc = 0xffff;
        for (int i=0; i< d_payload_len+1; i++){
            crc = ((crc >> 8) & 0xff) ^ crc_ccitt_table[(crc ^ d_codeword[i]) & 0xff];
        }
        crc ^= 0xffff;

        // check crc
        if ( crc != ( (d_codeword[d_payload_len+1]<<8) | d_codeword[d_payload_len+2] ) ) {
            if (d_verbose)
                printf("crc failed\n");
            return false;
        }

        // packet was succesfully decoded
        d_num_packets++;
        return true;
    }

    void ngham_decoder_impl::print_packet() {
        printf("number of packets received %i\n", d_num_packets);
        printf("decoded data:\n");
        for (int i=0; i<d_codeword_length; i+=8) {
            printf("\t");
            for (int j=0; j<8; j++) {
                if (i+j<d_codeword_length)
                    printf("0x%x%x ", (d_codeword[i+j] >> 4) & 0x0f, d_codeword[i+j] & 0x0f);
                else
                    printf("\t");
            }
            printf("\t");
            for (int j=0; j<8 && i+j<d_codeword_length; j++) {
                if (d_codeword[i+j] > 31 && d_codeword[i+j] < 127)
                    printf("%c ", d_codeword[i+j]);
                else
                    printf(". ");
            }
            printf("\n");
        }
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
                    if (d_codeword_length == NGHAM_CODEWORD_SIZE[d_size_index]) {
                        if (d_verbose)
                            printf("\tloaded codeword of length %i\n", d_codeword_length);

                        bool success = decode_packet();
                        if (success) {
                            // post payload data to message queue
                            pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL, pmt::make_blob(d_codeword+1, d_payload_len)));
                            message_port_pub(pmt::mp("out"), pdu);

                            if (d_printing)
                                print_packet();
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

