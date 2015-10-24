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
#include "ngham_codeword_generator_impl.h"

#include "ngham.h"
#include "crc_ccitt.h"

namespace gr {
  namespace nuts {

    ngham_codeword_generator::sptr
    ngham_codeword_generator::make(const std::string& len_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new ngham_codeword_generator_impl(len_tag_key));
    }

    /*
     * The private constructor
     */
    ngham_codeword_generator_impl::ngham_codeword_generator_impl(const std::string& len_tag_key)
      : gr::tagged_stream_block("ngham_codeword_generator",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)), 
              len_tag_key)
    {}

    /*
     * Our virtual destructor.
     */
    ngham_codeword_generator_impl::~ngham_codeword_generator_impl()
    {
    }

    int
    ngham_codeword_generator_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      const int pl_len = ninput_items[0];
      int size_index = 0;
      while (pl_len > NGHAM_PL_SIZE[size_index]) size_index++;

      return NGHAM_PL_SIZE[size_index]; // should be NGHAM_PL_SIZE_FULL ??
    }

    int
    ngham_codeword_generator_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      

      int len = 0;
      int j;

      const int pl_len = ninput_items[0];
      int size_index = 0;
      while (pl_len > NGHAM_PL_SIZE[size_index]) size_index++;
      
      // insert padding size
      char padding_size = NGHAM_PL_SIZE[size_index] - pl_len;
      out[len++] = padding_size & 0x1f;

      // insert data
      for (j=0; j < pl_len; j++) out[len++] = in[j];

      // insert crc
      unsigned int crc = crc_ccitt(out, len); // was originally len+1
      out[len++] = (crc >> 8) & 0xff;
      out[len++] = crc & 0xff;

      // insert padding
      for (j=0; j < padding_size; j++) out[len++] = 0x00;


      // Tell runtime system how many output items we produced.
      return len;
    }

  } /* namespace nuts */
} /* namespace gr */

