#ifndef PANEL1CASEMENUNAV_H
#define PANEL1CASEMENUNAV_H
#include "baseaction.h"


class Panel1CaseMenuNav : public BaseAction
{
    CursorPanel *pan;
public:
    Panel1CaseMenuNav(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void printDebug() override;
    void reset() override;
};

#endif // PANEL1CASEMENUNAV_H
