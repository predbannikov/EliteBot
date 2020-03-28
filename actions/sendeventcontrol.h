#ifndef SENDEVENTCONTROL_H
#define SENDEVENTCONTROL_H
#include "baseaction.h"

class SendEventControl : public BaseAction
{
public:
    SendEventControl(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
    void printDebug() override;
};

#endif // SENDEVENTCONTROL_H
