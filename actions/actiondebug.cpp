#include "actiondebug.h"

ActionDebug::ActionDebug(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "DEBUG";
}

void ActionDebug::init(QStringList &asListParam)
{
}

bool ActionDebug::logic(QStringList &asListParam)
{
    return true;
}

void ActionDebug::reset()
{
}
