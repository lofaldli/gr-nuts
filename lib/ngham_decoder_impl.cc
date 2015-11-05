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

#include <stdio.h>
#include <gnuradio/io_signature.h>
#include "ngham_decoder_impl.h"
#include <volk/volk.h>

#include "fec-3.0.1/fec.h"
#include "fec-3.0.1/char.h"
#include "fec-3.0.1/rs-common.h"

#include "ngham.h"
//#include "crc_ccitt.h"
//#include "ccsds_scrambler.h"


#define STATE_PREAMBLE 0
#define STATE_SYNC 1
#define STATE_SIZE_TAG 2
#define STATE_CODEWORD 3
#define STATE_CODEWORD_DONE 4


namespace gr {
  namespace nuts {

    ngham_decoder::sptr
    ngham_decoder::make(bool rs_decode, bool descramble, bool printing)
    {
      return gnuradio::get_initial_sptr
        (new ngham_decoder_impl(rs_decode, descramble, printing));
    }

    /*
     * The private constructor
     */
    ngham_decoder_impl::ngham_decoder_impl(bool rs_decode, bool descramble, bool printing)
      : gr::block("ngham_decoder",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
        d_rs_decode(rs_decode),
        d_descramble(descramble),
        d_printing(printing),
        d_decoder_state(STATE_PREAMBLE),
        d_data_reg(0),
        d_size_index(0),
        d_bit_counter(0)
    {
/*      // initialize rs tables
      struct rs* rs_32 = (struct rs*)init_rs_char(8, 0x187, 112, 11, 32, 0);
      memcpy( (void*)&rs_cb[6], (void*)rs_32, sizeof(rs_cb[6]) );     
      memcpy( (void*)&rs_cb[5], (void*)rs_32, sizeof(rs_cb[5]) );     
      memcpy( (void*)&rs_cb[4], (void*)rs_32, sizeof(rs_cb[4]) );     
      memcpy( (void*)&rs_cb[3], (void*)rs_32, sizeof(rs_cb[3]) );     
    
      struct rs* rs_16 = (struct rs*)init_rs_char(8, 0x187, 112, 11, 16, 0);
      memcpy( (void*)&rs_cb[2], (void*)rs_16, sizeof(rs_cb[2]) );     
      memcpy( (void*)&rs_cb[1], (void*)rs_16, sizeof(rs_cb[1]) );     
      memcpy( (void*)&rs_cb[0], (void*)rs_16, sizeof(rs_cb[0]) );     

      rs_cb[6].pad = 255-NGHAM_CODEWORD_SIZE[6];
      rs_cb[5].pad = 255-NGHAM_CODEWORD_SIZE[5];
      rs_cb[4].pad = 255-NGHAM_CODEWORD_SIZE[4];
      rs_cb[3].pad = 255-NGHAM_CODEWORD_SIZE[3];
      rs_cb[2].pad = 255-NGHAM_CODEWORD_SIZE[2];
      rs_cb[1].pad = 255-NGHAM_CODEWORD_SIZE[1];
      rs_cb[0].pad = 255-NGHAM_CODEWORD_SIZE[0];

      delete rs_32;
      delete rs_16;
*/
    }

    /*
     * Our virtual destructor.
     */
    ngham_decoder_impl::~ngham_decoder_impl()
    { 
/*      
      delete [] rs_cb[6].alpha_to;
      delete [] rs_cb[6].index_of;
      delete [] rs_cb[6].genpoly;
      delete [] rs_cb[0].alpha_to;
      delete [] rs_cb[0].index_of;
      delete [] rs_cb[0].genpoly;
*/
    }
/*
    void
    ngham_decoder_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      switch (d_decoder_state) {
          case STATE_PREAMBLE:
              ninput_items_required = 1
              break;
          case STATE_SYNC:
              break;
          case STATE_SIZE_TAG:
              break;
          case STATE_PAYLOAD:
              break;
      }
    }*/

    void ngham_decoder_impl::enter_preamble() {
      d_decoder_state = STATE_PREAMBLE;
    }
    void ngham_decoder_impl::enter_sync() {
      d_decoder_state = STATE_SYNC;
    }
    void ngham_decoder_impl::enter_size_tag() {
      d_decoder_state = STATE_SIZE_TAG;
    }
    void ngham_decoder_impl::enter_codeword() {
      d_decoder_state = STATE_CODEWORD;
      d_codeword_length = 0;
      d_bit_counter = 0;
    }

    uint8_t ngham_tag_check(uint32_t x, uint32_t y){
      uint8_t j, distance;
      uint32_t diff;
      diff = x^y;
      if (!diff) return 1;    // Early check to save time

      distance = 0;
      for (j=0; j<24; j++){
        if (diff & 0x01){
            distance++;
          if (distance > NGHAM_SIZE_TAG_MAX_ERROR) return 0;
        }
        diff >>= 1;
      }
      return 1;
    }

    int
    ngham_decoder_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      int count = 0;
      int j,k;
      unsigned long access_code;

      while (count < noutput_items) {

          switch (d_decoder_state) {
              case STATE_PREAMBLE:

                  while (count < noutput_items) {
                    out[count] = 0;//in[count]; // dont propagate preamble
                    d_data_reg = (d_data_reg << 1) | ((in[count++]) & 0x01);
                    
                    uint32_t wrong_bits = 0;
                    uint32_t nwrong = 1; // TODO threshold

                    // FIXME make the conversion from byte array to long prettier
                    for (j=0; j<NGHAM_PREAMBLE_SIZE; j++) {
                      access_code = (access_code << 8) | (NGHAM_PREAMBLE[j] & 0xff);
                    }

                    wrong_bits = (d_data_reg ^ access_code);
                    
                    volk_32u_popcnt(&nwrong, wrong_bits);
                    if (nwrong <= 0) {
                      printf("preamble found!\n");
                      enter_sync();
                      break;
                    }
                  }
                  break;
              case STATE_SYNC:
                  while (count < noutput_items) {
                    out[count] = 0;//in[count]; // dont propagate sync
                    d_data_reg = (d_data_reg << 1) | ((in[count++]) & 0x01);
                    
                    uint32_t wrong_bits = 0;
                    uint32_t nwrong = 1; 

                    // FIXME make the conversion from byte array to long prettier
                    for (j=0; j<NGHAM_SYNC_SIZE; j++) {
                      access_code = (access_code << 8) | (NGHAM_SYNC[j] & 0xff);
                    }

                    wrong_bits = (d_data_reg ^ access_code);
                    
                    volk_32u_popcnt(&nwrong, wrong_bits);
                    if (nwrong <= 0) {
                      printf("sync found!\n");
                      enter_size_tag();
                      break;
                    }
                  }
                  break;
              case STATE_SIZE_TAG:
                  // FIXME leave this state in a proper way
                  while (d_decoder_state == STATE_SIZE_TAG && count < noutput_items) {
                    out[count] = 0;//in[count]; // dont propagate size tag
                    d_data_reg = (d_data_reg << 1) | ((in[count++]) & 0x01);
                    
                    d_size_index = 0;
                    while (d_size_index < NGHAM_SIZES) {
                      uint32_t wrong_bits = 0;
                      uint32_t nwrong = 1; 

                      // FIXME make the conversion from byte array to long prettier
                      access_code = 0;
                      for (j=0; j<NGHAM_SIZE_TAG_SIZE; j++) { 
                        access_code = (access_code << 8) | (NGHAM_SIZE_TAG[d_size_index][j] & 0xff);
                      }
                      wrong_bits = ((d_data_reg & 0xffffff) ^ access_code);
                    
                      volk_32u_popcnt(&nwrong, wrong_bits);
                      if (nwrong <= 0) {
                      //if (ngham_tag_check(access_code, d_data_reg & 0xffffff)){
                        printf("size tag found! size %i\n", d_size_index);
                        enter_codeword();
                        break;
                      }
                      d_size_index++;

                    }
                    
                    
                  }
                  break;
              case STATE_CODEWORD:
                  while (count < noutput_items) {
                    out[count] = in[count];

                    
                      d_codeword[d_codeword_length] = (d_codeword[d_codeword_length] << 1) | (in[count++] & 0x01);
                      d_bit_counter++;
                      if (d_bit_counter == 8) {
                        d_codeword_length++;
                        d_bit_counter = 0;    
                      }
                      
                    
                    
                    
                    if (d_codeword_length == NGHAM_CODEWORD_SIZE[d_size_index]) {
                      enter_preamble();
                      printf("codeword length %i\n", d_codeword_length);
                      for (j=0; j<d_codeword_length; j++) {
                        //printf("%i ", d_codeword[j]);
                        printf("0x%x%x ", (d_codeword[j] >> 4) & 0x0f, d_codeword[j] & 0x0f);fflush(stdout);
                      }
                      printf("\n");

                      break;
                    }
                  }

                  // TODO decode packet

                  
                  
                  break;
          }
          
      }
      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
    
    
  } /* namespace nuts */
} /* namespace gr */

