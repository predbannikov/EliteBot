#include "mapsystem.h"


void MapSystem::mouseClick(QPoint point)
{
    int relat = 5;
    int x = point.x();
    int y = point.y();
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

MapSystem::MapSystem(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "MAPSYSTEM";
}

void MapSystem::init(QStringList &asListParam)
{
    waitMSec = 700;
    sys_debugLog = false;

    searchTrack_push_trigger = false;
    fieldSearch_trigger = false;
    push_search_trigger = false;
    sSearchImage = "pic_setToTarget";
    nCount = 9;
    iStart = 2;
    iEnd = 80;
    diffCoef = 0.93;

}

bool MapSystem::logic(QStringList &asListParam)
{
    QPoint _point;
    if(confirmTimer.elapsed() > 30000) {
        push_key("m");
        QThread::msleep(3000);
        asListParam[1] = "1";
        asListParam[2] = "no cursor set system";
        qDebug() << "TEST!!! -> setCursorSystem freeze  =" << asListParam;
        return true;
    }
    if(!searchTrack_push_trigger) {
        _point = capture->getPoint("searchTrack_push");
        qDebug() << "searchTrack_push" << _point;
        mouseClick(_point);
        searchTrack_push_trigger = true;
    } else {
        if(!fieldSearch_trigger) {
            _point = capture->getPoint("fieldSearch");
            qDebug() << "fieldSearch" << asListParam[2];
            // ФИкс баг чей не понятно, без которого не работают клики по кнопкам
            QThread::msleep(50);
            press_key("a");
            QThread::msleep(100);
            release_key("a");

            mouseClick(_point);
            QThread::msleep(500);
            typingText(asListParam[2]);

            QThread::msleep(waitMSec);
            fieldSearch_trigger = true;
        } else {
            if(!push_search_trigger) {
                _point = capture->getPoint("push_search");
                qDebug() << "push_search" << _point;
                mouseClick(_point);
                QThread::msleep(50);
                press_key("a");
                QThread::msleep(100);
                release_key("a");
                QThread::msleep(3000);
                push_search_trigger = true;
                timer.restart();
            } else {
                if(!trigger) {
                    if(timer.elapsed() > 17000) {
                        qDebug() << "Требуется сброс, похоже на зависание TEST!!! выходим из процедуры";
                        push_key("m");
                        QThread::msleep(2000);
                        return true;
                    }
                    sSearchImage = "pic_setToTarget";
                    srchAreaOnceInPart();
                    qDebug() << QString::number(coeff, 'f', 2) << "first";
                    if(coeff < diffCoef) {
                        coeff = 0;
                        sSearchImage = "pic_nothSetToTarget";
                        srchAreaOnceInPart();
                    }
                    qDebug() << QString::number(coeff, 'f', 2) << "second";
                    if(coeff < diffCoef) {
                        coeff = 0;
                        sSearchImage = "pic_nothHideSetToTarget";
                        srchAreaOnceInPart();
                    }
                    qDebug() << QString::number(coeff, 'f', 2) << "thirdm";
                    if(coeff > diffCoef) {
                        int x = point.x(), y = point.y();
                        mouse_move(x, y);
                        int relat = 5;
                        QThread::msleep(15);
                        mouse_move_rel(relat + 20, 0);
                        QThread::msleep(15);
                        mouse_move_rel(0, relat);
                        QThread::msleep(15);
                        mouse_move_rel(-relat, 0);
                        QThread::msleep(15);
                        mouse_move_rel(0, -relat);
                        QThread::msleep(5);
                        mouse_move_click(x + 60, y);
                        QThread::msleep(3000);
                        qDebug() << "target found" << point;
                        push_key("m");
                        QThread::msleep(2000);
                        return true;
                    }
                }
            }
        }
    }
//    mouse_move(point.x(), point.y());
    return false;
}

void MapSystem::printDebug()
{
    if(sys_debugLog) {
        qDebug() << "DEBUG: " << m_sActionName << list;
        QString sOut = QString("panel -> resLogic=%4")
//                .arg(sName)
                .arg(sys_resulLogic);
        qDebug() << qPrintable(sOut);
        qDebug() << "--------------------------------------------------";

    }


}

void MapSystem::reset()
{
}
