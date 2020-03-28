#ifndef AIMPFLYAROUND_H
#define AIMPFLYAROUND_H
#include "baseaction.h"


class AimpFlyAround : public BaseAction
{

    enum {MOVEAWAY_TRANS_1, MOVEAWAY_TRANS_2} moveAwayTrans;
    enum {AIMPSIDEWAY_TRANS_1, AIMPSIDEWAY_TRANS_2, AIMPSIDEWAY_TRANS_3, AIMPSIDEWAY_TRANS_4} aimpSideWaysTrans;
    enum {TRANS_1, TRANS_2, TRANS_3} trans;


    QString key;
    QString sSide;

    double lastRadius = 0;

    int powerX;
    int seekY;
    int seekX;
    int mapSide = 0;

    bool waitCompasActive = false;
    bool offSet = false;

    bool pickup = false;
    bool pickdownX = false;
    bool pickdownY = false ;
    bool rotate = false ;
//    bool rotation(int anSide = 0);
    bool moveAway(Primitives *aPrim);
    bool compassAimp();
    bool aimpSideWays();
    bool aimpTarget();

    bool foundPrimitive(Primitives *aPrim);

public:
    AimpFlyAround(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};

#endif // AIMPFLYAROUND_H
