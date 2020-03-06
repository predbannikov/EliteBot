#include "displays.h"

Displays::Displays(int x, int y, int width, int height) :
    m_x(x),
    m_y(y),
    m_width(width),
    m_height(height)
{
    if(m_height <= 0 || m_width <= 0) {
        QSize _size = qApp->screens()[0]->size();
        size = cv::Size(_size.width(), _size.height());
//        size = m_screen.availableSize();
        m_width = size.width;
        m_height = size.height;
    }
}

cv::Mat Displays::imageFromDisplay(int _format)
{
//    Display* display = XOpenDisplay(nullptr);
//    Window root = DefaultRootWindow(display);
//    XImage* img = XGetImage(display, root, X, Y , static_cast<unsigned int>(Width), static_cast<unsigned int>(Height), AllPlanes, ZPixmap);
//    Pixels.resize(static_cast<size_t>(Width) * static_cast<size_t>(Height) * 4);
//    memcpy(&Pixels[0], img->data, Pixels.size());
//    XDestroyImage(img);
//    XCloseDisplay(display);

//    cv::Size size(Height, Width);
//    if(_format == CV_8UC3) {
//        cv::Mat res = cv::Mat(size, _format);
//        cv::cvtColor(cv::Mat(Height, Width, CV_8UC4, &Pixels[0]), res, CV_RGBA2RGB);
//        return res;
//    } else if(_format == CV_8UC4) {
//        return cv::Mat(Height, Width, CV_8UC4, &Pixels[0]);
//    } else {
//        return cv::Mat(Height, Width, CV_8UC4, &Pixels[0]);         // UB
//    }


//    QList<QPixmap> scrs;
//    int w = 0, h = 0;
    auto screens = QGuiApplication::primaryScreen();
    QPixmap pix = screens->grabWindow(0, m_x, m_y, m_width, m_height);
//    foreach (auto scr, screens) {
//        QPixmap pix = scr->grabWindow(, Y, 210, 1240, 700);
//        w += pix.width();
//        if (h < pix.height()) h = pix.height();
//        scrs << pix;
//    }
    QImage tmpImage = pix.toImage();
//    cv::Mat mat = cv::Mat(tmpImage.height(), tmpImage.width(), CV_8UC4, reinterpret_cast<char*>(tmpImage.bits()));

    if(_format == CV_8UC3) {
        cv::Mat res = cv::Mat(size, _format);
        cv::cvtColor(cv::Mat(m_height, m_width, CV_8UC4, reinterpret_cast<char*>(tmpImage.bits())), res, CV_RGBA2RGB);
        return res;
    } else if(_format == CV_8UC4) {
        return cv::Mat(m_height, m_width, CV_8UC4, reinterpret_cast<char*>(tmpImage.bits()));
    } else {
        return cv::Mat(m_height, m_width, CV_8UC4, reinterpret_cast<char*>(tmpImage.bits()));         // UB
    }

//    IplImage *srcFrame = cvCreateImage(cvSize(tmpImage.width(), tmpImage.height()), IPL_DEPTH_8U, 4);
//    return mat;



}

cv::Size Displays::getSizeDesktops()
{
//    Display* display = XOpenDisplay(nullptr);
//    Window root = DefaultRootWindow(display);
//    XWindowAttributes attributes = {};
//    XGetWindowAttributes(display, root, &attributes);
//    cv::Size size;
//    size.width = attributes.width;
//    size.height = attributes.height;
//    XCloseDisplay(display);
//    return size;
    return cv::Size();
}
