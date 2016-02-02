#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Uhd Bent Pipe
# Generated: Tue Jan 26 18:06:47 2016
##################################################

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


class uhd_bent_pipe(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Uhd Bent Pipe")

        ##################################################
        # Variables
        ##################################################
        self.xlat_bandwidth = xlat_bandwidth = 10000
        self.samp_rate_rx = samp_rate_rx = int(2e6)
        self.freq_rx = freq_rx = int(145.98e6)
        self.xlat_taps = xlat_taps = firdes.low_pass(1, samp_rate_rx, xlat_bandwidth/2, 1000)
        self.xlat_decim = xlat_decim = 8
        self.tuner = tuner = 0
        self.sps_tx = sps_tx = 10
        self.sps_rx = sps_rx = 10
        self.samp_rate_tx = samp_rate_tx = int(2e6)
        self.freq_tx = freq_tx = int(146.18e6)*0 + freq_rx
        self.data_rate = data_rate = 9600

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(samp_rate_rx)
        self.uhd_usrp_source_0.set_center_freq(freq_rx, 0)
        self.uhd_usrp_source_0.set_gain(50, 0)
        self.uhd_usrp_source_0.set_antenna("RX2", 0)
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_sink_0.set_samp_rate(samp_rate_tx)
        self.uhd_usrp_sink_0.set_center_freq(freq_tx, 0)
        self.uhd_usrp_sink_0.set_gain(30, 0)
        self.uhd_usrp_sink_0.set_antenna("TX/RX", 0)
        self.rational_resampler_xxx_1 = filter.rational_resampler_ccc(
                interpolation=samp_rate_tx,
                decimation=data_rate*sps_tx,
                taps=None,
                fractional_bw=None,
        )
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=sps_rx*data_rate,
                decimation=samp_rate_rx/xlat_decim,
                taps=None,
                fractional_bw=None,
        )
        self.nuts_ngham_encoder_0 = nuts.ngham_encoder("packet_len", True, True, False, False)
        self.nuts_ngham_decoder_0 = nuts.ngham_decoder("packet_len", 0, True, True, True, False)
        self.freq_xlating_fir_filter_xxx_0_1 = filter.freq_xlating_fir_filter_ccc(xlat_decim, (firdes.low_pass(1, samp_rate_rx, xlat_bandwidth/2, 1000)), tuner, samp_rate_rx)
        self.digital_gmsk_mod_0 = digital.gmsk_mod(
        	samples_per_symbol=sps_tx,
        	bt=0.35,
        	verbose=False,
        	log=False,
        )
        self.digital_gmsk_demod_0 = digital.gmsk_demod(
        	samples_per_symbol=sps_rx,
        	gain_mu=0.175,
        	mu=0.5,
        	omega_relative_limit=0.005,
        	freq_error=0.0,
        	verbose=False,
        	log=False,
        )

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.nuts_ngham_decoder_0, 'out'), (self.nuts_ngham_encoder_0, 'in'))    
        self.connect((self.digital_gmsk_demod_0, 0), (self.nuts_ngham_decoder_0, 0))    
        self.connect((self.digital_gmsk_mod_0, 0), (self.rational_resampler_xxx_1, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0_1, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.nuts_ngham_encoder_0, 0), (self.digital_gmsk_mod_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.digital_gmsk_demod_0, 0))    
        self.connect((self.rational_resampler_xxx_1, 0), (self.uhd_usrp_sink_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.freq_xlating_fir_filter_xxx_0_1, 0))    

    def get_xlat_bandwidth(self):
        return self.xlat_bandwidth

    def set_xlat_bandwidth(self, xlat_bandwidth):
        self.xlat_bandwidth = xlat_bandwidth
        self.set_xlat_taps(firdes.low_pass(1, self.samp_rate_rx, self.xlat_bandwidth/2, 1000))
        self.freq_xlating_fir_filter_xxx_0_1.set_taps((firdes.low_pass(1, self.samp_rate_rx, self.xlat_bandwidth/2, 1000)))

    def get_samp_rate_rx(self):
        return self.samp_rate_rx

    def set_samp_rate_rx(self, samp_rate_rx):
        self.samp_rate_rx = samp_rate_rx
        self.set_xlat_taps(firdes.low_pass(1, self.samp_rate_rx, self.xlat_bandwidth/2, 1000))
        self.freq_xlating_fir_filter_xxx_0_1.set_taps((firdes.low_pass(1, self.samp_rate_rx, self.xlat_bandwidth/2, 1000)))
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate_rx)

    def get_freq_rx(self):
        return self.freq_rx

    def set_freq_rx(self, freq_rx):
        self.freq_rx = freq_rx
        self.uhd_usrp_source_0.set_center_freq(self.freq_rx, 0)
        self.set_freq_tx(int(146.18e6)*0 + self.freq_rx)

    def get_xlat_taps(self):
        return self.xlat_taps

    def set_xlat_taps(self, xlat_taps):
        self.xlat_taps = xlat_taps

    def get_xlat_decim(self):
        return self.xlat_decim

    def set_xlat_decim(self, xlat_decim):
        self.xlat_decim = xlat_decim

    def get_tuner(self):
        return self.tuner

    def set_tuner(self, tuner):
        self.tuner = tuner
        self.freq_xlating_fir_filter_xxx_0_1.set_center_freq(self.tuner)

    def get_sps_tx(self):
        return self.sps_tx

    def set_sps_tx(self, sps_tx):
        self.sps_tx = sps_tx

    def get_sps_rx(self):
        return self.sps_rx

    def set_sps_rx(self, sps_rx):
        self.sps_rx = sps_rx

    def get_samp_rate_tx(self):
        return self.samp_rate_tx

    def set_samp_rate_tx(self, samp_rate_tx):
        self.samp_rate_tx = samp_rate_tx
        self.uhd_usrp_sink_0.set_samp_rate(self.samp_rate_tx)

    def get_freq_tx(self):
        return self.freq_tx

    def set_freq_tx(self, freq_tx):
        self.freq_tx = freq_tx
        self.uhd_usrp_sink_0.set_center_freq(self.freq_tx, 0)

    def get_data_rate(self):
        return self.data_rate

    def set_data_rate(self, data_rate):
        self.data_rate = data_rate


def main(top_block_cls=uhd_bent_pipe, options=None):

    tb = top_block_cls()
    tb.start()
    try:
        raw_input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
