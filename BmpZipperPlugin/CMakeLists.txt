qt_policy(SET QTP0001 NEW)

set(SRC_QML_FILES
    qml/FileListModelDelegate.qml
    qml/CustomDialogManager.qml
    qml/CustomDialog.qml
    qml/CustomButton.qml
    qml/CustomProgressBar.qml
)

set(SRC_QML_SINGLETONS
    qml/ColorMap.qml
)

set(SRC_FILES
    filelistmodel.cpp
    filelistmodel.h
    compressionmodel.cpp
    compressionmodel.h
    progressmodel.cpp
    progressmodel.h
)

set_source_files_properties(${SRC_QML_SINGLETONS} PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(bmp-zipper-plugin
    URI "BmpZipperPlugin"
    PLUGIN_TARGET bmp-zipper-plugin
    DEPENDENCIES QtQuick
    QML_FILES ${SRC_QML_FILES} ${SRC_QML_SINGLETONS}
)

target_sources(bmp-zipper-plugin PRIVATE
    ${SRC_FILES}
)

target_link_libraries(bmp-zipper-plugin PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    bmp-lib
)

install(TARGETS bmp-zipper-plugin
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/BmpZipperPlugin"
    LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}/BmpZipperPlugin"
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qmldir
    DESTINATION "${CMAKE_INSTALL_BINDIR}/BmpZipperPlugin"
)
