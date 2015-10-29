#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Rtl Rx
# Generated: Thu Oct 29 18:52:09 2015
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

from baz import facsink
from gnuradio import analog
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.wxgui import forms
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import baz
import wx


class rtl_rx(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Rtl Rx")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.xlate_bandwidth = xlate_bandwidth = 7200
        self.variable_0 = variable_0 = 0
        self.tuner = tuner = -10000
        self.samp_rate = samp_rate = 2000000
        self.freq = freq = 145980000
        self.data_rate = data_rate = 1200

        ##################################################
        # Blocks
        ##################################################
        _tuner_sizer = wx.BoxSizer(wx.VERTICAL)
        self._tuner_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_tuner_sizer,
        	value=self.tuner,
        	callback=self.set_tuner,
        	label='tuner',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._tuner_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_tuner_sizer,
        	value=self.tuner,
        	callback=self.set_tuner,
        	minimum=-20000,
        	maximum=20000,
        	num_steps=20,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_tuner_sizer)
        self.rtl2832_source_0 = baz.rtl_source_c(defer_creation=True, output_size=gr.sizeof_gr_complex)
        self.rtl2832_source_0.set_verbose(True)
        self.rtl2832_source_0.set_vid(0x0)
        self.rtl2832_source_0.set_pid(0x0)
        self.rtl2832_source_0.set_tuner_name("")
        self.rtl2832_source_0.set_default_timeout(0)
        self.rtl2832_source_0.set_use_buffer(True)
        self.rtl2832_source_0.set_fir_coefficients(([]))
        
        self.rtl2832_source_0.set_read_length(0)
        
        
        
        
        if self.rtl2832_source_0.create() == False: raise Exception("Failed to create RTL2832 Source: rtl2832_source_0")
        
        
        self.rtl2832_source_0.set_sample_rate(samp_rate)
        
        self.rtl2832_source_0.set_frequency(freq)
        
        
        
        self.rtl2832_source_0.set_auto_gain_mode(False)
        self.rtl2832_source_0.set_relative_gain(True)
        self.rtl2832_source_0.set_gain(20)
          
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=96000,
                decimation=250000,
                taps=None,
                fractional_bw=None,
        )
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(8, (firdes.low_pass(1, samp_rate, xlate_bandwidth/2, 1000)), tuner, samp_rate)
        self.facsink_0 = facsink.fac_sink_c(
        	self.GetWin(),
        	title="Fast AutoCorrelation",
        	sample_rate=96000,
        	baseband_freq=0,
                y_per_div=20,
        	ref_level=20,
        	fac_size=100000,
                fac_rate=facsink.default_fac_rate,
                average=False,
        	avg_alpha=0,
        	peak_hold=False,
        )
        self.Add(self.facsink_0.win)
        self.analog_simple_squelch_cc_0 = analog.simple_squelch_cc(-70, 0.1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_simple_squelch_cc_0, 0), (self.facsink_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.analog_simple_squelch_cc_0, 0))    
        self.connect((self.rtl2832_source_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))    

    def get_xlate_bandwidth(self):
        return self.xlate_bandwidth

    def set_xlate_bandwidth(self, xlate_bandwidth):
        self.xlate_bandwidth = xlate_bandwidth
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.samp_rate, self.xlate_bandwidth/2, 1000)))

    def get_variable_0(self):
        return self.variable_0

    def set_variable_0(self, variable_0):
        self.variable_0 = variable_0

    def get_tuner(self):
        return self.tuner

    def set_tuner(self, tuner):
        self.tuner = tuner
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.tuner)
        self._tuner_slider.set_value(self.tuner)
        self._tuner_text_box.set_value(self.tuner)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.freq_xlating_fir_filter_xxx_0.set_taps((firdes.low_pass(1, self.samp_rate, self.xlate_bandwidth/2, 1000)))
        self.rtl2832_source_0.set_sample_rate(self.samp_rate)

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.rtl2832_source_0.set_frequency(self.freq)

    def get_data_rate(self):
        return self.data_rate

    def set_data_rate(self, data_rate):
        self.data_rate = data_rate


def main(top_block_cls=rtl_rx, options=None):

    tb = top_block_cls()
    tb.Start(True)
    tb.Wait()


if __name__ == '__main__':
    main()
