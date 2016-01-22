#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Uhd Rx
# Generated: Fri Jan 22 15:17:41 2016
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import nuts
import sys
import time


class uhd_rx(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Uhd Rx")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Uhd Rx")
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "uhd_rx")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.xlat_decim = xlat_decim = 8
        self.xlat_bandwidth = xlat_bandwidth = 10000
        self.tuner = tuner = 0
        self.sps = sps = 10
        self.samp_rate = samp_rate = 1000000
        self.gain = gain = 10
        self.freq = freq = int(145.98e6)
        self.data_rate = data_rate = 9600

        ##################################################
        # Blocks
        ##################################################
        self._tuner_range = Range(-20000, 20000, 1000, 0, 200)
        self._tuner_win = RangeWidget(self._tuner_range, self.set_tuner, "tuner", "counter_slider", float)
        self.top_layout.addWidget(self._tuner_win)
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(samp_rate)
        self.uhd_usrp_source_0.set_center_freq(freq, 0)
        self.uhd_usrp_source_0.set_gain(30, 0)
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=data_rate*sps,
                decimation=samp_rate/xlat_decim,
                taps=None,
                fractional_bw=None,
        )
        self.nuts_ngham_decoder_0 = nuts.ngham_decoder("packet_len", 0, True, True, False, False)
        self._gain_range = Range(0, 50, 1, 10, 200)
        self._gain_win = RangeWidget(self._gain_range, self.set_gain, "gain", "counter_slider", float)
        self.top_layout.addWidget(self._gain_win)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(xlat_decim, (firdes.low_pass(1, samp_rate, xlat_bandwidth/2, 1000)), tuner, samp_rate)
        self.digital_gmsk_demod_0 = digital.gmsk_demod(
        	samples_per_symbol=sps,
        	gain_mu=0.175,
        	mu=0.5,
        	omega_relative_limit=0.005,
        	freq_error=0.0,
        	verbose=False,
        	log=False,
        )
        self.blocks_socket_pdu_0 = blocks.socket_pdu("UDP_CLIENT", "127.0.0.1", "5006", 10000, False)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.nuts_ngham_decoder_0, 'out'), (self.blocks_socket_pdu_0, 'pdus'))    
        self.connect((self.digital_gmsk_demod_0, 0), (self.nuts_ngham_decoder_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.digital_gmsk_demod_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "uhd_rx")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_xlat_decim(self):
        return self.xlat_decim

    def set_xlat_decim(self, xlat_decim):
        self.xlat_decim = xlat_decim

    def get_xlat_bandwidth(self):
        return self.xlat_bandwidth

    def set_xlat_bandwidth(self, xlat_bandwidth):
        self.xlat_bandwidth = xlat_bandwidth
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.samp_rate, self.xlat_bandwidth/2, 1000)))

    def get_tuner(self):
        return self.tuner

    def set_tuner(self, tuner):
        self.tuner = tuner
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.tuner)

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.samp_rate, self.xlat_bandwidth/2, 1000)))
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.uhd_usrp_source_0.set_center_freq(self.freq, 0)

    def get_data_rate(self):
        return self.data_rate

    def set_data_rate(self, data_rate):
        self.data_rate = data_rate


def main(top_block_cls=uhd_rx, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
