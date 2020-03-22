#ifndef PANEL1CASEMENUNAV_H
#define PANEL1CASEMENUNAV_H
#include "baseaction.h"


class Panel1CaseMenuNav : public BaseAction
{
public:
    Panel1CaseMenuNav(CaptureWindow *aCapture, SocketIO *aSock);
    void init() override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};

#endif // PANEL1CASEMENUNAV_H
