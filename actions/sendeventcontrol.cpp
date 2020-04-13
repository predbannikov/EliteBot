#include "sendeventcontrol.h"

SendEventControl::SendEventControl(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "SENDEVENTCONTROL";
}

void SendEventControl::init(QStringList &asListParam)
{
    sName = asListParam[2];
}

bool SendEventControl::logic(QStringList &asListParam)
{
    if(sName == "push_key") {
        push_key(asListParam[3]);
        return true;
    }
    if(sName == "push_f_key") {
        push_f_key(asListParam[3]);
        return true;
    }
    if(sName == "mouse_move") {
        mouse_move(asListParam[3].toInt(), asListParam[4].toInt());
        return true;
    }
    return false;
}

void SendEventControl::printDebug()
{
    qDebug() << "DEBUG: " << m_sActionName << list;
    QString sOut = QString("panel -> name=%1 active=%2 \n\tsrchName=%3 resLogic=%4")

            .arg(sName)
            .arg(sys_resulLogic);
    qDebug() << qPrintable(sOut);
    qDebug() << "--------------------------------------------------";

}

void SendEventControl::reset()
{
}
