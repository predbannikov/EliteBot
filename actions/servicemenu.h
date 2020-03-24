#ifndef SERVICEMENU_H
#define SERVICEMENU_H
#include "baseaction.h"


class ServiceMenu : public BaseAction
{
public:
    ServiceMenu(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};


#endif // SERVICEMENU_H
