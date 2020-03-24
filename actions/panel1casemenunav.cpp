#include "panel1casemenunav.h"

Panel1CaseMenuNav::Panel1CaseMenuNav(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "PANEL1CASEMENUNAV";
}

void Panel1CaseMenuNav::init(QStringList &asListParam)
{
    sName = asListParam[2];
    list = asListParam;
}

bool Panel1CaseMenuNav::logic(QStringList &asListParam)
{
    pan = capture->panel1Body();
    if(pan->activeBody) {
        QStringList list = buttonLeftNav.filter(pan->sBodyName);
        if(list.isEmpty()) {
            int res = comparisonStr(pan->sBodyName, sName);
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
    QThread::msleep(100);
    return false;
}

void Panel1CaseMenuNav::printDebug()
{
    qDebug() << "DEBUG: " << m_sActionName << list;
    QString sOut = QString("panel -> name=%1 active=%2 \n\tsrchName=%3 resLogic=%4")
            .arg(pan->sBodyName)
            .arg(pan->activeBody)
            .arg(sName)
            .arg(sys_resulLogic);
    qDebug() << qPrintable(sOut);
    qDebug() << "--------------------------------------------------";

}

void Panel1CaseMenuNav::reset()
{
}
