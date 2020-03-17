QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


CONFIG += console
CONFIG += c+14

#QMAKE_CXXFLAGS_DEBUG += Od
#QMAKE_CXXFLAGS_DEBUG += -Zi


unix: !macx {
    LIBS += -lX11   -lopencv_core
    QMAKE_CXXFLAGS_DEBUG += -O0
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
    -lopencv_imgproc
    -lopencv_highgui
    -lopencv_ml
    -lopencv_video
    -lopencv_features2d
    -lopencv_calib3d
    -lopencv_objdetect
    -lopencv_contrib
    -lopencv_legacy
    -lopencv_flann
}

VERSION_BUILD = 320

win32-msvc* {
    INCLUDEPATH += "F:\projects\opencv\opencv\opencv-build\install\include"
    INCLUDEPATH += "F:\projects\vcpkg\installed\x86-windows\include"

#    debug:LIBS += "F:\projects\vcpkg\installed\x86-windows\debug\lib\tesseract41d.lib"
#    else:LIBS += "F:\projects\vcpkg\installed\x86-windows\lib\tesseract41.lib"

    CONFIG(debug, debug|release) {
            LIBS += "F:\projects\vcpkg\installed\x86-windows\debug\lib\tesseract41d.lib"
            LIBS += "F:\projects\vcpkg\installed\x86-windows\debug\lib\leptonica-1.78.0d.lib"

            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_core$${VERSION_BUILD}d.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_highgui$${VERSION_BUILD}d.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_imgcodecs$${VERSION_BUILD}d.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_imgproc$${VERSION_BUILD}d.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_features2d$${VERSION_BUILD}d.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_calib3d$${VERSION_BUILD}d.lib"
    }
    CONFIG(release, debug|release) {
            LIBS += "F:\projects\vcpkg\installed\x86-windows\lib\tesseract41.lib"
            LIBS += "F:\projects\vcpkg\installed\x86-windows\lib\leptonica-1.78.0.lib"

            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_core$${VERSION_BUILD}.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_highgui$${VERSION_BUILD}.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_imgcodecs$${VERSION_BUILD}.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_imgproc$${VERSION_BUILD}.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_features2d$${VERSION_BUILD}.lib"
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_calib3d$${VERSION_BUILD}.lib"
    }

}



SOURCES += \
    aicontrol.cpp \
    capturewindow.cpp \
    displays.cpp \
    enginescript.cpp \
    guiinfo.cpp \
    iodata.cpp \
    main.cpp \
    parsingscript.cpp

HEADERS += \
    aicontrol.h \
    capturewindow.h \
    displays.h \
    enginescript.h \
    global.h \
    guiinfo.h \
    iodata.h \
    main.h \
    parsingscript.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
