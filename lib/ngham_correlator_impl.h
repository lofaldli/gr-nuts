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

#ifndef INCLUDED_NUTS_NGHAM_CORRELATOR_IMPL_H
#define INCLUDED_NUTS_NGHAM_CORRELATOR_IMPL_H

#include <nuts/ngham_correlator.h>

namespace gr {
  namespace nuts {

    class ngham_correlator_impl : public ngham_correlator
    {
     private:
      unsigned int d_data_reg, d_size_tags[7], d_sync_word;
      bool d_verbose;
      unsigned int d_threshold, d_state, d_bit_counter;

      pmt::pmt_t d_key, d_me;
      
      void enter_sync_search();
      void enter_load_size_tag();
      void enter_size_tag_compare();

     public:
      ngham_correlator_impl(const std::string len_tag_key, int threshold, bool verbose);
      ~ngham_correlator_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace nuts
} // namespace gr

#endif /* INCLUDED_NUTS_NGHAM_CORRELATOR_IMPL_H */

