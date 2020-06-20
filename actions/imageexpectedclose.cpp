#include "imageexpectedclose.h"

ImageExpectedClose::ImageExpectedClose(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "IMAGEEXPECTEDCLOSE";
}

void ImageExpectedClose::init(QStringList &asListParam)
{
    sSearchImage = asListParam[2];
    whileWaitMSec = asListParam[3].toUInt();
    diffCoef = asListParam[4].toDouble();
//    coeff = asListParam[4].toDouble();
    nCount = asListParam[5].toInt();
    iStart = asListParam[6].toInt();
    iEnd = asListParam[7].toInt();
    sModeAutoPilot = asListParam[8];

    lastElapse = 0;
}

bool ImageExpectedClose::logic(QStringList &asListParam)
{
    if(sSearchImage == "pic_autoPilot") {
        coeff = diffCoef;           // обязательно инициализировать коэффициэнт!!!
        int ret = 0;
        if(!capture->imageExpectedCloseAutoPilot(sSearchImage.toStdString(), coeff, ret, nCount, iStart, iEnd)) {             // Обязательно передать коофициэнт по которому функция может выйти из цикла
            qDebug() << "pattern hide time =" << timer.elapsed() << " factor =" << coeff << " diff =" << diffCoef << " ret =" << ret;
            if(ret == 1) {          // Значит перехват
                push_key("x");
                QThread::msleep(5000);
                asListParam[1] = "1";
                asListParam[2] = "detected intercept";
                qDebug() << "TEST!!! -> detected intercept  =" << asListParam;
                return true;
            }
            if(timer.elapsed() > whileWaitMSec) {
                return true;
            }
            qDebug() << "sSearchImage =" << sSearchImage << " sModeAutoPilot =" << sModeAutoPilot;
            if(sModeAutoPilot == "flyToStation")
                push_key("x");
        } else {
            timer.restart();
        }
    } else if(sSearchImage == "pic_warningRadTriangle") {
//        qDebug() << "pattern hide" << " factor =" << coeff;
        srchAreaOnceInPart();
        if(timer.elapsed() < whileWaitMSec) {
            if(coeff < diffCoef) {
//                qDebug() << "pattern image hide" << " factor =" << coeff;
            } else {
                qDebug() << "pattern image show"  << timer.elapsed() << "  whileWaitMSed =" << whileWaitMSec << " factor =" << coeff;
                asListParam[1] = "1";
                asListParam[2] = "warning triangle";
                qDebug() << "TEST!!! -> asListParam =" << asListParam;
                return true;
            }
        } else {
            qDebug() << "Предупреждения нет" << " factor =" << coeff;
            return true;
        }
    }
    return false;
}

void ImageExpectedClose::reset()
{
}
