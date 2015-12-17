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

#include <stdio.h>
#include <iostream>
#include <string>
#include <gnuradio/io_signature.h>
#include "ngham_encoder_impl.h"

#include "fec-3.0.1/fec.h"
#include "fec-3.0.1/char.h"
#include "fec-3.0.1/rs-common.h"

#include "ngham.h"

#define DEBUG

#ifdef DEBUG
#define PRINT(str) (printf(str))
#define PRINT1(str,arg) (printf(str,arg))
#define PRINT2(str,arg,arg1) (printf(str,arg,arg1))
#else
#define PRINT(str)
#define PRINT1(str,arg)
#define PRINT2(str,arg,arg1)
#endif

namespace gr {
  namespace nuts {

    ngham_encoder::sptr
    ngham_encoder::make(const std::string& len_tag_key, bool rs_encode, bool scramble, bool pad_for_usrp, bool printing)
    {
      return gnuradio::get_initial_sptr
        (new ngham_encoder_impl(len_tag_key, rs_encode, scramble, pad_for_usrp, printing));
    }
    struct rs rs_cb[NGHAM_SIZES];
    /*
     * The private constructor
     */
    ngham_encoder_impl::ngham_encoder_impl(const std::string& len_tag_key, bool rs_encode, bool scramble, bool pad_for_usrp, bool printing)
      : gr::tagged_stream_block("ngham_encoder",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)), 
              len_tag_key),
      d_rs_encode(rs_encode),
      d_scramble(scramble),
      d_pad_for_usrp(pad_for_usrp),
      d_printing(printing),
      d_num_packets(0)
    {
      // initialize rs tables
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
    }


    /*
     * Our virtual destructor.
     */
    ngham_encoder_impl::~ngham_encoder_impl()
    {
      delete [] rs_cb[6].alpha_to;
      delete [] rs_cb[6].index_of;
      delete [] rs_cb[6].genpoly;
      delete [] rs_cb[0].alpha_to;
      delete [] rs_cb[0].index_of;
      delete [] rs_cb[0].genpoly;
    }

    int
    ngham_encoder_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int header_size = NGHAM_PREAMBLE_SIZE + NGHAM_SYNC_SIZE + NGHAM_SIZE_TAG_SIZE;
      const int pl_len = ninput_items[0];

      int size_index = 0;
      while (pl_len > NGHAM_PL_SIZE[size_index]) size_index++;

      int noutput_items = header_size + NGHAM_PL_SIZE[size_index];

      if (d_pad_for_usrp) {
        int total_padded_length = 128;
        while (noutput_items > total_padded_length) total_padded_length += 128;

        noutput_items = total_padded_length;
      }

      return noutput_items;
    }

    int
    ngham_encoder_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      // pointer to input elements
      const unsigned char *in = (const unsigned char *) input_items[0];

      // pointer to output elements
      unsigned char *out = (unsigned char *) output_items[0];
      
      // initialize number of output items
      noutput_items = 0;

      unsigned char d[NGHAM_MAX_TOTAL_SIZE];
      int d_len = 0;

      // calculate size index
      const int pl_len = ninput_items[0];
      int size_index = 0;
      while (pl_len > NGHAM_PL_SIZE[size_index]) size_index++;

      // calc beginning of rs codeword
      int codeword_start = NGHAM_PREAMBLE_SIZE + NGHAM_SYNC_SIZE + NGHAM_SIZE_TAG_SIZE;

      // insert preamble, sync and size tag
      for (int i=0; i<NGHAM_PREAMBLE_SIZE; i++) d[d_len++] = NGHAM_PREAMBLE[i];
      for (int i=0; i<NGHAM_SYNC_SIZE; i++)     d[d_len++] = NGHAM_SYNC[i];
      for (int i=0; i<NGHAM_SIZE_TAG_SIZE; i++) d[d_len++] = NGHAM_SIZE_TAG[size_index][i];
      
      // calculate and insert padding size and ngham flags
      unsigned char padding_size = (NGHAM_PL_SIZE[size_index] - pl_len) & 0x1f; // 0001 1111
      unsigned char ngham_flags = (0x00 << 5) & 0xe0;// TODO implement NGHAM flags
      d[d_len++] = (ngham_flags | padding_size);

      // insert input elements
      for (int i=0; i<pl_len; i++) d[d_len++] = in[i];

      // calculate and insert crc
      unsigned int crc = 0xffff;
      for (int i=0; i<pl_len+1; i++){
        crc = ((crc >> 8) & 0xff) ^ crc_ccitt_table[(crc ^ d[codeword_start+i]) & 0xff];
      }
      crc ^= 0xffff;

      d[d_len++] = (crc >> 8) & 0xff;
      d[d_len++] = crc & 0xff;

      // insert padding
      for (int i=0; i<padding_size; i++) d[d_len++] = 0;

      // encode parity data and update packet length
      if (d_rs_encode) {
        encode_rs_char(&rs_cb[size_index], &d[codeword_start], &d[d_len]);
        d_len += NGHAM_PAR_SIZE[size_index];
      }

      d_num_packets++;
      // print packet before scrambling
      if (d_printing) {
                      
          printf("number of packets sent %i\n", d_num_packets);
          printf("encoded data:\n");

          for (int i=codeword_start; i<d_len; i+=8) {
              printf("\t");
              for (int j=0; j<8; j++) {
              if (i+j<d_len)
                  printf("0x%x%x ", (d[i+j] >> 4) & 0x0f, d[i+j] & 0x0f);
              else
                  printf("\t");
              }
              printf("\t");
              for (int j=0; j<8 && i+j<d_len; j++) {
                  if (d[i+j] > 32 && d[i+j] < 128)
                      printf("%c ", d[i+j]);
                  else
                      printf(". ");
                  }
                  printf("\n");
              }
           }

      // scramble data
      if (d_scramble) 
        for (int i=0; i<d_len; i++) d[codeword_start + i] ^= ccsds_poly[i];


      // make sure packet is multiple of 128 bytes
      if (d_pad_for_usrp) {
        int total_padded_length = 128;
        while (d_len > total_padded_length) total_padded_length += 128;

        int usrp_padding_size = total_padded_length - (d_len % 128);
        for (int i=0; i<usrp_padding_size; i++) d[d_len++] = 0;
      }

      memcpy(out, d, d_len);

      // tell runtime system how many output items we produced.
      noutput_items = d_len;
      return noutput_items;
    }

  } /* namespace nuts */
} /* namespace gr */

