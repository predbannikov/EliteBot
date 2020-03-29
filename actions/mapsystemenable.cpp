#include "mapsystemenable.h"

MapSystemEnable::MapSystemEnable(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "MAPSYSTEMENABLE";
}

void MapSystemEnable::init(QStringList &asListParam)
{
    waitEnable = 5000;
    confirmTime = 500;
    sSearchImage = "pic_cartographics";
    nCount = 10;
    iStart = 0;
    iEnd = 1;
    coeff = 0.1;
    diffCoef = 0.9;
    key = asListParam[2];
}

bool MapSystemEnable::logic(QStringList &asListParam)
{
    if(!trigger) {
        qDebug() << "open map";
        push_key(key);
        trigger = true;
    }
    srchAreaOnceInPart();
    qDebug() << QString::number(coeff, 'f', 2);
    if(coeff > diffCoef) {
        QThread::msleep(1000);
        qDebug() << "success";
        return true;
    }

    if(timer.elapsed() > waitEnable) {
        timer.restart();
        trigger = false;
    }

    return false;
}

void MapSystemEnable::reset()
{
}
