#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Uhd Tx Nogui Udp
# Generated: Sat Jan 23 16:32:36 2016
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
import time


class uhd_tx_nogui_udp(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Uhd Tx Nogui Udp")

        ##################################################
        # Variables
        ##################################################
        self.sps = sps = 10
        self.samp_rate = samp_rate = 500000
        self.gain = gain = 0
        self.freq = freq = 0*int(145.98e6) + int(437.305e6)
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
        self.nuts_ngham_encoder_0 = nuts.ngham_encoder("packet_len", True, True, False, True)
        self.digital_gmsk_mod_0 = digital.gmsk_mod(
        	samples_per_symbol=sps,
        	bt=0.35,
        	verbose=False,
        	log=False,
        )
        self.blocks_socket_pdu_0 = blocks.socket_pdu("UDP_SERVER", "129.241.3.255", "5006", 10000, False)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_socket_pdu_0, 'pdus'), (self.nuts_ngham_encoder_0, 'in'))    
        self.connect((self.digital_gmsk_mod_0, 0), (self.rational_resampler_xxx_1, 0))    
        self.connect((self.nuts_ngham_encoder_0, 0), (self.digital_gmsk_mod_0, 0))    
        self.connect((self.rational_resampler_xxx_1, 0), (self.uhd_usrp_sink_0_0, 0))    

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_sink_0_0.set_samp_rate(self.samp_rate)

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


def main(top_block_cls=uhd_tx_nogui_udp, options=None):

    tb = top_block_cls()
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
