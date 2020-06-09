#include "actiondeliverypaper.h"

ActionDeliveryPaper::ActionDeliveryPaper(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "ACTIONDELIVERYPAPER";
}

void ActionDeliveryPaper::init(QStringList &asListParam)
{
    sys_debugLog = false;
    sTypeAction = asListParam[2];
//    sTypeAction = sTypeAction.simplified();
//    sTypeAction = sTypeAction.toLower();
//    deleteCharExtra(sTypeAction);
    countPart = asListParam[3].toInt();
    sTypeDeliver = asListParam[4];
    sTypeCargoUpLoad = asListParam[5];
    menu_trans = MENU_TRANS_1;
    curPart = 0;
}

bool ActionDeliveryPaper::logic(QStringList &asListParam)
{
    // pic_serviceDockMenuContacts
    int ret;
    QString sText;
    switch (menu_trans) {
    case MENU_TRANS_1:
        if(expectedImage("menuServiceExit", 0.8, 4, 12, 13)) {
            QThread::msleep(1000);
            point = capture->getPoint("pic_serviceDockMenuContacts");
            mouse_move(point.x(), point.y());
            QThread::msleep(500);
            mouse_click(0);
            qDebug() << asListParam;
            menu_trans = MENU_TRANS_2;
        }
        break;
    case MENU_TRANS_2:
        ret = comparisonStr("контакты", capture->getTextStaticField("pic_contactsHeader", cv::Scalar(0, 0, 0), cv::Scalar(120, 255, 255), "ru"));
        if(ret <= 2) {
            menu_trans = MENU_TRANS_3;
        }
        break;
    case MENU_TRANS_3: {
        sText = "pic_contactsRight1";
        QString sContPower = capture->getTextStaticField(sText.toStdString(), cv::Scalar(0, 0, 70), cv::Scalar(25, 255, 255), "ru");
        ret = comparisonStr("контактдержавы", sContPower);
        if(ret <= 2 || sContPower.contains("контактдержавы")) {
            point = capture->getPoint(sText);
            mouse_move(point.x(), point.y());
            QThread::msleep(500);
            mouse_click(0);

            menu_trans = MENU_TRANS_4;
        }
        sText = "pic_contactsRight2";
        sContPower = capture->getTextStaticField(sText.toStdString(), cv::Scalar(0, 0, 70), cv::Scalar(25, 255, 255), "ru");
        ret = comparisonStr("контактдержавы", sContPower);
        if(ret <= 2 || sContPower.contains("контактдержавы")) {
            point = capture->getPoint(sText);
            mouse_move(point.x(), point.y());
            QThread::msleep(500);
            mouse_click(0);

            menu_trans = MENU_TRANS_4;
        }
        sText = "pic_contactsRight3";
        sContPower = capture->getTextStaticField(sText.toStdString(), cv::Scalar(0, 0, 70), cv::Scalar(25, 255, 255), "ru");
        ret = comparisonStr("контактдержавы", sContPower);
        if(ret <= 2 || sContPower.contains("контактдержавы")) {
            point = capture->getPoint(sText);
//            mouse_move_click(point.x(), point.y());
            mouse_move(point.x(), point.y());
            QThread::msleep(500);
            mouse_click(0);
            menu_trans = MENU_TRANS_4;
        }
        sText = "pic_contactsRight4";
        sContPower = capture->getTextStaticField(sText.toStdString(), cv::Scalar(0, 0, 70), cv::Scalar(25, 255, 255), "ru");
        ret = comparisonStr("контактдержавы", sContPower);
        if(ret <= 2 || sContPower.contains("контактдержавы")) {
            point = capture->getPoint(sText);
//            mouse_move_click(point.x(), point.y());
            mouse_move(point.x(), point.y());
            QThread::msleep(500);
            mouse_click(0);

            menu_trans = MENU_TRANS_4;
        }
        break;
    }
    case MENU_TRANS_4:
        mouse_move(g_screen.width()/2, g_screen.height()/2);
        timer.restart();
        menu_trans = MENU_TRANS_5;
        QThread::msleep(3000);
        break;
    case MENU_TRANS_5: {
        if(timer.elapsed() < 1000) {
            QStringList listRects = {"pic_systemGain", "pic_systemSys"};
            bool resultSrch = false;
            cv::Rect rectForAction;
            cv::Rect rectSrch;
            cv::Point findPoint;
            double factor = 0;
            for(int j = 0; j < 2 && !resultSrch; j++) {
                for(int i = 0; i < 2 && !resultSrch ; i++) {
                    factor = 0;
                    if(j == 0) {
                        capture->srchAreaOnceInRect(listRects[i].toStdString(), factor, point, 28, 262, 431);
                    } else if( j == 1) {
                        capture->srchAreaOnceInRect(listRects[i].toStdString(), factor, point, 28, 458, 627);
                    }
//                    qDebug() << listRects[i] <<  "found" << factor;
                    if(factor > 0.9) {
                        rectForAction = capture->getRect("rect_ContactPowerMenu");
                        rectForAction.y = point.y() - (capture->getRect(listRects[i]).height / 2);
                        rectForAction.y = rectForAction.y - (capture->getRect("rect_ContactPowerMenu").height / 2);
                        rectSrch = capture->getRect(listRects[i]);
                        rectSrch.y = point.y() - (capture->getRect(listRects[i]).height / 2);
                        rectSrch.x = rectSrch.x + rectSrch.width + 2;
                        rectSrch.width = 500;
                        sText = capture->getTextArea(rectSrch, findPoint, "ru");
//                        qDebug() << sText;
                        if(sTypeAction == "Усилить систему") {
                            if(sText.contains("усилить") && sText.contains("систему")) {
                                qDebug() << "Усилить систему";
                                resultSrch = true;
                            }
                        } else if(sTypeAction == "Подготовить системы для державы") {
                            if(sText.contains("подготов") && sText.contains("системы")) {
                                qDebug() << "Подготовить системы для державы";
                                resultSrch = true;
                            }
                        } else if(sTypeAction == "Подготовить систему") {
                            if(sText.contains("подготов") && sText.contains("системy")) {
                                qDebug() << "Подготовить систему";
                                resultSrch = true;
                            }
                        } else if(sTypeAction == "Укрепить для державы") {
                            if(sText.contains("укрепить") && sText.contains("держав")) {
                                qDebug() << "Укрепить для державы";
                                resultSrch = true;
                            }
                        } else {
                            if(sText.contains("захватить") && sText.contains("держав")) {
                                qDebug() << "Захватить контроль для державы";
                                resultSrch = true;
                            }
                        }
//                        if(resultSrch)
//                            timer.restart();
    //                    return true;
                    }
                }
            }

            if(resultSrch) {
                sText = capture->getTextApproximArea(rectForAction, findPoint, "ru");
                qDebug() << ">>> Get text" << sText;
                if(sTypeDeliver == "разгрузка") {
                    if(sTypeCargoUpLoad == "0") {
                        qDebug() << "found" << " factor =" << factor;
                        mouse_move(point.x(), point.y() - 110);
                        QThread::msleep(500);
                        press_key("d");
                        QThread::msleep(17000);
                        release_key("d");
                        QThread::msleep(300);
                        push_key(" ");
                    } else if(sTypeCargoUpLoad == "1")  {
                        qDebug() << "Доставить ..." << findPoint.x << findPoint.y;
//                        mouse_move_click(findPoint.x, findPoint.y);
                        mouse_move(findPoint.x, findPoint.y);
                        QThread::msleep(500);
                        mouse_click(0);

                        curPart = 0;
                        countPart = -1;
                    }
                    menu_trans = MENU_TRANS_7;
                } else {
                    if(sText.contains("ускорить")) {
                        qDebug() << "Ускорить ..." << findPoint.x << findPoint.y;
//                        mouse_move_click(findPoint.x, findPoint.y);
                        mouse_move(findPoint.x, findPoint.y);
                        QThread::msleep(500);
                        mouse_click(0);
//                    } else if(sText.contains("доставить")){
//                        qDebug() << "if(resultSrch) { if(разгрузка) { if доставить {}}}";
////                        qDebug() << "Доставить ..." << findPoint.x << findPoint.y;
////                        mouse_move_click(findPoint.x, findPoint.y);
////                        curPart = 0;
////                        countPart = -1;
////                        menu_trans = MENU_TRANS_7;
                    } else {
                        qDebug() << "if(resultSrch) { if(разгрузка) { else {}}}";
//                        qDebug() << "found" << " factor =" << factor;
                        mouse_move(point.x(), point.y() - 110);
                    }
                    menu_trans = MENU_TRANS_6;
                }




                timer.restart();
                QThread::msleep(1000);
//                return true;
            }


        } else {
            timer.restart();
            mouse_wheel(-120);
        }
        break;
    }
    case MENU_TRANS_6:
//        return true;     // ***
//        if(sTypeCargoUpLoad == "0") {
//            press_key("d");
//            QThread::msleep(15000);
//            release_key("d");
//        } else if(sTypeCargoUpLoad == "1")  {

//        }
//        if(sTypeDeliver != "разгрузка") {
            press_key("d");
            QThread::msleep(5500);
            release_key("d");
            QThread::msleep(300);
//        } else {
//            press_key("d");
//            QThread::msleep(10000);
//            release_key("d");
//        }

        push_key(" ");
        QThread::msleep(2000);
        menu_trans = MENU_TRANS_7;

        break;
    case MENU_TRANS_7: {
//        return true;        // ***
        cv::Rect rect = capture->getRect("pic_menuServiceBack");
        cv::Point cvPoint;
        sText = capture->getTextApproximArea(rect, cvPoint, "ru");
        ret = comparisonStr("назад", sText);
        if(ret <= 1) {
            qDebug() << "sText =" << sText << cvPoint.x << cvPoint.y;
            mouse_move(cvPoint.x, cvPoint.y);
            QThread::msleep(500);
            mouse_click(0);
            QThread::msleep(1000);
            curPart++;
            if(curPart < countPart && sTypeDeliver == "загрузка") {
                menu_trans = MENU_TRANS_3;
            }
            return false;
        }
        sText = capture->getTextApproximArea(rect, cvPoint, "ru");
        ret = comparisonStr("выход", sText);
        if(ret <= 1) {
            qDebug() << "sText " << sText << cvPoint.x << cvPoint.y;
//            mouse_move_click(cvPoint.x, cvPoint.y);
            mouse_move(cvPoint.x, cvPoint.y);
            QThread::msleep(500);
            mouse_click(0);

            menu_trans = MENU_TRANS_8;
        }

        break;
    }
    case MENU_TRANS_8:
        menu_trans = MENU_TRANS_1;
        return true;
    }

    return false;
}

void ActionDeliveryPaper::printDebug()
{

}

void ActionDeliveryPaper::reset()
{
}
