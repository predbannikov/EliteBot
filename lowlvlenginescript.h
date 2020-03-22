#ifndef LOWLVLENGINESCRIPT_H
#define LOWLVLENGINESCRIPT_H

#include <QString>
#include <QDebug>
#include <QElapsedTimer>
#include <QTcpSocket>
#include <QCoreApplication>
#include "capturewindow.h"
#include "global.h"
#include "socketio.h"
#include "actions/panel1Enable.h"
#include "actions/actionwait.h"



class LowLvlEngineScript
{
    QStringList     command;

    BaseAction          *parseAction();

    Panel1Enable        *panel1Enable;
    ActionWait          *actionWait;
    SocketIO            *m_sock;
public:
    virtual ~LowLvlEngineScript();
    LowLvlEngineScript();
    void update();
    bool cycle = true;
    virtual void init();
    CaptureWindow   *capture;
    IOData          *m_pIOData;
    std::map<std::string, ImageROI> *mp_dataSet;

protected:
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

    virtual void    readCommand(QStringList &aslistCommand) {}
public slots:

private:

};

#endif // LOWLVLENGINESCRIPT_H
