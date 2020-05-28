#include "panel1casehead.h"

Panel1CaseHead::Panel1CaseHead(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "PANEL1CASEHEADER";
}

void Panel1CaseHead::init(QStringList &asListParam)
{
}

bool Panel1CaseHead::logic(QStringList &asListParam)
{


    //            if(pan->sHeaderName.contains("навигация") || pan->sHeaderName.contains("транзакции") || pan->sHeaderName.contains("контакты") ) {
    //                qDebug() << "Панель 1 включена:";
    //                return true;

    //            } else {


    CursorPanel *pan = capture->panel1Header(asListParam[2]);
    if( pan->activeHeader ) {
        if(pan->sHeaderName.contains(asListParam[2]) ) {
            return true;
        } else {
            push_key("e");
        }
    } else {
        push_key("0");
        QThread::msleep(500);
        push_key("0");

//        push_key("1");
        QThread::msleep(1000);
        mouse_move_rel(-210, 0);
        QThread::msleep(4000);
        mouse_move_rel(210, 0);
        push_key(" ");
        QThread::msleep(300);
        push_key("1");
        QThread::msleep(1700);
    }
    QThread::msleep(1000);


//    CursorPanel *pan = capture->panel1Header();
//    if(pan->activeHeader) {
//        int res = comparisonStr(pan->sHeaderName, asListParam[2]);
//        if(res <= 2) {
//            return true;
//        } else {
//            push_key("e");
//        }
//    }
//    QThread::msleep(100);
    return false;
}

void Panel1CaseHead::reset()
{
}
