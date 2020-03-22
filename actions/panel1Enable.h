#ifndef PANEL1ENABLE_H
#define PANEL1ENABLE_H
#include "baseaction.h"

class Panel1Enable : public BaseAction
{
public:
    Panel1Enable(CaptureWindow *aCapture, SocketIO *aSock);
//    ~Panel1Enable() override;
    void init() override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};

#endif // PANEL1ENABLE_H
