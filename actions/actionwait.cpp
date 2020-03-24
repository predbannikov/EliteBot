#include "actionwait.h"


ActionWait::ActionWait(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "ACTIONWAIT";
}

void ActionWait::init(QStringList &asListParam)
{
    timer.restart();
}

bool ActionWait::logic(QStringList &asListParam)
{
    qint64 elaps = timer.elapsed();
    if(elaps > asListParam[2].toInt()) {
        return true;
    }
    return false;
}

void ActionWait::reset()
{
}
