#ifndef ACTIONDELIVERYPAPER_H
#define ACTIONDELIVERYPAPER_H
#include "baseaction.h"


class ActionDeliveryPaper : public BaseAction
{
    enum MENU {MENU_TRANS_1, MENU_TRANS_2, MENU_TRANS_3, MENU_TRANS_4, MENU_TRANS_5, MENU_TRANS_6, MENU_TRANS_7, MENU_TRANS_8} menu_trans;
    bool enableServiceDock();
    int countPart;
    int curPart;
    QString type;
    QPoint point;
public:
    ActionDeliveryPaper(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void printDebug() override;
    void reset() override;
};


#endif // ACTIONDELIVERYPAPER_H
