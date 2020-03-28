#ifndef MARKER_H
#define MARKER_H
#include "baseaction.h"


class Marker : public BaseAction
{
public:
    Marker(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void printDebug() override;
    void reset() override;
};


#endif // MARKER_H
