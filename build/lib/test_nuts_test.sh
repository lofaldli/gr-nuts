#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/nuts/gr-nuts/lib
export GR_CONF_CONTROLPORT_ON=False
export PATH=/home/nuts/gr-nuts/build/lib:$PATH
export LD_LIBRARY_PATH=/home/nuts/gr-nuts/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-nuts 
