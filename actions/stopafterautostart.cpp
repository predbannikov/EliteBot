#include "stopafterautostart.h"


StopAfterAutoStart::StopAfterAutoStart(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "STOPAFTERAUTOSTART";
}

void StopAfterAutoStart::init(QStringList &asListParam)
{
    sSearchImage = "stopAfterAutoStart";
    coeff = 0.87;
    nCount = 4;
    iStart = 2;
    iEnd = 7;
}

bool StopAfterAutoStart::logic(QStringList &asListParam)
{

    if(srchAreaOnceInPart()) {
        return true;
    } else {
        return false;
    }

}

void StopAfterAutoStart::reset()
{
}
