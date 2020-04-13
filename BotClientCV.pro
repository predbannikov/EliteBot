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

RC_FILE     = resource.rc



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
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_videoio$${VERSION_BUILD}d.lib"
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
            LIBS += "F:\projects\opencv\opencv\opencv-build\install\lib\opencv_videoio$${VERSION_BUILD}.lib"
    }

}



SOURCES += \
    actions/actionaimp.cpp \
    actions/actiondebug.cpp \
    actions/actiondeliverypaper.cpp \
    actions/actiongetcloser.cpp \
    actions/actionwait.cpp \
    actions/aimpflyaround.cpp \
    actions/checkcursystem.cpp \
    actions/dockingmenucase.cpp \
    actions/getstrstaticfield.cpp \
    actions/gettextstaticfield.cpp \
    actions/imageexpected.cpp \
    actions/imageexpectedclose.cpp \
    actions/mapsystem.cpp \
    actions/mapsystemenable.cpp \
    actions/marker.cpp \
    actions/panel1casehead.cpp \
    actions/panel1casemenucont.cpp \
    actions/panel1casemenunav.cpp \
    actions/panel1enable.cpp \
    actions/panel1subnav.cpp \
    actions/pickupspeed.cpp \
    actions/restorgame.cpp \
    actions/sendeventcontrol.cpp \
    actions/servicemenu.cpp \
    actions/stopafterautostart.cpp \
    actions/waitmenudocking.cpp \
    aicontrol.cpp \
    baseaction.cpp \
    capturewindow.cpp \
    controlpanel.cpp \
    displays.cpp \
    enginescript.cpp \
    guiinfo.cpp \
    iodata.cpp \
    lowlvlenginescript.cpp \
    main.cpp \
    socketio.cpp

HEADERS += \
    actions/actionaimp.h \
    actions/actiondebug.h \
    actions/actiondeliverypaper.h \
    actions/actiongetcloser.h \
    actions/actionwait.h \
    actions/aimpflyaround.h \
    actions/checkcursystem.h \
    actions/dockingmenucase.h \
    actions/getstrstaticfield.h \
    actions/gettextstaticfield.h \
    actions/imageexpected.h \
    actions/imageexpectedclose.h \
    actions/mapsystem.h \
    actions/mapsystemenable.h \
    actions/marker.h \
    actions/panel1Enable.h \
    actions/panel1casehead.h \
    actions/panel1casemenucont.h \
    actions/panel1casemenunav.h \
    actions/panel1subnav.h \
    actions/pickupspeed.h \
    actions/restorgame.h \
    actions/sendeventcontrol.h \
    actions/servicemenu.h \
    actions/stopafterautostart.h \
    actions/waitmenudocking.h \
    aicontrol.h \
    baseaction.h \
    capturewindow.h \
    controlpanel.h \
    displays.h \
    enginescript.h \
    global.h \
    guiinfo.h \
    iodata.h \
    lowlvlenginescript.h \
    main.h \
    resource.rc \
    socketio.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    controlpanel.ui
