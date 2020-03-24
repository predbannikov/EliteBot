#include "dockingmenucase.h"

DockingMenuCase::DockingMenuCase(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "DOCKINGMENUCASE";
}

void DockingMenuCase::init(QStringList &asListParam)
{
    trigger = false;
}

bool DockingMenuCase::logic(QStringList &asListParam)
{
    CursorPanel *pan = capture->menuDocking();
    if(pan->activeMenuDocking) {
        int res = comparisonStr(pan->sMenuDocking, asListParam[2]);
        if(res <= 2) {
            push_key(" ");
            return true;
        } else {
            push_key("s");
        }
    }
    QThread::msleep(100);
    return false;
}

void DockingMenuCase::reset()
{
}
