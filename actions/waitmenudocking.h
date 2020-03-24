#ifndef WAITMENUDOCKING_H
#define WAITMENUDOCKING_H
#include "baseaction.h"


class WaitMenuDocking : public BaseAction
{
    CursorPanel *pan;
public:
    WaitMenuDocking(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};


#endif // WAITMENUDOCKING_H
