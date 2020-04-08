#include "getstrstaticfield.h"

GetStrStaticField::GetStrStaticField(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "GETSTRSTATICFIELD";

}

void GetStrStaticField::init(QStringList &asListParam)
{
    sys_debugLog = false;
    sSearchImage = asListParam[2];
}

bool GetStrStaticField::logic(QStringList &asListParam)
{
    sName = capture->getStrStaticField(sSearchImage.toStdString());
    qDebug() << "Получили строку" << sName;
    asListParam[2] = sName;
    asListParam[1] = "2";
    return true;
}

void GetStrStaticField::printDebug()
{
    if(sys_debugLog) {
        qDebug() << "DEBUG: " << m_sActionName << list;
        QString sOut = QString("panel -> resLogic=%4")
//                .arg(sName)
                .arg(sys_resulLogic);
        qDebug() << qPrintable(sOut);
        qDebug() << "--------------------------------------------------";

    }
}

void GetStrStaticField::reset()
{
}
