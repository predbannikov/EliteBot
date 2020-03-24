#include "servicemenu.h"

ServiceMenu::ServiceMenu(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "SERVICEMENU";
}

void ServiceMenu::init(QStringList &asListParam)
{

}

bool ServiceMenu::logic(QStringList &asListParam)
{

    QPoint point = capture->getPoint("rectServiceFuel");
    mouse_move_click(point.x(), point.y());
    QThread::msleep(1000);
    QPoint point2 = capture->getPoint("rectServiceExit");
    mouse_move_click(point2.x(), point2.y());
    QThread::msleep(3000);


    return true;
}

void ServiceMenu::reset()
{

}
