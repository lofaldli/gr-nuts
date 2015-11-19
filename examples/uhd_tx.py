#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Uhd Tx
# Generated: Sun Nov 15 17:26:56 2015
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
from PyQt4.QtCore import QObject, pyqtSlot
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
import pmt
import sip
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
        self.tuner = tuner = 0
        self.gain = gain = 10
        self.freq = freq = 145.98e6
        self.data = data = [ord(x) for x in message]
        self.sps = sps = 10
        self.samp_rate = samp_rate = 2000000
        self.ngham_rate = ngham_rate = 9600
        self.length_tag = length_tag = gr.tag_utils.python_to_tag((0, pmt.intern("packet_len"), pmt.from_long(len(data)), pmt.intern("src")))
        self.gain_label = gain_label = gain
        self.freq_label = freq_label = freq+tuner

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
        self.qtgui.addTab(self.qtgui_widget_0, "encoded")
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
        self.top_grid_layout.addWidget(self.qtgui, 0,0,1,1)
        self._gain_range = Range(0, 50, 1, 10, 200)
        self._gain_win = RangeWidget(self._gain_range, self.set_gain, "gain", "counter_slider", float)
        self.top_layout.addWidget(self._gain_win)
        self._freq_options = (145.98e6, 437.305e6, )
        self._freq_labels = ("vhf", "uhf", )
        self._freq_group_box = Qt.QGroupBox("frequency select")
        self._freq_box = Qt.QHBoxLayout()
        class variable_chooser_button_group(Qt.QButtonGroup):
            def __init__(self, parent=None):
                Qt.QButtonGroup.__init__(self, parent)
            @pyqtSlot(int)
            def updateButtonChecked(self, button_id):
                self.button(button_id).setChecked(True)
        self._freq_button_group = variable_chooser_button_group()
        self._freq_group_box.setLayout(self._freq_box)
        for i, label in enumerate(self._freq_labels):
        	radio_button = Qt.QRadioButton(label)
        	self._freq_box.addWidget(radio_button)
        	self._freq_button_group.addButton(radio_button, i)
        self._freq_callback = lambda i: Qt.QMetaObject.invokeMethod(self._freq_button_group, "updateButtonChecked", Qt.Q_ARG("int", self._freq_options.index(i)))
        self._freq_callback(self.freq)
        self._freq_button_group.buttonClicked[int].connect(
        	lambda i: self.set_freq(self._freq_options[i]))
        self.top_layout.addWidget(self._freq_group_box)
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
        self.qtgui_time_sink_x_1_0_0 = qtgui.time_sink_f(
        	1024, #size
        	ngham_rate, #samp_rate
        	"encoded data", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_1_0_0.set_update_time(0.10)
        self.qtgui_time_sink_x_1_0_0.set_y_axis(-1, 2)
        
        self.qtgui_time_sink_x_1_0_0.set_y_label("", "")
        
        self.qtgui_time_sink_x_1_0_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_1_0_0.set_trigger_mode(qtgui.TRIG_MODE_TAG, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "packet_len")
        self.qtgui_time_sink_x_1_0_0.enable_autoscale(False)
        self.qtgui_time_sink_x_1_0_0.enable_grid(False)
        self.qtgui_time_sink_x_1_0_0.enable_control_panel(False)
        
        if not False:
          self.qtgui_time_sink_x_1_0_0.disable_legend()
        
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
                self.qtgui_time_sink_x_1_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_1_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_1_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_1_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_1_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_1_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_1_0_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_time_sink_x_1_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_1_0_0.pyqwidget(), Qt.QWidget)
        self.qtgui_layout_0.addWidget(self._qtgui_time_sink_x_1_0_0_win)
        self.qtgui_time_sink_x_0 = qtgui.time_sink_c(
        	10000, #size
        	sps*ngham_rate, #samp_rate
        	"modulated", #name
        	1 #number of inputs
        )
        self.qtgui_time_sink_x_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0.set_y_axis(-5, 5)
        
        self.qtgui_time_sink_x_0.set_y_label("", "")
        
        self.qtgui_time_sink_x_0.enable_tags(-1, True)
        self.qtgui_time_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_TAG, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "packet_len")
        self.qtgui_time_sink_x_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0.enable_grid(False)
        self.qtgui_time_sink_x_0.enable_control_panel(False)
        
        if not False:
          self.qtgui_time_sink_x_0.disable_legend()
        
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
                    self.qtgui_time_sink_x_0.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_0.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_time_sink_x_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0.pyqwidget(), Qt.QWidget)
        self.qtgui_layout_1.addWidget(self._qtgui_time_sink_x_0_win)
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	freq+tuner, #fc
        	samp_rate, #bw
        	"FFT", #name
        	1 #number of inputs
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(False)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)
        
        if not True:
          self.qtgui_freq_sink_x_0.disable_legend()
        
        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_freq_sink_x_0.set_plot_pos_half(not True)
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.qtgui_layout_2.addWidget(self._qtgui_freq_sink_x_0_win)
        self.nuts_ngham_encoder_0 = nuts.ngham_encoder("packet_len", True, True, True, False)
        self._gain_label_tool_bar = Qt.QToolBar(self)
        
        if None:
          self._gain_label_formatter = None
        else:
          self._gain_label_formatter = lambda x: x
        
        self._gain_label_tool_bar.addWidget(Qt.QLabel("Gain"+": "))
        self._gain_label_label = Qt.QLabel(str(self._gain_label_formatter(self.gain_label)))
        self._gain_label_tool_bar.addWidget(self._gain_label_label)
        self.top_layout.addWidget(self._gain_label_tool_bar)
          
        self._freq_label_tool_bar = Qt.QToolBar(self)
        
        if None:
          self._freq_label_formatter = None
        else:
          self._freq_label_formatter = lambda x: x
        
        self._freq_label_tool_bar.addWidget(Qt.QLabel("Frequency"+": "))
        self._freq_label_label = Qt.QLabel(str(self._freq_label_formatter(self.freq_label)))
        self._freq_label_tool_bar.addWidget(self._freq_label_label)
        self.top_layout.addWidget(self._freq_label_tool_bar)
          
        self.digital_gmsk_mod_0 = digital.gmsk_mod(
        	samples_per_symbol=sps,
        	bt=0.35,
        	verbose=False,
        	log=False,
        )
        self.blocks_vector_source_x_0_0 = blocks.vector_source_b(data, True, 1, [length_tag])
        self.blocks_unpack_k_bits_bb_0_0_0 = blocks.unpack_k_bits_bb(8)
        self.blocks_tagged_stream_align_0 = blocks.tagged_stream_align(gr.sizeof_char*1, "packet_len")
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_char_to_float_0_0_0 = blocks.char_to_float(1, 1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_char_to_float_0_0_0, 0), (self.qtgui_time_sink_x_1_0_0, 0))    
        self.connect((self.blocks_tagged_stream_align_0, 0), (self.digital_gmsk_mod_0, 0))    
        self.connect((self.blocks_unpack_k_bits_bb_0_0_0, 0), (self.blocks_char_to_float_0_0_0, 0))    
        self.connect((self.blocks_vector_source_x_0_0, 0), (self.nuts_ngham_encoder_0, 0))    
        self.connect((self.digital_gmsk_mod_0, 0), (self.qtgui_time_sink_x_0, 0))    
        self.connect((self.digital_gmsk_mod_0, 0), (self.rational_resampler_xxx_1, 0))    
        self.connect((self.nuts_ngham_encoder_0, 0), (self.blocks_tagged_stream_align_0, 0))    
        self.connect((self.nuts_ngham_encoder_0, 0), (self.blocks_unpack_k_bits_bb_0_0_0, 0))    
        self.connect((self.rational_resampler_xxx_1, 0), (self.blocks_null_sink_0, 0))    
        self.connect((self.rational_resampler_xxx_1, 0), (self.qtgui_freq_sink_x_0, 0))    
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

    def get_tuner(self):
        return self.tuner

    def set_tuner(self, tuner):
        self.tuner = tuner
        self.set_freq_label(self._freq_label_formatter(self.freq+self.tuner))
        self.qtgui_freq_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate)
        self.uhd_usrp_sink_0_0.set_center_freq(self.freq+self.tuner, 0)

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self.set_gain_label(self._gain_label_formatter(self.gain))
        self.uhd_usrp_sink_0_0.set_gain(self.gain, 0)
        	

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self._freq_callback(self.freq)
        self.set_freq_label(self._freq_label_formatter(self.freq+self.tuner))
        self.qtgui_freq_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate)
        self.uhd_usrp_sink_0_0.set_center_freq(self.freq+self.tuner, 0)

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
        self.qtgui_time_sink_x_0.set_samp_rate(self.sps*self.ngham_rate)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_freq_sink_x_0.set_frequency_range(self.freq+self.tuner, self.samp_rate)
        self.uhd_usrp_sink_0_0.set_samp_rate(self.samp_rate)

    def get_ngham_rate(self):
        return self.ngham_rate

    def set_ngham_rate(self, ngham_rate):
        self.ngham_rate = ngham_rate
        self.qtgui_time_sink_x_0.set_samp_rate(self.sps*self.ngham_rate)
        self.qtgui_time_sink_x_1_0_0.set_samp_rate(self.ngham_rate)

    def get_length_tag(self):
        return self.length_tag

    def set_length_tag(self, length_tag):
        self.length_tag = length_tag
        self.blocks_vector_source_x_0_0.set_data(self.data, [self.length_tag])

    def get_gain_label(self):
        return self.gain_label

    def set_gain_label(self, gain_label):
        self.gain_label = gain_label
        Qt.QMetaObject.invokeMethod(self._gain_label_label, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.gain_label)))

    def get_freq_label(self):
        return self.freq_label

    def set_freq_label(self, freq_label):
        self.freq_label = freq_label
        Qt.QMetaObject.invokeMethod(self._freq_label_label, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.freq_label)))


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
