#include "waitmenudocking.h"

WaitMenuDocking::WaitMenuDocking(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "WAITMENUDOCKING";
}

void WaitMenuDocking::init(QStringList &asListParam)
{
}

bool WaitMenuDocking::logic(QStringList &asListParam)
{
    pan = capture->menuDocking();
    if(pan->activeMenuDocking) {
        return true;
    }
    return false;
}

void WaitMenuDocking::reset()
{
}

