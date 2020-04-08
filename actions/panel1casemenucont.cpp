#include "panel1casemenucont.h"

Panel1CaseMenuCont::Panel1CaseMenuCont(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "PANEL1CASEMENUCONT";
}

void Panel1CaseMenuCont::init(QStringList &asListParam)
{
    sName = asListParam[2];
    waitMSec = 7000;
    triggerForNotice = false;
}

bool Panel1CaseMenuCont::logic(QStringList &asListParam)
{

    if(!triggerForNotice) {
        qDebug() << "Panel1CaseMenuCont::logic ->     if(!triggerForNotice) {";
        pan = capture->panel1Body();
        if(!trigger) {
            qDebug() << "RECOGNIZE: "<< pan->sBodyName << " сравниваем с" << sName;
            int res = comparisonStr(pan->sBodyName, sName);
            if(res <= 2) {
                push_key("d");
                trigger = true;
            } else {
                push_key("s");
            }
        } else {
            int res = comparisonStr(pan->sBodyName, "req_docking");                 // Требуется посадка
            if(res <= 2) {
                push_key(" ");
            } else {
                push_key(" ");
                qDebug() << "Нажал вправо но несработало, пытаемся нажать пробел и проверить что получится" << pan->sBodyName << sName;
            }
            triggerForNotice = true;
            timer.restart();
        }
    } else {
        qDebug() << "TEST!!!pan->sBodyNameNotice;" << pan->sBodyNameNotice;
        pan = capture->panelBodyContNotice();
        qDebug() << "начало сравниваний";
        if(timer.elapsed() < waitMSec) {
            int res;
            res = comparisonStr(pan->sBodyNameNotice, "отказановстыковке");
            if(res <= 2) {
                qDebug() << "ждём 15сек";
    //            QThread::msleep(waitMSec);
                triggerForNotice = false;
                trigger = false;
                QThread::msleep(waitMSec);
                return false;
            }
            res = comparisonStr(pan->sBodyNameNotice, "стыковкаразрешена");
            if(res <= 2){
                qDebug() << "садимся";
                return true;
            }
            res = comparisonStr(pan->sBodyNameNotice, "Notice: not active");
            if(res <= 2){
                push_key("s");
                qDebug() << "ждём 15сек";
                triggerForNotice = false;
                trigger = false;
                QThread::msleep(waitMSec);
                return false;
            }
        } else {
            triggerForNotice = false;
            trigger = false;
        }
//        QThread::msleep(waitMSec);
//        triggerForNotice = false;
//        trigger = false;
    }
    QThread::msleep(100);
    return false;
}

void Panel1CaseMenuCont::reset()
{
}
