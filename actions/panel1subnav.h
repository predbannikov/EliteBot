#ifndef PANEL1SUBNAV_H
#define PANEL1SUBNAV_H

#include "baseaction.h"

class Panel1SubNav : public BaseAction
{
    CursorPanel *pan;
public:
    Panel1SubNav(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void printDebug() override;
    void reset() override;
};


#endif // PANEL1SUBNAV_H
