#ifndef BASEACTION_H
#define BASEACTION_H
#include <QElapsedTimer>
#include "socketio.h"
#include "global.h"
#include "capturewindow.h"
class BaseAction
{
    enum TRANSITION {TRANS_1, TRANS_2 } trans;
    SocketIO        *m_sock;

public:
    BaseAction(CaptureWindow *aCapture, SocketIO *aSock);
    virtual ~BaseAction();

    virtual void init() = 0;
    virtual bool logic(QStringList &asListParam) = 0;
    virtual void reset() = 0;

    bool perform(QStringList &asListParam);
    void printDebug();
    void resetBase();
    void push_key(QString aChar);
    void press_key(QString aChar);
    void release_key(QString aChar);
    void typingText(QString asText);


    CaptureWindow   *capture;

    bool            compass;
    bool            target;
    bool            check;
    QString         sPush_key;
    QString         sPress_key;
    QString         sRelease_key;
    QString         sPanHeadName;
    QString         sSearchImage;
    QString         sTypingText;
    QStringList     listForSearch;
    QElapsedTimer   timeElapsed;
    double          coeff;
    double          radius;
    double          distance;
    int             frameFreq;
    int             timeWaitMsec;
    int             whileWaitMsec;
    int             nCount;
    int             iStart;
    int             iEnd;


};

#endif // BASEACTION_H
