/* -*- c++ -*- */
/*
 * Copyright 2015, 2016 André Løfaldli.
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

#ifndef INCLUDED_NUTS_NGHAM_ENCODER_IMPL_H
#define INCLUDED_NUTS_NGHAM_ENCODER_IMPL_H

#include <nuts/ngham_encoder.h>
#include <stdint.h>
#include "ngham.h"
#include "reed_solomon.h"

namespace gr {
  namespace nuts {

    class ngham_encoder_impl : public ngham_encoder
    {
     private:
      bool d_rs_encode;
      bool d_scramble;
      bool d_pad_for_usrp;
      bool d_printing;

      ngham_tx_pkt pkt;
      reed_solomon* d_rs[NGHAM_SIZES];

      uint8_t d_size_index;
      uint16_t d_num_packets;

      pmt::pmt_t           d_curr_meta;
      pmt::pmt_t           d_curr_vect;
      size_t               d_curr_len;

      bool get_msg();
      void copy_stream_tags();

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      ngham_encoder_impl(const std::string& len_tag_key, bool rs_encode, bool scramble, bool pad_for_usrp, bool printing);
      ~ngham_encoder_impl();

      // Where all the action really happens
      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace nuts
} // namespace gr

#endif /* INCLUDED_NUTS_NGHAM_ENCODER_IMPL_H */

