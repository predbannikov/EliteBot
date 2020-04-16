#ifndef CHECKCURSYSTEM_H
#define CHECKCURSYSTEM_H
#include "baseaction.h"


class CheckCurSystem : public BaseAction
{
    void mouseClick(QPoint aPoint);
    enum {TRANS_1, TRANS_2, TRANS_3, TRANS_4} trans;
public:
    CheckCurSystem(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void printDebug() override;
    void reset() override;
private:
};


#endif // CHECKCURSYSTEM_H
