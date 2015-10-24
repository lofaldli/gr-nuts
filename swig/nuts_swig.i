/* -*- c++ -*- */

#define NUTS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "nuts_swig_doc.i"

%{
#include "nuts/ngham_framer.h"
#include "nuts/ngham_codeword_generator.h"
%}


%include "nuts/ngham_framer.h"
GR_SWIG_BLOCK_MAGIC2(nuts, ngham_framer);

%include "nuts/ngham_codeword_generator.h"
GR_SWIG_BLOCK_MAGIC2(nuts, ngham_codeword_generator);
