#include "imageexpected.h"


ImageExpected::ImageExpected(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "IMAGEEXPECTED";
}

void ImageExpected::init(QStringList &asListParam)
{
    sSearchImage = asListParam[2];
    diffCoef = asListParam[3].toDouble();
    nCount = asListParam[4].toInt();
    iStart = asListParam[5].toInt();
    iEnd = asListParam[6].toInt();
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
