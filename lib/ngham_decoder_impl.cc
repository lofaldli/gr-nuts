/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
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

#define STATE_PREAMBLE 0
#define STATE_SYNC 1
#define STATE_SIZE_TAG 2
#define STATE_CODEWORD 3
#define STATE_DECODE 4

namespace gr {
  namespace nuts {

    ngham_decoder::sptr
    ngham_decoder::make(const std::string& len_tag_key, bool rs_decode, bool descramble, bool printing)
    {
      return gnuradio::get_initial_sptr(new ngham_decoder_impl(len_tag_key, rs_decode, descramble, printing));
    }

    struct rs rs_cb_rx[NGHAM_SIZES];
    uint32_t preamble, sync_word, size_tag[NGHAM_SIZES];
    /*
     * The private constructor
     */
    ngham_decoder_impl::ngham_decoder_impl(const std::string& len_tag_key, bool rs_decode, bool descramble, bool printing)
      : gr::sync_block("ngham_decoder",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(0, 0, 0)),
      d_len_tag_key(pmt::string_to_symbol(len_tag_key)),
      d_rs_decode(rs_decode),
      d_descramble(descramble),
      d_printing(printing),
      d_decoder_state(STATE_SIZE_TAG),
      d_data_reg(0),
      d_size_index(0),
      d_bit_counter(0)
    {
      message_port_register_out(pmt::mp("out"));

      preamble = 0;
      for (int j=0; j<NGHAM_PREAMBLE_SIZE; j++) {
          preamble = (preamble << 8) | (NGHAM_PREAMBLE[j] & 0xff);
      }
      
      sync_word = 0;
      for (int j=0; j<NGHAM_SYNC_SIZE; j++) {
          sync_word = (sync_word << 8) | (NGHAM_SYNC[j] & 0xff);
      }
      
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

      delete rs_32;
      delete rs_16;
    }

    /*
     * Our virtual destructor.
     */
    ngham_decoder_impl::~ngham_decoder_impl()
    {
      delete [] rs_cb_rx[6].alpha_to;
      delete [] rs_cb_rx[6].index_of;
      delete [] rs_cb_rx[6].genpoly;
      delete [] rs_cb_rx[0].alpha_to;
      delete [] rs_cb_rx[0].index_of;
      delete [] rs_cb_rx[0].genpoly;
    }

    void ngham_decoder_impl::enter_preamble() {
        //printf("enter_preamble\n");
        d_decoder_state = STATE_PREAMBLE;
    }
    void ngham_decoder_impl::enter_sync() {
        //printf("enter_sync\n");
        d_decoder_state = STATE_SYNC;
    }
    void ngham_decoder_impl::enter_size_tag() {
        //printf("enter_size_tag\n");
        d_decoder_state = STATE_SIZE_TAG;
    }
    void ngham_decoder_impl::enter_codeword() {
        //printf("enter_codeword\n");
        d_decoder_state = STATE_CODEWORD;
        d_codeword_length = 0;
        d_bit_counter = 0;
    }
    void ngham_decoder_impl::enter_decode() {
        //printf("enter_decode\n");
        d_decoder_state = STATE_DECODE;
    }

    void ngham_decoder_impl::parse_length_tags(const std::vector<std::vector<tag_t> > &tags, gr_vector_int &packet_lengths) {
      for (int i=0; i<tags.size(); i++) {
        for (int k=0; k<tags[i].size(); k++) {
          if (tags[i][k].key == d_len_tag_key) {
            packet_lengths[i] = pmt::to_long(tags[i][k].value);
            remove_item_tag(i, tags[i][k]);
          }
        }
      }
    }    

    int
    ngham_decoder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];

      int count = 0;
      int j,k;
      uint32_t wrong_bits, nwrong;


      while (count < noutput_items) {

          switch (d_decoder_state) {

              case STATE_PREAMBLE:
                  enter_sync();
/*                      d_data_reg = (d_data_reg << 1) | ((in[count++]) & 0x01);

                      wrong_bits = 0;
                      nwrong = 6; // TODO threshold

                      wrong_bits = (d_data_reg ^ preamble);

                      volk_32u_popcnt(&nwrong, wrong_bits);
                      if (nwrong <= 0) {
                          enter_sync();
                      }*/
                  break;
              case STATE_SYNC:
                      d_data_reg = (d_data_reg << 1) | ((in[count++]) & 0x01);

                      wrong_bits = 0;
                      nwrong = 6; 
                      wrong_bits = (d_data_reg ^ sync_word);

                      volk_32u_popcnt(&nwrong, wrong_bits);
                      if (nwrong <= 0) {
                          enter_size_tag();
                      }
                      // FIXME what should happen if sync word isn't found ?
                  break;
              case STATE_SIZE_TAG:
                      d_data_reg = (d_data_reg << 1) | ((in[count++]) & 0x01);

                      d_size_index = 0;
                      while (d_size_index < NGHAM_SIZES) {
                          wrong_bits = 0;
                          nwrong = 6; 

                          wrong_bits = ((d_data_reg & 0xffffff) ^ size_tag[d_size_index]);

                          volk_32u_popcnt(&nwrong, wrong_bits);
                          if (nwrong <= 0) {
                              enter_codeword();
                              break;
                          }
                          d_size_index++;
                      }
                  break;

              case STATE_CODEWORD:
                      d_codeword[d_codeword_length] = (d_codeword[d_codeword_length] << 1) | (in[count++] & 0x01);
                      d_bit_counter++;
                      if (d_bit_counter == 8) {
                          d_codeword_length++;
                          d_bit_counter = 0;
                      }
                      if (d_codeword_length == NGHAM_CODEWORD_SIZE[d_size_index]) {
                          enter_decode();
                      }
                  break;
              case STATE_DECODE:
                  enter_preamble();
                  

                  // descramble data
                  if (d_descramble) {
                      for (j=0; j<d_codeword_length; j++) d_codeword[j] ^= ccsds_poly[j];
                  }

                  // decode parity data
                  int nerrors = 0;
                  if (d_rs_decode) {
                      nerrors = decode_rs_char(&rs_cb_rx[d_size_index], d_codeword, 0, 0);
                  }

                  // check if packet is decodable
                  if (nerrors == -1) {
                      printf("\tcould not decode packet\n");
                      break;
                  }

                  // calculate payload length
                  int pl_len = NGHAM_PL_SIZE[d_size_index] - (d_codeword[0] & 0x1f);

                  // calculate crc
                  unsigned int crc = 0xffff;
                  for (j=0; j<pl_len+1; j++){
                      crc = ((crc >> 8) & 0xff) ^ crc_ccitt_table[(crc ^ d_codeword[j]) & 0xff];
                  }
                  crc ^= 0xffff;

                  // check crc
                  if ( crc != ( (d_codeword[pl_len+1]<<8) | d_codeword[pl_len+2] ) ) {
                      break;
                  }

                  pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL, pmt::make_blob(d_codeword+1, pl_len)));
                  message_port_pub(pmt::mp("out"), pdu);

                  // print codeword if tests were passed
                  if (d_printing) {
                      for (j=0; j<d_codeword_length; j+=4) {
                          for (k=0; k<4; k++) {
                              if (j+k<d_codeword_length)
                                  printf("0x%x%x ", (d_codeword[j+k] >> 4) & 0x0f, d_codeword[j+k] & 0x0f);
                              else
                                  printf("\t");
                              //printf("%#*2x ",2, d_codeword[j+k]);
                          }
                          printf("\t");
                          for (k=0; k<4 && j+k<d_codeword_length; k++) {
                              if (d_codeword[j+k] > 32 && d_codeword[j+k] < 128)
                                  printf("%c ", d_codeword[j+k]);
                              else
                                  printf(". ");
                          }
                          printf("\n");
                      }
                  }


                  // propagate the payload of tests were passed
//                    for (j=0; j<pl_len;j++) {
//                        out[count+j] = d_codeword[j];
//                    }
                  break;
          }
      }


      return noutput_items;
    }

  } /* namespace nuts */
} /* namespace gr */

