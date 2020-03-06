#include <iostream>
#include "enginescript.h"



int main(int argc, char *argv[])
{

    //    cvSetMouseCallback("win2", my_mouse_callback, reinterpret_cast<void *> (image));

    QApplication a(argc, argv);
    EngineScript *engine = new EngineScript(&a);

//    cv::Mat mat = cv::imread("F:/projects/work/elite/autoClickerOnOpenCV/1.jpg", cv::IMREAD_COLOR);
//    cv::imwrite("2.jpg", mat);
//    cv::imshow("win2", mat);

//    cv::createButton("button1", callbackButton1, capture, CV_PUSH_BUTTON, false);

    QTimer::singleShot(1, [engine]() -> void {
        engine->update();
    });
    return a.exec();

}
