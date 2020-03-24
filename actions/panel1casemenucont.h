#ifndef PANEL1CASEMENUCONT_H
#define PANEL1CASEMENUCONT_H
#include "baseaction.h"

class Panel1CaseMenuCont : public BaseAction
{
    bool triggerForNotice;
    CursorPanel *pan;
public:
    Panel1CaseMenuCont(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};



#endif // PANEL1CASEMENUCONT_H
