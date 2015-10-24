#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/nuts/gr-nuts/python
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/nuts/gr-nuts/build/python:$PATH
export LD_LIBRARY_PATH=/home/nuts/gr-nuts/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/nuts/gr-nuts/build/swig:$PYTHONPATH
/usr/bin/python2 /home/nuts/gr-nuts/python/qa_ngham_codeword_generator.py 
