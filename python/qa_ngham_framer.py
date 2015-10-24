#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2015 <+YOU OR YOUR COMPANY+>.
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
import nuts_swig as nuts

class qa_ngham_framer (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.tsb_key = "length"

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        #data = (0,1,2,3,4,5,6,7)
        #src = blocks.vector_source_b(data)
        #s2ts = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, len(data), self.tsb_key)
        #framer = nuts.ngham_framer(self.tsb_key)
        #sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        #self.tb.connect(src, s2ts, framer, sink)
        self.tb.run ()

        #print "data", data
        #print "packet", sink.data()[0]


if __name__ == '__main__':
    gr_unittest.run(qa_ngham_framer, "qa_ngham_framer.xml")
