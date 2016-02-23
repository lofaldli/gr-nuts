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
#include "ngham_correlator_impl.h"
#include <volk/volk.h>
#include <iostream>

#include "ngham.h"


#define STATE_SYNC_SEARCH 0
#define STATE_LOAD_SIZE_TAG 1
#define STATE_SIZE_TAG_COMPARE 2

namespace gr {
  namespace nuts {

//    uint32_t sync_word, size_tags[NGHAM_SIZES];

    ngham_correlator::sptr
    ngham_correlator::make(const std::string len_tag_key, int threshold, bool verbose)
    {
      return gnuradio::get_initial_sptr
        (new ngham_correlator_impl(len_tag_key, threshold, verbose));
    }

    /*
     * The private constructor
     */
    ngham_correlator_impl::ngham_correlator_impl(const std::string len_tag_key, int threshold, bool verbose)
      : gr::sync_block("ngham_correlator",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),
      d_data_reg(0),
      d_threshold(threshold),
      d_verbose(verbose)
      //d_len(NGHAM_SIZE_TAG_SIZE*8)
    {
        std::stringstream str;
        str << name() << unique_id();
        d_me = pmt::string_to_symbol(str.str());
        d_key = pmt::string_to_symbol(len_tag_key);

        d_sync_word = 0;
        for (int i=0; i<NGHAM_SYNC_SIZE; i++) {
            d_sync_word = (d_sync_word << 8) | (NGHAM_SYNC[i] & 0xff);
        }

        for (int size_index=0; size_index<NGHAM_SIZES; size_index++) {
            d_size_tags[size_index] = 0;
            for (int i=0; i<NGHAM_SIZE_TAG_SIZE; i++) {
                d_size_tags[size_index] = (d_size_tags[size_index] << 8) | (NGHAM_SIZE_TAG[size_index][i] & 0xff);
            }
        }
        enter_sync_search();
    }

    /*
     * Our virtual destructor.
     */
    ngham_correlator_impl::~ngham_correlator_impl()
    {
      //delete[] size_tags;
    }

    void ngham_correlator_impl::enter_sync_search() {
        if (d_verbose) printf("enter_sync_search\n");
        d_state = STATE_SYNC_SEARCH;
        d_data_reg = 0;
    }
    void ngham_correlator_impl::enter_load_size_tag() {
        if (d_verbose) printf("enter_load_size_tag");
        d_state = STATE_LOAD_SIZE_TAG;
        d_bit_counter = 0;
    }
    void ngham_correlator_impl::enter_size_tag_compare() {
        if(d_verbose) printf("enter_size_tag_compare\n");
        d_state = STATE_SIZE_TAG_COMPARE;
    }

    int
    ngham_correlator_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      unsigned long abs_out_sample_cnt = nitems_written(0);

      unsigned int wrong_bits, nwrong;
      for (int i=0; i<noutput_items; i++) {

        switch (d_state) {

            case STATE_SYNC_SEARCH:
                out[i] = in[i];
                d_data_reg = (d_data_reg << 1) | (in[i] & 0x01);

                wrong_bits = 0;
                nwrong = d_threshold + 1;

                wrong_bits = (d_data_reg ^ d_sync_word);
                volk_32u_popcnt(&nwrong, wrong_bits);

                if (nwrong <= d_threshold) {
                    if (d_verbose) printf("\tfound size tag with %i bits wrong\n", nwrong);
                    enter_load_size_tag();
                }
                break;
            case STATE_LOAD_SIZE_TAG:
                out[i] = in[i];
                d_data_reg = (d_data_reg << 1) | (in[i] & 0x01);
                d_bit_counter++;

                if (d_bit_counter == NGHAM_SIZE_TAG_SIZE * 8) {
                    enter_size_tag_compare();
                }
                break;
            case STATE_SIZE_TAG_COMPARE:

                for (int size_index=0; size_index<NGHAM_SIZES; size_index++) {
                    wrong_bits = 0;
                    nwrong = d_threshold + 1;

                    // the size tag is only 3 bytes, so the data register needs to be masked
                    wrong_bits = ((d_data_reg & 0xffffff) ^ d_size_tags[size_index]);
                    volk_32u_popcnt(&nwrong, wrong_bits);

                    if (d_verbose)
                        printf("\tcomparing %x and %x, %i bits are different\n", d_data_reg & 0xffffff,
                               d_size_tags[size_index], nwrong);

                    if (nwrong <= d_threshold) {
                        add_item_tag(0,  // stream id
                                abs_out_sample_cnt + i, // sample no
                                d_key, // frame info
                                pmt::from_long(NGHAM_RS_CODEWORD_SIZE[size_index]*8), // packet length (unpacked bytes)
                                d_me // block source id
                                );
                        break;
                     }
                }
                enter_sync_search();
                break;

        }
      }
      return noutput_items;
    }

  } /* namespace nuts */
} /* namespace gr */

