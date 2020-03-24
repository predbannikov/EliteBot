#include "panel1casehead.h"

Panel1CaseHead::Panel1CaseHead(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "PANEL1CASEHEADER";
}

void Panel1CaseHead::init(QStringList &asListParam)
{
    trigger = false;
}

bool Panel1CaseHead::logic(QStringList &asListParam)
{
    CursorPanel *pan = capture->panel1Header();
    if(pan->activeHeader) {
        int res = comparisonStr(pan->sHeaderName, asListParam[2]);
        if(res <= 2) {
            return true;
        } else {
            push_key("e");
        }
    }
    QThread::msleep(100);
    return false;
}

void Panel1CaseHead::reset()
{
}
