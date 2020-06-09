#include "checkcursystem.h"


CheckCurSystem::CheckCurSystem(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "CHECKCURSYSTEM";

}

void CheckCurSystem::mouseClick(QPoint aPoint)
{
    int relat = 5;
    int x = aPoint.x();
    int y = aPoint.y();
    ulong wait = 5;
    mouse_move(x, y);
    QThread::msleep(wait);
    mouse_move_rel(relat, 0);
    QThread::msleep(wait);
    mouse_move_rel(0, relat);
    QThread::msleep(wait);
    mouse_move_rel(-relat, 0);
    QThread::msleep(wait);
    mouse_move_rel(0, -relat);
    QThread::msleep(wait);
    mouse_move_click(x, y);
    QThread::msleep(waitMSec);

}

void CheckCurSystem::init(QStringList &asListParam)
{
    sys_debugLog = false;
    trans = TRANS_1;
}

bool CheckCurSystem::logic(QStringList &asListParam)
{
    qDebug() << "Должна быть система" <<  asListParam[2];
    QPoint _point;
    cv::Point _cvPoint;
    QString sText;
    switch (trans) {
    case TRANS_1:
        QThread::msleep(1000);
        _point = capture->getPoint("searchTrack_push");
        qDebug() << "searchTrack_push" << _point;
        mouseClick(_point);
        QThread::msleep(2000);
        trans = TRANS_2;
        break;
    case TRANS_2:
//        sText = capture->getTextApproximBoundingRect(capture->getRect("rect_curSystemMenuM"), _cvPoint, "eng");
        if(capture->containTextApproximBoundingRect(capture->getRect("rect_curSystemMenuM"), _cvPoint, asListParam[2], "eng", 2)) {
            sName = "found";
        } else {
            sName = "not found";
        }
//        sText = capture->getTextStaticField("rect_curSystemMenuM", cv::Scalar(100, 0, 0), cv::Scalar(255, 255, 255), "eng");
//        sName = sText;
        qDebug() << "Получили строку" << sName;
        asListParam[2] = sName;
        asListParam[1] = "2";
        trans = TRANS_1;
        return true;
    }
    return false;
}

void CheckCurSystem::printDebug()
{

}

void CheckCurSystem::reset()
{
}
