#include "dockingmenucase.h"

DockingMenuCase::DockingMenuCase(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "DOCKINGMENUCASE";
}

void DockingMenuCase::init(QStringList &asListParam)
{
    trigger = false;
    readyDockingMenu = false;
    countTry = 0;
}

bool DockingMenuCase::logic(QStringList &asListParam)
{
    CursorPanel *pan = capture->menuDocking();
    if(!readyDockingMenu) {
        if(pan->activeMenuDocking) {
            QThread::msleep(3500);
            readyDockingMenu = true;
        }
    } else {
        qDebug() << "распознаныая кнопка " << pan->sMenuDocking;
        if(pan->activeMenuDocking) {
            int res = comparisonStr(pan->sMenuDocking, asListParam[2]);
            if(res <= 2) {
                push_key(" ");
                QThread::msleep(500);
                return true;
            } else {
                if(countTry == 2) {
                    countTry =0;
                    push_key("w");
                    QThread::msleep(300);
                    push_key("w");
                    QThread::msleep(500);
                } else {
                    countTry++;
                    push_key("s");
                    QThread::msleep(500);
                }
            }
        }
    }

    QThread::msleep(150);
    return false;
}

void DockingMenuCase::reset()
{
}
