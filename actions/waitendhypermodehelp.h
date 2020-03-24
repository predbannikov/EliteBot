#ifndef WAITENDHYPERMODEHELP_H
#define WAITENDHYPERMODEHELP_H
#include "baseaction.h"


class WaitEndHyperModeHelp : public BaseAction
{
public:
    WaitEndHyperModeHelp(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};


#endif // WAITENDHYPERMODEHELP_H
