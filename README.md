# gr-nuts

This is a GNU Radio module for the NUTS Student satellite project.

It features encoder and decoder blocks for the [NGHAM](https://github.com/skagmo/ngham) protocol.

See the `examples` folder for sample flowgraphs

## Build instructions

    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/your/prefix
    make
    make install

