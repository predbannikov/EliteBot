#include "actiondeliverypaper.h"

#define TIME_FOR_WAIT_TO_LOAD   5500
#define DEBUG_DELIVERY 2


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
    wheelCount = 0;
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
        wheelCount = 0;
        QThread::msleep(3000);

        break;
    case MENU_TRANS_5: {
//        if(timer.elapsed() < 1000) {
#define COUNT_WHEEL_TOBORDER    15
        if(wheelCount > COUNT_WHEEL_TOBORDER) {   // По идее пришли к границе прокрутки больше быть не может
            wheelCount = 0;
            menu_trans = MENU_TRANS_7;
        } else {

            QStringList listRects = {"pic_systemGain", "pic_systemSys"};
            bool resultSrch = false;
            cv::Rect rectSrch;
            cv::Point findPoint;
            double factor = 0;
//            for(int j = 0; j < 2 && !resultSrch; j++) {
                for(int i = 0; i < 2 && !resultSrch ; i++) {
                    factor = 0;
                    if(wheelCount == COUNT_WHEEL_TOBORDER) {        // Если на границе проверяем нижний квадрат
//                        if(j == 0) {
                        capture->srchAreaOnceInRect(listRects[i].toStdString(), factor, point, 28, 430, 655);
                    } else {
                        capture->srchAreaOnceInRect(listRects[i].toStdString(), factor, point, 28, 262, 431);
                    }
//                    else if( i == 1) {
////                    } else if( j == 1) {
//                        capture->srchAreaOnceInRect(listRects[i].toStdString(), factor, point, 28, 458, 627);
//                    }
                    QApplication::processEvents();
                    qDebug() << listRects[i] <<  "found" << factor;
                    if(factor > 0.9) {
                        cv::Rect rectForFeildAction = capture->getRect("rect_ContactPowerMenu");
//                        rectForFeildAction =
                        rectForFeildAction.y = point.y() - (capture->getRect(listRects[i]).height / 2) - capture->getRect("rect_ContactPowerMenu").height;          // pint это центр найденного квадрата, это для выравнивания интересуемого поля
//                        rectForAction.y = rectForAction.y - (capture->getRect("rect_ContactPowerMenu").height / 2);
                        rectSrch = capture->getRect(listRects[i]);
                        rectSrch.y = point.y() - (capture->getRect(listRects[i]).height / 2);
                        rectSrch.x = rectSrch.x + rectSrch.width + 2;
                        rectSrch.width = 500;
                        sText = capture->getTextArea(rectSrch, findPoint, "ru");
                        QApplication::processEvents();
//                        qDebug() << sText;
                        qDebug() << "found" << " factor =" << factor;

                        if(sTypeAction == "Усилить систему") {
                            if(sText.contains("усилить") && sText.contains("систему")) {
                                qDebug() << "Усилить систему";
//                                resultSrch = true;
                                resultSrchDeliviryActionField(rectForFeildAction, findPoint);
                                return false;
                            }
                        } else if(sTypeAction == "Подготовить системы для державы") {
                            if(sText.contains("подготов") && sText.contains("системы")) {
                                qDebug() << "Подготовить системы для державы";
//                                resultSrch = true;
                                resultSrchDeliviryActionField(rectForFeildAction, findPoint);
                                return false;
                            }
                        } else if(sTypeAction == "Подготовить систему") {
                            if(sText.contains("подготов") && sText.contains("системy")) {
                                qDebug() << "Подготовить систему";
//                                resultSrch = true;
                                resultSrchDeliviryActionField(rectForFeildAction, findPoint);
                                return false;
                            }
                        } else if(sTypeAction == "Укрепить для державы") {
                            if(sText.contains("укрепить") && sText.contains("держав")) {
                                qDebug() << "Укрепить для державы";
                                resultSrchDeliviryActionField(rectForFeildAction, findPoint);
                                return false;
//                                resultSrch = true;
                            }
                        } else {
                            if(sText.contains("захватить") && sText.contains("держав")) {
                                qDebug() << "Захватить контроль для державы";
                                resultSrchDeliviryActionField(rectForFeildAction, findPoint);
                                return false;
//                                resultSrch = true;
                            }
                        }
//                        if(resultSrch)
//                            timer.restart();
    //                    return true;
                    }
//                    QThread::msleep(2000);
                }
//            }

//            if(resultSrch) {
//                sText = capture->getTextApproximArea(rectForAction, findPoint, "ru");
//                qDebug() << ">>> Get text" << sText;
//                if(sTypeDeliver == "разгрузка") {
//                    if(sTypeCargoUpLoad == "0") {
//                        qDebug() << "found" << " factor =" << factor;
//                        mouse_move(point.x(), point.y() - 110);
//                        QThread::msleep(500);
//                        press_key("d");
//                        QThread::msleep(17000);
//                        release_key("d");
//                        QThread::msleep(300);
//                        push_key(" ");
//                    } else if(sTypeCargoUpLoad == "1")  {
//                        qDebug() << "Доставить ..." << findPoint.x << findPoint.y;
//                        mouse_move(findPoint.x, findPoint.y);
//                        QThread::msleep(500);
//                        mouse_click(0);
//                        curPart = 0;
//                        countPart = -1;
//                    }
//                    menu_trans = MENU_TRANS_7;
//                } else {
//                    if(sText.contains("ускорить")) {
//                        qDebug() << "Ускорить ..." << findPoint.x << findPoint.y;
//                        mouse_move(findPoint.x, findPoint.y);
//                        QThread::msleep(500);
//                        mouse_click(0);
//                    } else {
//                        qDebug() << "if(resultSrch) { if(разгрузка) { else {}}}";
////                        qDebug() << "found" << " factor =" << factor;
//                        mouse_move(point.x(), point.y() - 110);
//                    }
//                    menu_trans = MENU_TRANS_6;
//                }
//                timer.restart();
//                QThread::msleep(1000);
//                return true;
//            }


//        } else {
            wheelCount++;
            qDebug() << " mouse wheel event";
            timer.restart();
            mouse_wheel(-120);
//        }
        QThread::msleep(700);
        }
        break;

    }
    case MENU_TRANS_6:

        press_key("d");
        if(DEBUG_DELIVERY == 1) {
            QThread::msleep(100);
        } else {
            QThread::msleep(TIME_FOR_WAIT_TO_LOAD);
        }
        release_key("d");
        QThread::msleep(300);


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
            push_key("0");

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
            mouse_move(cvPoint.x, cvPoint.y);
            QThread::msleep(500);
            push_key("0");
            menu_trans = MENU_TRANS_8;
        }

        break;
    }
    case MENU_TRANS_8:
        menu_trans = MENU_TRANS_1;
        return true;
    }

    QThread::msleep(100);
    return false;
}

void ActionDeliveryPaper::resultSrchDeliviryActionField(cv::Rect aRectFoFieldrAction, cv::Point aFindPoint)
{
    QString sText;


    sText = capture->getTextApproximArea(aRectFoFieldrAction, aFindPoint, "ru");
    QApplication::processEvents();
    qDebug() << ">>> Get text" << sText;
//    QThread::msleep(60000);
//    if(DEBUG_DELIVERY == 2) {
//        qDebug() << "move to found picture";
//        for(int i = 0; i < 100; i++) {
//            mouse_move(aRectFoFieldrAction.x, aRectFoFieldrAction.y + aRectFoFieldrAction.height);
//            QThread::msleep(1000);
//            mouse_move(aRectFoFieldrAction.x, aRectFoFieldrAction.y);
//            QThread::msleep(1000);
//        }
//        QThread::msleep(100000);
//        return;
//    }


//    cv::Rect rectQuad = capture->getRect("rect_ContactPowerMenu");            // !!!
    if(sTypeDeliver == "разгрузка") {
        if(sTypeCargoUpLoad == "0") {

//            qDebug() << "found" << " factor =" << factor;
            mouse_move(point.x(), point.y() - 110);
//            mouse_move(point.x(), point.y() - (rectQuad.height / 2) - 15 );

            QThread::msleep(500);
            press_key("d");
            QThread::msleep(17000);
            release_key("d");
            QThread::msleep(300);
            push_key(" ");
        } else if(sTypeCargoUpLoad == "1")  {
            qDebug() << "Доставить ..." << aFindPoint.x << aFindPoint.y;
            mouse_move(aFindPoint.x, aFindPoint.y);
            QThread::msleep(500);
            mouse_click(0);
            curPart = 0;
            countPart = -1;
        }
        menu_trans = MENU_TRANS_7;
    } else {

        if(sText.contains("ускорить")) {
            qDebug() << "Ускорить ..." << aFindPoint.x << aFindPoint.y;
            mouse_move(aFindPoint.x, aFindPoint.y);
            QThread::msleep(500);
            mouse_click(0);
        } else {
            qDebug() << "if(resultSrch) { if(разгрузка) { else {}}}";
//                        qDebug() << "found" << " factor =" << factor;
            mouse_move(point.x(), point.y() - 110);
//            mouse_move(point.x(), point.y() - (rectQuad.height / 2) - 15 );

        }
        menu_trans = MENU_TRANS_6;
    }
    timer.restart();
    QThread::msleep(1000);
}


void ActionDeliveryPaper::printDebug()
{

}

void ActionDeliveryPaper::reset()
{
}
