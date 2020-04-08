#ifndef GETSTRSTATICFIELD_H
#define GETSTRSTATICFIELD_H
#include "baseaction.h"


class GetStrStaticField : public BaseAction
{
public:
    GetStrStaticField(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void printDebug() override;
    void reset() override;
};

#endif // GETSTRSTATICFIELD_H
