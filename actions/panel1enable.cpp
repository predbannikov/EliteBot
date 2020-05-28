#include "panel1Enable.h"

Panel1Enable::Panel1Enable(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "PANEL1ENABLE";
}

void Panel1Enable::init(QStringList &asListParam)
{
    waitEnable = 2000;
    confirmTime = 700;
    trigger = false;
    timer.restart();
    confirmTimer.restart();
}

bool Panel1Enable::logic(QStringList &asListParam)
{

    bool enable = static_cast<bool>(asListParam[2].toUInt());
    if(enable) {
        push_key("1");
    } else {
        push_key("0");
        QThread::msleep(300);
        push_key("0");
    }

    QThread::msleep(2000);
    return true;
//    CursorPanel *pan = capture->panel1Header();
//    bool enable = static_cast<bool>(asListParam[2].toUInt());
//    if(pan->activeHeader == enable) {
//        if(confirmTimer.elapsed() > confirmTime) {
//            if(pan->sHeaderName.contains("навигация") || pan->sHeaderName.contains("транзакции") || pan->sHeaderName.contains("контакты") ) {
//                qDebug() << "Панель 1 включена:";
//                return true;

//            } else {
//                push_key("1");
//                QThread::msleep(500);
//                press_key("a");
//                QThread::msleep(5000);
//                release_key("a");
//            }
//        }
//    } else {
//        confirmTimer.restart();
//        if(!trigger) {
//            timer.restart();
//            push_key("1");
//            trigger = true;
//        } else {
//            if(timer.elapsed() > waitEnable) {
//                trigger = false;
//            }
//        }
//    }
//    return false;
}

void Panel1Enable::reset()
{
}
