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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <gnuradio/io_signature.h>
#include "ngham_encoder_impl.h"

#include "ngham.h"
#include "reed_solomon.h"

#define PDU_PORT_IN pmt::mp("in")

namespace gr {
  namespace nuts {


    ngham_encoder::sptr
    ngham_encoder::make(const std::string& len_tag_key, bool rs_encode, bool scramble, bool pad_for_usrp, bool printing)
    {
      return gnuradio::get_initial_sptr
        (new ngham_encoder_impl(len_tag_key, rs_encode, scramble, pad_for_usrp, printing));
    }

    ngham_encoder_impl::ngham_encoder_impl(const std::string& len_tag_key, bool rs_encode, bool scramble, bool pad_for_usrp, bool printing)
      : gr::tagged_stream_block("ngham_encoder",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              len_tag_key),
      d_rs_encode(rs_encode),
      d_scramble(scramble),
      d_pad_for_usrp(pad_for_usrp),
      d_printing(printing),
      d_num_packets(0),
      d_curr_len(0)
    {
      message_port_register_in(PDU_PORT_IN);

      // initialize rs tables
      for (uint8_t i=0; i<NGHAM_SIZES; i++)
          d_rs[i] = new reed_solomon(NGHAM_RS_PARITY_SIZE[i], 255-NGHAM_RS_CODEWORD_SIZE[i]);

      // initialize constant part of header
      memcpy(pkt.preamble, NGHAM_PREAMBLE, NGHAM_PREAMBLE_SIZE);
      memcpy(pkt.sync_word, NGHAM_SYNC, NGHAM_SYNC_SIZE);
    }

    ngham_encoder_impl::~ngham_encoder_impl() {
        for (uint8_t i=0; i<NGHAM_SIZES; i++) {
            delete d_rs[i];
        }
    }

    int
    ngham_encoder_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      if (d_curr_len != 0) return 0;

      bool success = get_msg();

      d_size_index = 0;
      while (d_curr_len > NGHAM_RS_DATA_SIZE[d_size_index]) d_size_index++;

      return NGHAM_HEADER_SIZE + NGHAM_RS_CODEWORD_SIZE[d_size_index];
    }

    int
    ngham_encoder_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      // see if there is anything to be done
      if (d_curr_len == 0) return 0;

      // insert size tag in header
      memcpy(pkt.size_tag, NGHAM_SIZE_TAG[d_size_index], NGHAM_SIZE_TAG_SIZE);

      uint8_t padding_size = (NGHAM_RS_DATA_SIZE[d_size_index] - d_curr_len);
      uint8_t ngham_flags = (0x00 << 5);
      pkt.rs_codeword[0] = (ngham_flags  & NGHAM_FLAGS_MASK | padding_size & NGHAM_PADDING_SIZE_MASK);

      // copy vector output and insert data in codeword
      size_t len(0);
      const uint8_t* in = (const uint8_t*) uniform_vector_elements(d_curr_vect, len);
      //copy_stream_tags();
      uint8_t data_index = NGHAM_RS_HEADER_SIZE;
      memcpy(&pkt.rs_codeword[data_index], in, d_curr_len);

      // calculate and insert crc
      uint8_t crc_index = data_index + d_curr_len;
      uint16_t crc = calc_crc(pkt.rs_codeword, d_curr_len + 1);
      pkt.rs_codeword[crc_index] = (crc >> 8) & 0xff;
      pkt.rs_codeword[crc_index + 1] = crc & 0xff;

      // insert padding
      uint8_t padding_index = crc_index + NGHAM_CRC_SIZE;
      memset(&pkt.rs_codeword[padding_index], 0, padding_size);

      // encode parity data and update packet length
      uint8_t parity_index = NGHAM_RS_DATA_SIZE_FULL[d_size_index];
      if (d_rs_encode) {
          d_rs[d_size_index]->encode(pkt.rs_codeword, &pkt.rs_codeword[parity_index]);
      } else {
          memset(&pkt.rs_codeword[parity_index], 0, NGHAM_RS_PARITY_SIZE[d_size_index]);
      }

      // print packet before scrambling
      if (d_printing) {
          print_bytes(pkt.rs_codeword, NGHAM_RS_CODEWORD_SIZE[d_size_index]);
      }

      // scramble data
      if (d_scramble)
          scramble(pkt.rs_codeword, NGHAM_RS_CODEWORD_SIZE[d_size_index]);

      // copy frame into output array
      uint8_t *out = (uint8_t *) output_items[0];
      memcpy(out, &pkt, NGHAM_HEADER_SIZE + NGHAM_RS_CODEWORD_SIZE[d_size_index]);

      // reset state
      d_curr_len = 0;

      // tell runtime system how many output items we produced.
      return NGHAM_HEADER_SIZE + NGHAM_RS_CODEWORD_SIZE[d_size_index];
    }

    bool
    ngham_encoder_impl::get_msg() {
      pmt::pmt_t msg(delete_head_blocking(PDU_PORT_IN, 100));
      if (msg.get() == NULL) {
          return false;
      }

      if (!pmt::is_pair(msg))
          throw std::runtime_error("received a malformed pdu message");

      d_curr_meta = pmt::car(msg);
      d_curr_vect = pmt::cdr(msg);
      d_curr_len = pmt::length(d_curr_vect);

      return true;
    }

    void
    ngham_encoder_impl::copy_stream_tags() {
      if (!pmt::eq(d_curr_meta, pmt::PMT_NIL) ) {
          printf("copying tags\n");
          pmt::pmt_t klist(pmt::dict_keys(d_curr_meta));
          for (size_t i = 0; i < pmt::length(klist); i++) {
              pmt::pmt_t k(pmt::nth(i, klist));
              pmt::pmt_t v(pmt::dict_ref(d_curr_meta, k, pmt::PMT_NIL));
              add_item_tag(0, nitems_written(0), k, v, alias_pmt());
          }
      }
    }

  } /* namespace nuts */
} /* namespace gr */
