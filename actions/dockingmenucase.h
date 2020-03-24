#ifndef DOCKINGMENUCASE_H
#define DOCKINGMENUCASE_H

#include "baseaction.h"

class DockingMenuCase : public BaseAction
{
public:
    DockingMenuCase(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};

#endif // DOCKINGMENUCASE_H