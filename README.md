# gr-nuts

This is a GNURadio module for the NUTS Student satellite project.

## Build instructions

    mkdir build
    cd build
    cmake ..
    make

To install in grc

    make install

To run tests

    make test

To run a specific test

    ctest -V -R <keyword>

### Note

If your project was built using pyBOMBS, you need to specify you install path

    cmake .. -DCMAKE_INSTALL_PREFIX=~/target
