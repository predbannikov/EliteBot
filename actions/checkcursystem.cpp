#include "checkcursystem.h"


CheckCurSystem::CheckCurSystem(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "CHECKCURSYSTEM";

}

void CheckCurSystem::init(QStringList &asListParam)
{
    sys_debugLog = false;
    trans = TRANS_1;
}

bool CheckCurSystem::logic(QStringList &asListParam)
{
    QPoint _point;
    cv::Point _cvPoint;
    QString sText;
    switch (trans) {
    case TRANS_1:
        _point = capture->getPoint("searchTrack_push");
        qDebug() << "searchTrack_push" << _point;
        mouse_move_click(_point.x(), _point.y());
        trans = TRANS_2;
        break;
    case TRANS_2:
        sText = capture->getTextApproximBoundingRect(capture->getRect("rect_curSystemMenuM"), _cvPoint, "eng");
//        sText = capture->getTextStaticField("rect_curSystemMenuM", cv::Scalar(100, 0, 0), cv::Scalar(255, 255, 255), "eng");
        sName = sText;
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
