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
    trans = TRANS_1;
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
    switch(trans){
    case TRANS_1:
        _point = capture->getPoint("searchTrack_push");
        qDebug() << "searchTrack_push" << _point;
        mouse_move_click(_point.x(), _point.y());
        trans = TRANS_2;
        break;
    case TRANS_2:
        _point = capture->getPoint("fieldSearch");
        qDebug() << "fieldSearch" << asListParam[2];
        QThread::msleep(500);
        push_key("q");
        QThread::msleep(500);

        mouse_move(_point.x(), _point.y());

        for(int i = 0; i < 21; i++) {
            if(i < 11) {
                _point.setY(_point.y() - 3);
            } else {
                _point.setY(_point.y() + 3);
            }
            mouse_move(_point.x(), _point.y());
            QThread::msleep(10);

        }
        mouse_click(0);



        typingText(asListParam[2]);
        QThread::msleep(1000);
        trans = TRANS_3;
        break;
    case TRANS_3:
        _point = capture->getPoint("push_search");
        qDebug() << "push_search" << _point;
        QThread::msleep(500);
        mouse_move(_point.x(), _point.y());

        for(int i = 0; i < 21; i++) {
            if(i < 11) {
                _point.setX(_point.x() - 3);
            } else {
                _point.setX(_point.x() + 3);
            }
            mouse_move(_point.x(), _point.y());
            QThread::msleep(10);

        }

        mouse_click(0);

        QThread::msleep(5000);

        push_key("q");
        QThread::msleep(2000);
        QThread::msleep(1000);
        trans = TRANS_5;
        break;

    case TRANS_4: {         // Не обрабатываемое состояние сейчас.
        cv::Point _p;
        QString sText = capture->getTextApproximBoundingRect(capture->getRect("fieldSearch"), _p, "eng");
        if(comparisonStr(asListParam[2], sText) <= 1) {
            trans = TRANS_5;
        } else {
            trans = TRANS_2;
        }

        break;
    }
    case TRANS_5:
        _point = capture->getPoint("searchTrack_push");
        qDebug() << "TRANS_5: searchTrack_push" << _point;
        mouse_move(_point.x(), _point.y());
        for(int i = 0; i < 21; i++) {
            _point.setX(_point.x() - 3);
            mouse_move(_point.x(), _point.y());
            QThread::msleep(10);
        }
        mouse_click(0);
        trans = TRANS_6;
        QThread::msleep(1000);
        break;
    case TRANS_6: {
        cv::Point _p;
        qDebug() << "TRANS_6: rect_currStaySystemMap" << _point;
        if(capture->containTextMat(capture->getRect("rect_currStaySystemMap"), _p, asListParam[2], "eng", 1)) {
            trans = TRANS_7;
            timer.restart();
        } else {
//            trans = TRANS_1;


            push_key("m");
            QThread::msleep(3000);
            asListParam[1] = "1";
            asListParam[2] = "no cursor set system";
            qDebug() << "TEST!!! -> setCursorSystem =" << asListParam;
            return true;

        }

        break;
    }
    case TRANS_7:
        if(timer.elapsed() > 20000) {
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
            mouse_move_click(x + 60, y);
            QThread::msleep(3000);
            qDebug() << "target found" << point;
            push_key("m");
            QThread::msleep(2000);
            return true;
        }

        break;
    }

//    if(!searchTrack_push_trigger) {
//        _point = capture->getPoint("searchTrack_push");
//        qDebug() << "searchTrack_push" << _point;
//        mouse_move_click(_point.x(), _point.y());
////        mouseClick(_point);
//        searchTrack_push_trigger = true;
//    } else {
//        if(!fieldSearch_trigger) {
//            _point = capture->getPoint("fieldSearch");
//            qDebug() << "fieldSearch" << asListParam[2];
//            // ФИкс баг чей не понятно, без которого не работают клики по кнопкам
//            QThread::msleep(150);
////            press_key("a");
//////            QThread::msleep(10);
////            release_key("a");
//            push_key("a");
//            QThread::msleep(1500);
////            mouseClick(_point);
//            mouse_move_click(_point.x(), _point.y());
//            QThread::msleep(1500);
//            QApplication::processEvents();

//            typingText(asListParam[2]);
//            QApplication::processEvents();
//            QThread::msleep(1000);
//            fieldSearch_trigger = true;
////            mouse_press(1);
//        } else {
//            if(!push_search_trigger) {
//                _point = capture->getPoint("push_search");
//                qDebug() << "push_search" << _point;
////                QThread::msleep(50);
////                press_key("a");
////                QThread::msleep(50);
////                release_key("a");
////                QThread::msleep(150);
////                _point.setY(_point.y() + 35);
//                QApplication::processEvents();
//                QThread::msleep(1000);
//                mouse_move(_point.x(), _point.y());
//                mouse_move_click(_point.x(), _point.y());
//                QApplication::processEvents();
////               QThread::msleep(1000);
////                mouse_click(1);
////                mouse_move(_point.x(), _point.y());
////                QThread::msleep(2000);


////                mouse_move_click(_point.x(), _point.y());

////                QThread::msleep(1000);


////                mouseClick(_point);

////                push_f_key("ENTER");
//                QThread::msleep(1000);
//                QApplication::processEvents();
//                push_key("a");
////                press_key("a");
////                QThread::msleep(10);
////                QApplication::processEvents();
////                release_key("a");
//                QThread::msleep(4000);
//                push_search_trigger = true;
//                timer.restart();
//            } else {
//                if(!trigger) {
//                    QApplication::processEvents();
//                    if(timer.elapsed() > 17000) {
//                        qDebug() << "Требуется сброс, похоже на зависание TEST!!! выходим из процедуры";
//                        push_key("m");
//                        QThread::msleep(2000);
//                        return true;
//                    }
//                    sSearchImage = "pic_setToTarget";
//                    srchAreaOnceInPart();
//                    qDebug() << QString::number(coeff, 'f', 2) << "first";
//                    if(coeff < diffCoef) {
//                        coeff = 0;
//                        sSearchImage = "pic_nothSetToTarget";
//                        srchAreaOnceInPart();
//                    }
//                    qDebug() << QString::number(coeff, 'f', 2) << "second";
//                    if(coeff < diffCoef) {
//                        coeff = 0;
//                        sSearchImage = "pic_nothHideSetToTarget";
//                        srchAreaOnceInPart();
//                    }
//                    qDebug() << QString::number(coeff, 'f', 2) << "thirdm";
//                    if(coeff > diffCoef) {
//                        int x = point.x(), y = point.y();
//                        mouse_move(x, y);
////                        int relat = 5;
////                        QThread::msleep(15);
////                        mouse_move_rel(relat + 20, 0);
////                        QThread::msleep(15);
////                        mouse_move_rel(0, relat);
////                        QThread::msleep(15);
////                        mouse_move_rel(-relat, 0);
////                        QThread::msleep(15);
////                        mouse_move_rel(0, -relat);
////                        QThread::msleep(5);
//                        mouse_move_click(x + 60, y);
////                        mouse_release(1);
//                        QThread::msleep(3000);
//                        qDebug() << "target found" << point;
//                        push_key("m");
//                        QThread::msleep(2000);
//                        return true;
//                    }
//                }
//            }
//        }
//    }
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
