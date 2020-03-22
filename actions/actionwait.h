#ifndef ACTIONWAIT_H
#define ACTIONWAIT_H
#include "baseaction.h"


class ActionWait : public BaseAction
{
public:
    ActionWait(CaptureWindow *aCapture, SocketIO *aSock);
    void init() override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};

#endif // ACTIONWAIT_H
