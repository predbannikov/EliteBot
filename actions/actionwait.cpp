#include "actionwait.h"

ActionWait::ActionWait(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{

}

void ActionWait::init()
{
    timeWaitMsec = 3000;
}

bool ActionWait::logic(QStringList &asListParam)
{
    return false;
}

void ActionWait::reset()
{
    timeWaitMsec = 3000;
}
