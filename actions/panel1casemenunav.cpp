#include "panel1casemenunav.h"

Panel1CaseMenuNav::Panel1CaseMenuNav(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{

}

void Panel1CaseMenuNav::init()
{
    timeWaitMsec = 3000;
}

bool Panel1CaseMenuNav::logic(QStringList &asListParam)
{
    CursorPanel *pan = capture->panel1Body();
    if(pan->activeBody) {
        QStringList list = buttonLeftNav.filter(pan->sBodyName);
        if(list.isEmpty()) {
            int res = comparisonStr(pan->sBodyName, asListParam[2]);
            if(res <= 2) {
                push_key(" ");
                return true;
            } else {
                push_key("s");
            }
        } else {
            push_key("d");
        }
    }
    return false;
}

void Panel1CaseMenuNav::reset()
{
    timeWaitMsec = 3000;
}
