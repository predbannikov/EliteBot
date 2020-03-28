#ifndef MAPSYSTEMENABLE_H
#define MAPSYSTEMENABLE_H
#include "baseaction.h"

class MapSystemEnable : public BaseAction
{
public:
    MapSystemEnable(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};

#endif // MAPSYSTEMENABLE_H
