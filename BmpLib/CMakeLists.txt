cmake_minimum_required(VERSION 3.16)

set(SRC
    bmpexceptions.cpp
    bmpproxy.cpp
    dynamicbitset.cpp
    filehandler.cpp
    bmpdefs.h
    bmpexceptions.h
    bmpproxy.h
    bmputils.h
    dynamicbitset.h
    filehandler.h
)

add_library(bmp-lib SHARED ${SRC})

target_include_directories(bmp-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS bmp-lib
    LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}/BmpLib"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/BmpLib"
)