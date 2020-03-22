#include "panel1Enable.h"

Panel1Enable::Panel1Enable(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{

}

void Panel1Enable::init()
{
    timeWaitMsec = 3000;
}

bool Panel1Enable::logic(QStringList &asListParam)
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
    return false;
}

void Panel1Enable::reset()
{
    timeWaitMsec = 3000;
}
