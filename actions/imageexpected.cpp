#include "imageexpected.h"


ImageExpected::ImageExpected(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "IMAGEEXPECTED";
}

void ImageExpected::init(QStringList &asListParam)
{
    sSearchImage = asListParam[2];
    nCount = asListParam[3].toInt();
    iStart = asListParam[4].toInt();
    iEnd = asListParam[5].toInt();
    diffCoef = asListParam[6].toDouble();
//    sys_debugLog = true;
}

bool ImageExpected::logic(QStringList &asListParam)
{
    coeff = 0;
    srchAreaOnceInPart();
    if(coeff > diffCoef) {
        return true;
    } else {

        return false;
    }
}

void ImageExpected::printDebug()
{
    qDebug() << QString::number(coeff, 'f', 2);
}

void ImageExpected::reset()
{
}
