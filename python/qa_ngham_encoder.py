#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2015 André Løfaldli.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pmt, time
import nuts_swig as nuts

enable_vw_testing = False

class qa_ngham_encoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.tsb_key = "packet_len"
    def tearDown (self):
        self.tb = None

    def test_000 (self):
        if enable_vw_testing:
            return

        # this function will check that the encoder produces correct packet lengths
        payload_sizes = [28, 60, 92, 124, 156, 188, 220]
        codeword_sizes = [47, 79, 111, 159, 191, 223 ,255]

        enc = nuts.ngham_encoder(self.tsb_key, scramble=False, pad_for_usrp=False)
        snk = blocks.vector_sink_b()
        self.tb.connect(enc, snk)
        port = pmt.intern("in")

        self.tb.start()
        for i in range(7):
            snk.reset()
            src_data = [x for x in range(payload_sizes[i])]
            src_vec = pmt.init_u8vector(len(src_data), src_data)
            msg = pmt.cons( pmt.PMT_NIL, src_vec )
            enc.to_basic_block()._post(port, msg)

            time.sleep(0.1)

            res_data = snk.data()
            print len(src_data), "=>", len(res_data), "(", codeword_sizes[i] + 11,")"
            self.assertEqual(len(res_data), codeword_sizes[i]+11)

        self.tb.stop()
        self.tb.wait()


if __name__ == '__main__':
    gr_unittest.run(qa_ngham_encoder, "qa_ngham_encoder.xml")
