# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND nuts_sources
    fec-3.0.1/init_rs_char.c
    fec-3.0.1/encode_rs_char.c 
    fec-3.0.1/decode_rs_char.c
    #ccsds_scrambler.c
    #crc_ccitt.c
    reed_solomon.cc
    ngham_encoder_impl.cc
    ngham_correlator_impl.cc
    ngham_decoder_impl.cc
    )

set(nuts_sources "${nuts_sources}" PARENT_SCOPE)
if(NOT nuts_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT nuts_sources)

add_library(gnuradio-nuts SHARED ${nuts_sources})
target_link_libraries(gnuradio-nuts ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-nuts PROPERTIES DEFINE_SYMBOL "gnuradio_nuts_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-nuts PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-nuts
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_nuts_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_nuts.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_nuts.cc
)

add_executable(test-nuts ${test_nuts_sources})

target_link_libraries(
  test-nuts
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-nuts
)

GR_ADD_TEST(test_nuts test-nuts)
