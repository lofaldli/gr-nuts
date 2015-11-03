/* -*- c++ -*- */

#define NUTS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "nuts_swig_doc.i"

%{
#include "nuts/ngham_encoder.h"
#include "nuts/ngham_decoder.h"
%}

%include "nuts/ngham_encoder.h"
GR_SWIG_BLOCK_MAGIC2(nuts, ngham_encoder);

%include "nuts/ngham_decoder.h"
GR_SWIG_BLOCK_MAGIC2(nuts, ngham_decoder);
