#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Uhd Rx
# Generated: Sat Jan 23 18:53:22 2016
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
from gnuradio import qtgui
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import nuts
import sip
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
        self.tuner = tuner = 0
        self.freq = freq = int(145.98e6)
        self.xlat_decim = xlat_decim = 8
        self.xlat_bandwidth = xlat_bandwidth = 10000
        self.sps = sps = 10
        self.samp_rate = samp_rate = 1000000
        self.gain = gain = 10
        self.freq_label = freq_label = freq+tuner
        self.data_rate = data_rate = 9600

        ##################################################
        # Blocks
        ##################################################
        self._tuner_range = Range(-20000, 20000, 1000, 0, 200)
        self._tuner_win = RangeWidget(self._tuner_range, self.set_tuner, "tuner", "counter_slider", float)
        self.top_layout.addWidget(self._tuner_win)
        self.qtgui = Qt.QTabWidget()
        self.qtgui_widget_0 = Qt.QWidget()
        self.qtgui_layout_0 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.qtgui_widget_0)
        self.qtgui_grid_layout_0 = Qt.QGridLayout()
        self.qtgui_layout_0.addLayout(self.qtgui_grid_layout_0)
        self.qtgui.addTab(self.qtgui_widget_0, "demodulated")
        self.qtgui_widget_1 = Qt.QWidget()
        self.qtgui_layout_1 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.qtgui_widget_1)
        self.qtgui_grid_layout_1 = Qt.QGridLayout()
        self.qtgui_layout_1.addLayout(self.qtgui_grid_layout_1)
        self.qtgui.addTab(self.qtgui_widget_1, "modulated")
        self.qtgui_widget_2 = Qt.QWidget()
        self.qtgui_layout_2 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.qtgui_widget_2)
        self.qtgui_grid_layout_2 = Qt.QGridLayout()
        self.qtgui_layout_2.addLayout(self.qtgui_grid_layout_2)
        self.qtgui.addTab(self.qtgui_widget_2, "fft")
        self.qtgui_widget_3 = Qt.QWidget()
        self.qtgui_layout_3 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.qtgui_widget_3)
        self.qtgui_grid_layout_3 = Qt.QGridLayout()
        self.qtgui_layout_3.addLayout(self.qtgui_grid_layout_3)
        self.qtgui.addTab(self.qtgui_widget_3, "waterfall")
        self.top_grid_layout.addWidget(self.qtgui, 0,0,1,1)
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
        self.qtgui_waterfall_sink_x_0 = qtgui.waterfall_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	freq+tuner, #fc
        	samp_rate, #bw
        	"", #name
                1 #number of inputs
        )
        self.qtgui_waterfall_sink_x_0.set_update_time(0.10)
        self.qtgui_waterfall_sink_x_0.enable_grid(False)
        
        if not True:
          self.qtgui_waterfall_sink_x_0.disable_legend()
        
        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_waterfall_sink_x_0.set_plot_pos_half(not True)
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        colors = [0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_waterfall_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_waterfall_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_waterfall_sink_x_0.set_color_map(i, colors[i])
            self.qtgui_waterfall_sink_x_0.set_line_alpha(i, alphas[i])
        
        self.qtgui_waterfall_sink_x_0.set_intensity_range(-140, 10)
        
        self._qtgui_waterfall_sink_x_0_win = sip.wrapinstance(self.qtgui_waterfall_sink_x_0.pyqwidget(), Qt.QWidget)
        self.qtgui_layout_3.addWidget(self._qtgui_waterfall_sink_x_0_win)
        self.qtgui_time_sink_x_1_0 = qtgui.time_sink_f(
        	1000, #size
        	data_rate, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_1_0.set_update_time(0.10)
        self.qtgui_time_sink_x_1_0.set_y_axis(-1, 2)
        
        self.qtgui_time_sink_x_1_0.set_y_label("", "")
        
        self.qtgui_time_sink_x_1_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_1_0.set_trigger_mode(qtgui.TRIG_MODE_TAG, qtgui.TRIG_SLOPE_POS, 0.5, 0.005, 0, "ngham_preamble")
        self.qtgui_time_sink_x_1_0.enable_autoscale(False)
        self.qtgui_time_sink_x_1_0.enable_grid(False)
        self.qtgui_time_sink_x_1_0.enable_control_panel(True)
        
        if not True:
          self.qtgui_time_sink_x_1_0.disable_legend()
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_1_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_1_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_1_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_1_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_1_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_1_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_1_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_time_sink_x_1_0_win = sip.wrapinstance(self.qtgui_time_sink_x_1_0.pyqwidget(), Qt.QWidget)
        self.qtgui_layout_0.addWidget(self._qtgui_time_sink_x_1_0_win)
        self.qtgui_time_sink_x_1 = qtgui.time_sink_c(
        	2048, #size
        	data_rate*sps, #samp_rate
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_1.set_update_time(0.10)
        self.qtgui_time_sink_x_1.set_y_axis(-2, 2)
        
        self.qtgui_time_sink_x_1.set_y_label("Amplitude", "")
        
        self.qtgui_time_sink_x_1.enable_tags(-1, True)
        self.qtgui_time_sink_x_1.set_trigger_mode(qtgui.TRIG_MODE_AUTO, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_1.enable_autoscale(False)
        self.qtgui_time_sink_x_1.enable_grid(False)
        self.qtgui_time_sink_x_1.enable_control_panel(False)
        
        if not True:
          self.qtgui_time_sink_x_1.disable_legend()
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "blue"]
        styles = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        
        for i in xrange(2*1):
            if len(labels[i]) == 0:
                if(i % 2 == 0):
                    self.qtgui_time_sink_x_1.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_1.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_1.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_1.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_1.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_1.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_1.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_1.set_line_alpha(i, alphas[i])
        
        self._qtgui_time_sink_x_1_win = sip.wrapinstance(self.qtgui_time_sink_x_1.pyqwidget(), Qt.QWidget)
        self.qtgui_layout_1.addWidget(self._qtgui_time_sink_x_1_win)
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	freq+tuner, #fc
        	samp_rate/xlat_decim, #bw
        	"", #name
        	2 #number of inputs
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-200, 0)
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(False)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)
        
        if not True:
          self.qtgui_freq_sink_x_0.disable_legend()
        
        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_freq_sink_x_0.set_plot_pos_half(not True)
        
        labels = ["filtered", "raw", "", "", "",
                  "", "", "", "", ""]
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 0.25, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(2):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.qtgui_layout_2.addWidget(self._qtgui_freq_sink_x_0_win)
        self.nuts_ngham_decoder_0 = nuts.ngham_decoder("packet_len", 0, True, True, False, False)
        self._gain_range = Range(0, 50, 1, 10, 200)
        self._gain_win = RangeWidget(self._gain_range, self.set_gain, "gain", "counter_slider", float)
        self.top_layout.addWidget(self._gain_win)
        self.freq_xlating_fir_filter_xxx_0_0 = filter.freq_xlating_fir_filter_ccc(8, (firdes.low_pass(1, samp_rate, samp_rate/2, 1000)), tuner, samp_rate)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(xlat_decim, (firdes.low_pass(1, samp_rate, xlat_bandwidth/2, 1000)), tuner, samp_rate)
        self._freq_label_tool_bar = Qt.QToolBar(self)
        
        if None:
          self._freq_label_formatter = None
        else:
          self._freq_label_formatter = lambda x: x
        
        self._freq_label_tool_bar.addWidget(Qt.QLabel("Frequency"+": "))
        self._freq_label_label = Qt.QLabel(str(self._freq_label_formatter(self.freq_label)))
        self._freq_label_tool_bar.addWidget(self._freq_label_label)
        self.top_layout.addWidget(self._freq_label_tool_bar)
          
        self.digital_gmsk_demod_0 = digital.gmsk_demod(
        	samples_per_symbol=sps,
        	gain_mu=0.175,
        	mu=0.5,
        	omega_relative_limit=0.005,
        	freq_error=0.0,
        	verbose=False,
        	log=False,
        )
        self.digital_correlate_access_code_tag_bb_1 = digital.correlate_access_code_tag_bb("1011101111001100010101001111110", 0, "ngham_sync")
        self.digital_correlate_access_code_tag_bb_0 = digital.correlate_access_code_tag_bb("10101010101010101010101010101010", 0, "ngham_preamble")
        self.blocks_socket_pdu_0 = blocks.socket_pdu("UDP_CLIENT", "127.0.0.1", "5006", 10000, False)
        self.blocks_char_to_float_0 = blocks.char_to_float(1, 1)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.nuts_ngham_decoder_0, 'out'), (self.blocks_socket_pdu_0, 'pdus'))    
        self.connect((self.blocks_char_to_float_0, 0), (self.qtgui_time_sink_x_1_0, 0))    
        self.connect((self.digital_correlate_access_code_tag_bb_0, 0), (self.digital_correlate_access_code_tag_bb_1, 0))    
        self.connect((self.digital_correlate_access_code_tag_bb_1, 0), (self.blocks_char_to_float_0, 0))    
        self.connect((self.digital_gmsk_demod_0, 0), (self.digital_correlate_access_code_tag_bb_0, 0))    
        self.connect((self.digital_gmsk_demod_0, 0), (self.nuts_ngham_decoder_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.qtgui_freq_sink_x_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0_0, 0), (self.qtgui_freq_sink_x_0, 1))    
        self.connect((self.freq_xlating_fir_filter_xxx_0_0, 0), (self.qtgui_waterfall_sink_x_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.digital_gmsk_demod_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.qtgui_time_sink_x_1, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.freq_xlating_fir_filter_xxx_0_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "uhd_rx")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_tuner(self):
        return self.tuner

    def set_tuner(self, tuner):
        self.tuner = tuner
        self.freq_xlating_fir_filter_xxx_0_0.set_center_freq(self.tuner)
        self.qtgui_freq_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate/self.xlat_decim)
        self.qtgui_waterfall_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate)
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.tuner)
        self.set_freq_label(self._freq_label_formatter(self.freq+self.tuner))

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.qtgui_freq_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate/self.xlat_decim)
        self.qtgui_waterfall_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate)
        self.uhd_usrp_source_0.set_center_freq(self.freq, 0)
        self.set_freq_label(self._freq_label_formatter(self.freq+self.tuner))

    def get_xlat_decim(self):
        return self.xlat_decim

    def set_xlat_decim(self, xlat_decim):
        self.xlat_decim = xlat_decim
        self.qtgui_freq_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate/self.xlat_decim)

    def get_xlat_bandwidth(self):
        return self.xlat_bandwidth

    def set_xlat_bandwidth(self, xlat_bandwidth):
        self.xlat_bandwidth = xlat_bandwidth
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.samp_rate, self.xlat_bandwidth/2, 1000)))

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.qtgui_time_sink_x_1.set_samp_rate(self.data_rate*self.sps)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.freq_xlating_fir_filter_xxx_0_0.set_taps((firdes.low_pass(1, self.samp_rate, self.samp_rate/2, 1000)))
        self.qtgui_freq_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate/self.xlat_decim)
        self.qtgui_waterfall_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate)
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.samp_rate, self.xlat_bandwidth/2, 1000)))

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain

    def get_freq_label(self):
        return self.freq_label

    def set_freq_label(self, freq_label):
        self.freq_label = freq_label
        Qt.QMetaObject.invokeMethod(self._freq_label_label, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.freq_label)))

    def get_data_rate(self):
        return self.data_rate

    def set_data_rate(self, data_rate):
        self.data_rate = data_rate
        self.qtgui_time_sink_x_1_0.set_samp_rate(self.data_rate)
        self.qtgui_time_sink_x_1.set_samp_rate(self.data_rate*self.sps)


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
