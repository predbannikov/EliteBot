#ifndef PANEL1CASEHEAD_H
#define PANEL1CASEHEAD_H
#include "baseaction.h"


class Panel1CaseHead : public BaseAction
{
public:
    Panel1CaseHead(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};


#endif // PANEL1CASEHEAD_H
