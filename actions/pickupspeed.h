#ifndef PICKUPSPEED_H
#define PICKUPSPEED_H
#include "baseaction.h"

class PickUpSpeed : public BaseAction
{
    enum {TRANS_1, TRANS_2, TRANS_3, TRANS_4 } trans;
    int waitMSec;
public:
    PickUpSpeed(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};



#endif // PICKUPSPEED_H
