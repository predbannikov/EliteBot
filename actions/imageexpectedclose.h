#ifndef WAITENDHYPERMODEHELP_H
#define WAITENDHYPERMODEHELP_H
#include "baseaction.h"


class ImageExpectedClose : public BaseAction
{
    QString sModeAutoPilot;
public:
    ImageExpectedClose(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};


#endif // WAITENDHYPERMODEHELP_H
