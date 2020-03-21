#ifndef LOWLVLENGINESCRIPT_H
#define LOWLVLENGINESCRIPT_H

#include <QString>
#include <QDebug>
#include <QElapsedTimer>
#include <QTcpSocket>
#include <QCoreApplication>
#include "capturewindow.h"
#include "global.h"



class LowLvlEngineScript
{
    QStringList     command;

    static QTcpSocket      *sock;
    static CaptureWindow   *capture;
    static QPoint          m_pointOffsetScreen;

public:
    LowLvlEngineScript();
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

    virtual bool processing(QString sName, bool &abCheck, int anMSec);
    virtual bool proc() = 0;
    virtual bool interapt() = 0;
    virtual bool update() = 0;

    static inline void push_key();
    static inline void push_key(QString aChar);
    static inline void press_key(QString aChar);
    static inline void release_key(QString aChar);
    static inline void press_key();
    static inline void release_key();
    static inline void typingText();
    static inline void move_mouse_rel(int x, int y);
    static inline void mouse_move_click(cv::Point cvPoint);
    static inline void sendDataToSlave(QByteArray a_data);
private:
    ~LowLvlEngineScript();
};

#endif // LOWLVLENGINESCRIPT_H
