#ifndef ACTIONGETCLOSER_H
#define ACTIONGETCLOSER_H
#include "baseaction.h"


class ActionGetCloser : public BaseAction
{
public:
    ActionGetCloser(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};


#endif // ACTIONGETCLOSER_H
