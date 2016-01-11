#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Uhd Tx Nogui
# Generated: Mon Jan 11 23:19:11 2016
##################################################

from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import nuts
import pmt
import time


class uhd_tx_nogui(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Uhd Tx Nogui")

        ##################################################
        # Variables
        ##################################################
        self.message = message = "LA1NGS this is an NGHAM test message"
        self.data = data = [ord(x) for x in message]
        self.sps = sps = 10
        self.samp_rate = samp_rate = 2000000
        self.length_tag = length_tag = gr.tag_utils.python_to_tag((0, pmt.intern("packet_len"), pmt.from_long(len(data)), pmt.intern("src")))
        self.gain = gain = 30
        self.freq = freq = int(145.98e6)
        self.data_rate = data_rate = 9600

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_sink_0_0 = uhd.usrp_sink(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_sink_0_0.set_samp_rate(samp_rate)
        self.uhd_usrp_sink_0_0.set_center_freq(freq, 0)
        self.uhd_usrp_sink_0_0.set_gain(gain, 0)
        self.rational_resampler_xxx_1 = filter.rational_resampler_ccc(
                interpolation=samp_rate,
                decimation=sps*data_rate,
                taps=None,
                fractional_bw=None,
        )
        self.nuts_ngham_encoder_0 = nuts.ngham_encoder("packet_len", True, True, True, False)
        self.digital_gmsk_mod_0 = digital.gmsk_mod(
        	samples_per_symbol=sps,
        	bt=0.35,
        	verbose=False,
        	log=False,
        )
        self.blocks_vector_source_x_0_0 = blocks.vector_source_b(data, True, 1, [length_tag])
        self.blocks_tagged_stream_align_0 = blocks.tagged_stream_align(gr.sizeof_char*1, "packet_len")

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_tagged_stream_align_0, 0), (self.digital_gmsk_mod_0, 0))    
        self.connect((self.blocks_vector_source_x_0_0, 0), (self.nuts_ngham_encoder_0, 0))    
        self.connect((self.digital_gmsk_mod_0, 0), (self.rational_resampler_xxx_1, 0))    
        self.connect((self.nuts_ngham_encoder_0, 0), (self.blocks_tagged_stream_align_0, 0))    
        self.connect((self.rational_resampler_xxx_1, 0), (self.uhd_usrp_sink_0_0, 0))    

    def get_message(self):
        return self.message

    def set_message(self, message):
        self.message = message
        self.set_data([ord(x) for x in self.message])

    def get_data(self):
        return self.data

    def set_data(self, data):
        self.data = data
        self.set_length_tag(gr.tag_utils.python_to_tag((0, pmt.intern("packet_len"), pmt.from_long(len(self.data)), pmt.intern("src"))))
        self.blocks_vector_source_x_0_0.set_data(self.data, [self.length_tag])

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_sink_0_0.set_samp_rate(self.samp_rate)

    def get_length_tag(self):
        return self.length_tag

    def set_length_tag(self, length_tag):
        self.length_tag = length_tag
        self.blocks_vector_source_x_0_0.set_data(self.data, [self.length_tag])

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self.uhd_usrp_sink_0_0.set_gain(self.gain, 0)
        	

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.uhd_usrp_sink_0_0.set_center_freq(self.freq, 0)

    def get_data_rate(self):
        return self.data_rate

    def set_data_rate(self, data_rate):
        self.data_rate = data_rate


def main(top_block_cls=uhd_tx_nogui, options=None):

    tb = top_block_cls()
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
