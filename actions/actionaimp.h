#ifndef ACTIONAIMP_H
#define ACTIONAIMP_H
#include "baseaction.h"


class ActionAimp : public BaseAction
{
public:
    ActionAimp(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};

#endif // ACTIONAIMP_H
