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
#include "ngham_correlator_impl.h"
#include <volk/volk.h>
#include <iostream>

#include "ngham.h"

namespace gr {
  namespace nuts {

    uint32_t size_tags[NGHAM_SIZES];


    ngham_correlator::sptr
    ngham_correlator::make(const std::string len_tag_key, int threshold)
    {
      return gnuradio::get_initial_sptr
        (new ngham_correlator_impl(len_tag_key, threshold));
    }

    /*
     * The private constructor
     */
    ngham_correlator_impl::ngham_correlator_impl(const std::string len_tag_key, int threshold)
      : gr::sync_block("ngham_correlator",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
      d_data_reg(0),
      d_threshold(threshold),
      d_len(NGHAM_SIZE_TAG_SIZE*8)
    {
        std::stringstream str;
        str << name() << unique_id();
        d_me = pmt::string_to_symbol(str.str());
        d_key = pmt::string_to_symbol(len_tag_key);
        
        int i;
        for (int size_index=0; size_index<NGHAM_SIZES; size_index++) { 
            size_tags[size_index] = 0;
            for (i=0; i<NGHAM_SIZE_TAG_SIZE; i++) {
                size_tags[size_index] = (size_tags[size_index] << 8) | (NGHAM_SIZE_TAG[size_index][i] & 0xff);
            }
        }
    }

    /*
     * Our virtual destructor.
     */
    ngham_correlator_impl::~ngham_correlator_impl()
    {
      //delete[] size_tags;
    }

    int
    ngham_correlator_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      uint64_t abs_out_sample_cnt = nitems_written(0);

      uint32_t wrong_bits, nwrong, mask = 0xffffff;
      for (int i=0; i<noutput_items; i++) {
        out[i] = in[i];
        d_data_reg = (d_data_reg << 1) | (in[i] & 0x01);
        
        wrong_bits = 0;
        nwrong = d_threshold + 1;

        for (int size_index=0; size_index<NGHAM_SIZES; size_index++) {
            wrong_bits = 0;
            nwrong = d_threshold + 1;

            wrong_bits = ((d_data_reg & mask) ^ size_tags[size_index]);
            volk_32u_popcnt(&nwrong, wrong_bits);


            if (nwrong <= d_threshold) {
                add_item_tag(0,  // stream id
                        abs_out_sample_cnt + i, // sample no
                        d_key, // frame info
                        pmt::from_long(NGHAM_CODEWORD_SIZE[size_index]*8*sizeof(unsigned char)), // packet length (unpacked bytes)
                        d_me // block source id
                        );
                // TODO should debounce to avoid mutiple tags!?
            }
        }
      }
      return noutput_items;
    }

  } /* namespace nuts */
} /* namespace gr */

