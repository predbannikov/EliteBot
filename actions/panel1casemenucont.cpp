#include "panel1casemenucont.h"

Panel1CaseMenuCont::Panel1CaseMenuCont(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "PANEL1CASEMENUCONT";
}

void Panel1CaseMenuCont::init(QStringList &asListParam)
{
    sName = asListParam[2];
    waitMSec = 5000;
    triggerForNotice = false;
}

bool Panel1CaseMenuCont::logic(QStringList &asListParam)
{

    if(!triggerForNotice) {
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
        }
    } else {
        qDebug() << pan->sBodyNameNotice;
        pan = capture->panelBodyContNotice();
        int res = comparisonStr(pan->sBodyNameNotice, "отказановстыковке");
        if(res <= 2) {
            triggerForNotice = false;
            trigger = false;
            qDebug() << "ждём 15сек";
            QThread::msleep(waitMSec);
        }
        res = comparisonStr(pan->sBodyNameNotice, "стыковкаразрешена");
        if(res <= 2){
            qDebug() << "садимся";
            return true;
        }
    }
    QThread::msleep(100);
    return false;
}

void Panel1CaseMenuCont::reset()
{
}
