#ifndef IMAGEEXPECTED_H
#define IMAGEEXPECTED_H
#include "baseaction.h"

class ImageExpected : public BaseAction
{
public:
    ImageExpected(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};


#endif // IMAGEEXPECTED_H
