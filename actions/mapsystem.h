#ifndef MAPSYSTEM_H
#define MAPSYSTEM_H
#include "baseaction.h"


class MapSystem : public BaseAction
{
    enum {TRANS_1, TRANS_2, TRANS_3, TRANS_4, TRANS_5, TRANS_6, TRANS_7} trans;
    bool searchTrack_push_trigger;
    bool fieldSearch_trigger;
    bool push_search_trigger;
    void mouseClick(QPoint point);
public:
    MapSystem(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void printDebug() override;
    void reset() override;
};

#endif // MAPSYSTEM_H
