INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_NUTS nuts)

FIND_PATH(
    NUTS_INCLUDE_DIRS
    NAMES nuts/api.h
    HINTS $ENV{NUTS_DIR}/include
        ${PC_NUTS_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    NUTS_LIBRARIES
    NAMES gnuradio-nuts
    HINTS $ENV{NUTS_DIR}/lib
        ${PC_NUTS_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NUTS DEFAULT_MSG NUTS_LIBRARIES NUTS_INCLUDE_DIRS)
MARK_AS_ADVANCED(NUTS_LIBRARIES NUTS_INCLUDE_DIRS)

