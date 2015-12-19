# gr-nuts

This is a GNU Radio module for the NUTS Student satellite project.

## Build instructions

    mkdir build
    cd build
    cmake ..
    make

To install in grc

    make install

**NOTE** If your project was built using pyBOMBS, you need to specify you install path

    cmake .. -DCMAKE_INSTALL_PREFIX=~/target
