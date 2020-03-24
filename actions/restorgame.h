#ifndef RESTORGAME_H
#define RESTORGAME_H
#include "baseaction.h"

class RestorGame : public BaseAction
{
public:
    RestorGame(CaptureWindow *aCapture, SocketIO *aSock);
    void init(QStringList &asListParam) override;
    bool logic(QStringList &asListParam) override;
    void reset() override;
};


#endif // RESTORGAME_H
