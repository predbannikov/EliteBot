#include "panel1subnav.h"

Panel1SubNav::Panel1SubNav(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "PANEL1SUBNAV";
}

void Panel1SubNav::init(QStringList &asListParam)
{
}

bool Panel1SubNav::logic(QStringList &asListParam)
{
    pan = capture->subPanel1Nav();
    if(pan->activeSubNav) {
        int res;
        res = comparisonStr(pan->sSubNavName, asListParam[2]);
        if(res <= 2) {
            qDebug() << "Match success push_key(' ')";
            push_key(" ");
            return true;
        }
        res = comparisonStr(pan->sSubNavName, "missed_the_mark");
        if(res <= 2) {
            press_key("a");
            QThread::msleep(1000);
            release_key("a");
            qDebug() << "Не опознанные кнопки";
        } else {
            qDebug() << "Сдвигаем курсор";
            push_key("d");
        }

    } else {
        push_key("1");
        QThread::msleep(1000);
        press_key("a");
        mouse_move_rel(150, 150);
        QThread::msleep(5000);
        release_key("a");
        mouse_move_rel(-170, -170);
        push_key(" ");
        QThread::msleep(300);
        push_key("1");
        QThread::msleep(1700);
        push_key(" ");
        QThread::msleep(1500);
    }
    QThread::msleep(300);
    return false;
}

void Panel1SubNav::printDebug()
{
    qDebug() << "DEBUG: " << m_sActionName << list;
    QString sOut = QString("panel -> name=%1 active=%2 \n\tsrchName=%3 resLogic=%4")
            .arg(pan->sSubNavName)
            .arg(pan->activeSubNav)
            .arg(sName)
            .arg(sys_resulLogic);
    qDebug() << qPrintable(sOut);
    qDebug() << "--------------------------------------------------";


}

void Panel1SubNav::reset()
{
}
