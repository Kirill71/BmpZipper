cmake_minimum_required(VERSION 3.16)
project(BmpZipper LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

find_package(Qt6 REQUIRED COMPONENTS Qml Quick QuickControls2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(bmp-zipper
    main.cpp
)

set_target_properties(bmp-zipper PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(bmp-zipper PRIVATE
    Qt::Qml
    Qt::Quick
    Qt::QuickControls2
)

qt_add_qml_module(bmp-zipper
    URI BmpZipperApp
    QML_FILES App.qml
)

add_subdirectory(BmpZipperPlugin)
add_subdirectory(BmpLib)

install(TARGETS bmp-zipper
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET bmp-zipper
    OUTPUT_SCRIPT deploy_script
    MACOS_BUNDLE_POST_BUILD
    NO_UNSUPPORTED_PLATFORM_ERROR
    DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
)
install(SCRIPT ${deploy_script})


