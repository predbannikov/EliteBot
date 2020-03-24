#include "panel1Enable.h"

Panel1Enable::Panel1Enable(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "PANEL1ENABLE";
}

void Panel1Enable::init(QStringList &asListParam)
{
    waitEnable = 1500;
    confirmTime = 500;
    trigger = false;
    timer.restart();
    confirmTimer.restart();
}

bool Panel1Enable::logic(QStringList &asListParam)
{

    CursorPanel *pan = capture->panel1Header();
    bool enable = static_cast<bool>(asListParam[2].toUInt());
    if(pan->activeHeader == enable) {
        if(confirmTimer.elapsed() > confirmTime)
            return true;
    } else {
        confirmTimer.restart();
        if(!trigger) {
            timer.restart();
            push_key("1");
            trigger = true;
        } else {
            if(timer.elapsed() > waitEnable) {
                trigger = false;
            }
        }
    }
    return false;
}

void Panel1Enable::reset()
{
}
