#ifndef STOPAFTERAUTOSTART_H
#define STOPAFTERAUTOSTART_H
#include "baseaction.h"

class StopAfterAutoStart : public BaseAction
{
public:
    StopAfterAutoStart(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};

#endif // STOPAFTERAUTOSTART_H
