#include "actiondeliverypaper.h"

ActionDeliveryPaper::ActionDeliveryPaper(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "ACTIONDELIVERYPAPER";
}

void ActionDeliveryPaper::init(QStringList &asListParam)
{
    sys_debugLog = false;
    type = asListParam[2];
    countPart = asListParam[3].toInt();
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
            mouse_move_click(point.x(), point.y());
            menu_trans = MENU_TRANS_2;
        }
        break;
    case MENU_TRANS_2:
        ret = comparisonStr("контакты", capture->getTextStaticField("pic_contactsHeader", cv::Scalar(0, 0, 0), cv::Scalar(120, 255, 255), "ru"));
        if(ret <= 2) {
            menu_trans = MENU_TRANS_3;
        }
        break;
    case MENU_TRANS_3:
        sText = "pic_contactsRight1";
        ret = comparisonStr("контактдержавы", capture->getTextStaticField(sText.toStdString(), cv::Scalar(0, 0, 70), cv::Scalar(25, 255, 255), "ru"));
        if(ret <= 2) {
            point = capture->getPoint(sText);
            mouse_move_click(point.x(), point.y());
            menu_trans = MENU_TRANS_4;
        }
        sText = "pic_contactsRight2";
        ret = comparisonStr("контактдержавы", capture->getTextStaticField(sText.toStdString(), cv::Scalar(0, 0, 70), cv::Scalar(25, 255, 255), "ru"));
        if(ret <= 2) {
            point = capture->getPoint(sText);
            mouse_move_click(point.x(), point.y());
            menu_trans = MENU_TRANS_4;
        }
        sText = "pic_contactsRight3";
        ret = comparisonStr("контактдержавы", capture->getTextStaticField(sText.toStdString(), cv::Scalar(0, 0, 70), cv::Scalar(25, 255, 255), "ru"));
        if(ret <= 2) {
            point = capture->getPoint(sText);
            mouse_move_click(point.x(), point.y());
            menu_trans = MENU_TRANS_4;
        }
        sText = "pic_contactsRight4";
        ret = comparisonStr("контактдержавы", capture->getTextStaticField(sText.toStdString(), cv::Scalar(0, 0, 70), cv::Scalar(25, 255, 255), "ru"));
        if(ret <= 2) {
            point = capture->getPoint(sText);
            mouse_move_click(point.x(), point.y());
            menu_trans = MENU_TRANS_4;
        }
        break;
    case MENU_TRANS_4:
        mouse_move(g_screen.width()/2, g_screen.height()/2);
        timer.restart();
        menu_trans = MENU_TRANS_5;
        QThread::msleep(3000);
        break;
    case MENU_TRANS_5: {
        double factor = 0;
        capture->srchAreaOnceInRect("pic_systemGain", factor, point, 9, 21, 39);
        if(timer.elapsed() < 1000) {
            if(factor > 0.93) {
                qDebug() << "found power";
                cv::Rect rect = capture->getRect("rect_ContactPowerMenu");
                rect.y = point.y() - (capture->getRect("pic_systemGain").height / 2);
                rect.y = rect.y - (capture->getRect("rect_ContactPowerMenu").height / 2);

                cv::Point findPoint;
                sText = capture->getTextApproximArea(rect, findPoint, "ru");
                qDebug() << "Get text" << sText;
                if(sText.contains("ускорить")) {
                    qDebug() << "Ускорить ..." << findPoint.x << findPoint.y;
                    mouse_move_click(findPoint.x, findPoint.y);
                } else if(sText.contains("доставить")){
                    qDebug() << "Доставить ..." << findPoint.x << findPoint.y;
                    mouse_move_click(findPoint.x, findPoint.y);
                    curPart = 0;
                    countPart = -1;
                    menu_trans = MENU_TRANS_7;
                } else {
                    qDebug() << "found" << " factor =" << factor;
                    mouse_move(point.x(), point.y() - 110);
                }
                timer.restart();
                menu_trans = MENU_TRANS_6;
                QThread::msleep(1000);
            }
        } else {
            timer.restart();
            mouse_wheel(-120);
        }
        break;
    }
    case MENU_TRANS_6:
        press_key("d");
        QThread::msleep(2000);
        release_key("d");
        push_key(" ");
        QThread::msleep(2000);
        menu_trans = MENU_TRANS_7;

        break;
    case MENU_TRANS_7: {
        cv::Rect rect = capture->getRect("pic_menuServiceBack");
        cv::Point cvPoint;
        sText = capture->getTextApproximArea(rect, cvPoint, "ru");
        ret = comparisonStr("назад", sText);
        if(ret <= 1) {
            qDebug() << "sText =" << sText << cvPoint.x << cvPoint.y;
            mouse_move_click(cvPoint.x, cvPoint.y);
            QThread::msleep(1000);
            curPart++;
            if(curPart < countPart) {
                menu_trans = MENU_TRANS_3;
            }
            return false;
        }
        sText = capture->getTextApproximArea(rect, cvPoint, "ru");
        ret = comparisonStr("выход", sText);
        if(ret <= 1) {
            qDebug() << "sText " << sText << cvPoint.x << cvPoint.y;
            mouse_move_click(cvPoint.x, cvPoint.y);
            menu_trans = MENU_TRANS_8;
        }

        break;
    }
    case MENU_TRANS_8:
        return true;
        break;
    }

    return false;
}

void ActionDeliveryPaper::printDebug()
{

}

void ActionDeliveryPaper::reset()
{
}
