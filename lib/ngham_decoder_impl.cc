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

namespace gr {
  namespace nuts {

    ngham_decoder::sptr
    ngham_decoder::make(const std::string& len_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new ngham_decoder_impl(len_tag_key));
    }

    /*
     * The private constructor
     */
    ngham_decoder_impl::ngham_decoder_impl(const std::string & len_tag_key)
      : gr::tagged_stream_block("ngham_decoder",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char)), 
              len_tag_key)
    {}

    /*
     * Our virtual destructor.
     */
    ngham_decoder_impl::~ngham_decoder_impl()
    {
    }

    int
    ngham_decoder_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int noutput_items = 0;
      return noutput_items ;
    }

    int
    ngham_decoder_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      // Do <+signal processing+>

      // tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace nuts */
} /* namespace gr */

