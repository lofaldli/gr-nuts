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

#ifndef INCLUDED_NUTS_NGHAM_DECODER_IMPL_H
#define INCLUDED_NUTS_NGHAM_DECODER_IMPL_H

#include <nuts/ngham_decoder.h>

namespace gr {
  namespace nuts {

    class ngham_decoder_impl : public ngham_decoder
    {
     private:
      bool d_rs_decode;
      bool d_descramble;
      bool d_printing;
      int d_decoder_state;
      unsigned long d_data_reg;
      int d_size_index;
      int d_codeword_length;
      unsigned char d_codeword[255]; // FIXME NGHAM_MAX_CODEWORD_SIZE
      char d_bit_counter;

      void enter_preamble();
      void enter_sync();
      void enter_size_tag();
      void enter_codeword();
      void enter_decode();

     public:
      ngham_decoder_impl(bool rs_decode, bool descramble, bool printing);
      ~ngham_decoder_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace nuts
} // namespace gr

#endif /* INCLUDED_NUTS_NGHAM_DECODER_IMPL_H */

