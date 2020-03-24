#include "waitendhypermodehelp.h"

WaitEndHyperModeHelp::WaitEndHyperModeHelp(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "WAITENDHYPERMODEHELP";
}

void WaitEndHyperModeHelp::init(QStringList &asListParam)
{
    sSearchImage = "helpInHypModeTriangle";
    whileWaitMsec = 9000;
    coeff = 0.70;
    nCount = 14;
    iStart = 61;
    iEnd = 78;
    lastElapse = 0;
}

bool WaitEndHyperModeHelp::logic(QStringList &asListParam)
{
    if(!srchAreaOnceInPart()) {
        if(timer.elapsed() - lastElapse > whileWaitMsec) {
            return true;
        }
        qDebug() << "pattern image hide" << lastElapse << timer.elapsed() - lastElapse << "  whileWaitMSed =" << whileWaitMsec;
    } else {
        lastElapse = timer.elapsed();
    }
    return false;
}

void WaitEndHyperModeHelp::reset()
{
}
