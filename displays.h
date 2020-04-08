#ifndef DISPLAYS_H
#define DISPLAYS_H
#include <QPixmap>
#include <QThread>

#include <vector>
#include <iostream>
#include "global.h"
#if defined (Q_OS_WIN)
#include <QScreen>

#elif defined (Q_OS_LINUX)
#include <QScreen>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
#endif



class Displays
{
public:
    Displays(int x = 0, int y = 0, int width = 800, int height = 600);
    cv::Mat imageFromDisplay(int _format = CV_8UC3);
    cv::Size getSizeDesktops();


private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    cv::Rect rect;
    cv::Size size;
    std::vector<uint8_t> Pixels;
};



#endif // DISPLAYS_H
