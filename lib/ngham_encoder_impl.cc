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
#include "ngham_encoder_impl.h"

#include "fec-3.0.1/fec.h"
#include "fec-3.0.1/char.h"
#include "fec-3.0.1/rs-common.h"

#include "ngham.h"
#include "crc_ccitt.h"
#include "ccsds_scrambler.h"

namespace gr {
  namespace nuts {

    ngham_encoder::sptr
    ngham_encoder::make(const std::string& len_tag_key, bool rs_encode, bool scramble, bool pad_for_usrp)
    {
      return gnuradio::get_initial_sptr
        (new ngham_encoder_impl(len_tag_key, rs_encode, scramble, pad_for_usrp));
    }
    struct rs rs_cb[NGHAM_SIZES];
    /*
     * The private constructor
     */
    ngham_encoder_impl::ngham_encoder_impl(const std::string& len_tag_key, bool rs_encode, bool scramble, bool pad_for_usrp)
      : gr::tagged_stream_block("ngham_encoder",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)), 
              len_tag_key),
      d_rs_encode(rs_encode),
      d_scramble(scramble),
      d_pad_for_usrp(pad_for_usrp)
    {
      printf("ngham_encoder_impl()\n");
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
        // FIXME causes core dump 
        //free_rs_char(&rs_cb);
        printf("~!!!!!!!!!!!ngham_encoder_impl()\n");
        //for (int j=0; j<NGHAM_SIZES; j++) {
            delete [] rs_cb[6].alpha_to;
            delete [] rs_cb[6].index_of;
            delete [] rs_cb[6].genpoly;
            delete [] rs_cb[0].alpha_to;
            delete [] rs_cb[0].index_of;
            delete [] rs_cb[0].genpoly;
        //}
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

      // calculate size index
      const int pl_len = ninput_items[0];
      int size_index = 0;
      while (pl_len > NGHAM_PL_SIZE[size_index]) size_index++;

      // calc beginning of rs codeword
      int codeword_start = NGHAM_PREAMBLE_SIZE + NGHAM_SYNC_SIZE + NGHAM_SIZE_TAG_SIZE;

      int j;
      // insert preamble, sync and size tag
      for (j=0; j<NGHAM_PREAMBLE_SIZE; j++) out[noutput_items++] = NGHAM_PREAMBLE[j];
      for (j=0; j<NGHAM_SYNC_SIZE; j++)     out[noutput_items++] = NGHAM_SYNC[j];
      for (j=0; j<NGHAM_SIZE_TAG_SIZE; j++) out[noutput_items++] = NGHAM_SIZE_TAG[size_index][j];
      
      // calculate and insert padding size 
      unsigned char padding_size = (NGHAM_PL_SIZE[size_index] - pl_len) & 0x1f; // 0001 1111
      // TODO NGHAM flags
      unsigned char ngham_flags = (0x00 << 5) & 0xe0;
      out[noutput_items++] = (ngham_flags | padding_size);

      // insert input elements
      for (j=0; j<pl_len; j++) out[noutput_items++] = in[j];

      // calculate and insert crc
      const int crc = crc_ccitt(&out[codeword_start], pl_len+1);
      out[noutput_items++] = (crc >> 8) & 0xff;
      out[noutput_items++] = crc & 0xff;

      // insert padding
      for (j=0; j<padding_size; j++) out[noutput_items++] = 0;

      // encode parity data and update packet length
      if (d_rs_encode) {
        encode_rs_char(&rs_cb[size_index], &out[codeword_start], &out[noutput_items]);
        noutput_items += NGHAM_PAR_SIZE[size_index];
      }

      // scramble data
      if (d_scramble) 
        for (j=0; j<noutput_items; j++) out[codeword_start + j] ^= ccsds_poly[j];

      // make sure packet is multiple of 128 bytes
      if (d_pad_for_usrp) {
        int total_padded_length = 128;
        while (noutput_items > total_padded_length) total_padded_length += 128;

        int usrp_padding_size = total_padded_length - (noutput_items % 128);
        for (j=0; j<usrp_padding_size; j++) out[noutput_items++] = 0;
      }

      // tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace nuts */
} /* namespace gr */

