#ifndef BASEACTION_H
#define BASEACTION_H
#include <QElapsedTimer>
#include "socketio.h"
#include "global.h"
#include "capturewindow.h"
class BaseAction
{
    enum TRANSITION {TRANSITION_1, TRANSITION_2 } transition;
    SocketIO        *m_sock;

public:
    BaseAction(CaptureWindow *aCapture, SocketIO *aSock);
    virtual ~BaseAction();

    virtual void init(QStringList &asListParam) = 0;
    virtual bool logic(QStringList &asListParam) = 0;
    virtual void reset() = 0;
    virtual void printDebug();

    bool srchAreaOnceInPart();
    bool perform(QStringList &asListParam);
    void resetBase();
    void push_key(QString aChar);
    void press_key(QString aChar);
    void release_key(QString aChar);
    void typingText(QString asText);
    void move_mouse_rel(int x, int y);
    void mouse_move_click(int x, int y);

    QString         m_sActionName;
    QString getName();


    CaptureWindow   *capture;

    QElapsedTimer   sys_timeElapsed;
    int             sys_timeWaitMSec;
    bool            sys_resulLogic;
    bool            m_bCompass;
    bool            target;
    bool            check;
    bool            trigger;
//    QString         sPush_key;
//    QString         sPress_key;
//    QString         sRelease_key;
    QString         sPanHeadName;
    QString         sSearchImage;
    QString         sName;
    QString         sTypingText;
    QStringList     list;
    QStringList     listForSearch;
    QElapsedTimer   timer;
    QElapsedTimer   confirmTimer;
    double          coeff;
    double          radius;
    double          distance;
    int             waitEnable;
    int             confirmTime;
    int             waitMSec;
    int             whileWaitMsec;
    int             nCount;
    int             iStart;
    int             iEnd;
    qint64          lastElapse;

};

#endif // BASEACTION_H
