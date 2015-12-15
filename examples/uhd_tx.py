#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Uhd Tx
# Generated: Mon Dec 14 13:24:55 2015
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
import pmt
import sys
import time


class uhd_tx(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Uhd Tx")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Uhd Tx")
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

        self.settings = Qt.QSettings("GNU Radio", "uhd_tx")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.message = message = "LA1NGS this is an NGHAM test message"
        self.data = data = [ord(x) for x in message]
        self.tuner = tuner = 0
        self.sps = sps = 10
        self.samp_rate = samp_rate = 2000000
        self.ngham_rate = ngham_rate = 9600
        self.length_tag = length_tag = gr.tag_utils.python_to_tag((0, pmt.intern("packet_len"), pmt.from_long(len(data)), pmt.intern("src")))
        self.gain = gain = 10
        self.freq = freq = int(145.98e6)

        ##################################################
        # Blocks
        ##################################################
        self._tuner_range = Range(-20000, 20000, 1000, 0, 200)
        self._tuner_win = RangeWidget(self._tuner_range, self.set_tuner, "tuner", "counter_slider", float)
        self.top_layout.addWidget(self._tuner_win)
        self._gain_range = Range(0, 50, 1, 10, 200)
        self._gain_win = RangeWidget(self._gain_range, self.set_gain, "gain", "counter_slider", float)
        self.top_layout.addWidget(self._gain_win)
        self.uhd_usrp_sink_0_0 = uhd.usrp_sink(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_sink_0_0.set_samp_rate(samp_rate)
        self.uhd_usrp_sink_0_0.set_center_freq(freq+tuner, 0)
        self.uhd_usrp_sink_0_0.set_gain(gain, 0)
        self.rational_resampler_xxx_1 = filter.rational_resampler_ccc(
                interpolation=samp_rate,
                decimation=sps*ngham_rate,
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

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "uhd_tx")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


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

    def get_tuner(self):
        return self.tuner

    def set_tuner(self, tuner):
        self.tuner = tuner
        self.uhd_usrp_sink_0_0.set_center_freq(self.freq+self.tuner, 0)

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_sink_0_0.set_samp_rate(self.samp_rate)

    def get_ngham_rate(self):
        return self.ngham_rate

    def set_ngham_rate(self, ngham_rate):
        self.ngham_rate = ngham_rate

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
        self.uhd_usrp_sink_0_0.set_center_freq(self.freq+self.tuner, 0)


def main(top_block_cls=uhd_tx, options=None):

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
