#include "imageexpected.h"


ImageExpected::ImageExpected(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "IMAGEEXPECTED";
}

void ImageExpected::init(QStringList &asListParam)
{
    sSearchImage = asListParam[2];
    coeff = 0.9;
    nCount = 4;
    iStart = 12;
    iEnd = 13;
}

bool ImageExpected::logic(QStringList &asListParam)
{
    if(srchAreaOnceInPart()) {
        return true;
    } else {
        return false;
    }
}

void ImageExpected::reset()
{
}
