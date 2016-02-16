# gr-nuts

This is a GNU Radio module for the NUTS Student satellite project.

It features encoder and decoder blocks for the ![NGHAM](https://github.com/skagmo/ngham) protocol.

See the `examples` folder for sample flowgraphs

## Build instructions

    mkdir build
    cd build
    cmake ..
    make
    make install

**NOTE:** If your project was built using pyBOMBS, you need to specify you install path

    cmake .. -DCMAKE_INSTALL_PREFIX=~/target
