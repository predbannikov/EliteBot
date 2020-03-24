#ifndef ACTIONDEBUG_H
#define ACTIONDEBUG_H
#include "baseaction.h"


class ActionDebug : public BaseAction
{
public:
    ActionDebug(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};

#endif // ACTIONDEBUG_H
