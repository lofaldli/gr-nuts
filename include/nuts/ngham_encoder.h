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


#ifndef INCLUDED_NUTS_NGHAM_ENCODER_H
#define INCLUDED_NUTS_NGHAM_ENCODER_H

#include <nuts/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace nuts {

    /*!
     * \brief ngham encoder block
     * \ingroup nuts
     *
     */
    class NUTS_API ngham_encoder : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<ngham_encoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of nuts::ngham_encoder.
       *
       * To avoid accidental use of raw pointers, nuts::ngham_encoder's
       * constructor is in a private implementation
       * class. nuts::ngham_encoder::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::string& len_tag_key="packet_length", bool rs_encode=true, bool scramble=true, bool pad_for_usrp=true, bool printing=false);
    };

  } // namespace nuts
} // namespace gr

#endif /* INCLUDED_NUTS_NGHAM_ENCODER_H */

