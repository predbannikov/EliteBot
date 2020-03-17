#ifndef AICONTROL_H
#define AICONTROL_H
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QStringList>
#include "global.h"





class AIControl : QObject
{
    Q_OBJECT
public:
    SYS_STATE state;
    AIControl(QObject *parent = nullptr);
    ~AIControl();
//    QString getPathToTarget();

    bool            compass;
    bool            target;
    bool            check;
    QString         push_key;
    QString         press_key;
    QString         release_key;
    QString         panHeadName;
    QString         searchImage;
    QString         typingText;
    QStringList     listForSearch;
    QString         tmpStr;
    QStringList     tmpList;
    QElapsedTimer   timeElapsed;
    CursorPanel     *cursorPanel;
    double          coeff;
    double          radius;
    double          distance;
    int             frameFreq;
    int             timeWaitMsec;
    int             nCount;
    int             iStart;
    int             iEnd;
private:
    QString         m_fixCurrBugs;
//    enum {STAGE_1, STAGE_2, STAGE_3, STAGE_4, STAGE_5, STAGE_6, STAGE_7, STAGE_8, STAGE_9} stage;
//    enum {STAGE_PUSH_1, STAGE_PUSH_2} stagePush;
//    enum {STAGE_TRANS_RL_1, STAGE_TRANS_RL_2, STAGE_TRANS_RL_3} stage_trans_rl;
//    enum {STAGE_PAN_NAV, STAGE_PAN_TRAN, STAGE_PAN_CON} stage_pan;
    QStringList     m_slistStations;
    QStringList     m_slistCommand;

    QString         m_sStation;
    QString         m_sTarget;
    // ------------------------------------------------- Высокий уровня
public:
    bool smallRing();

    // ------------------------------------------------- Средний уровня
private:

//    int             higtMSec;
//    QElapsedTimer   highTimer;

    bool test(bool &bCheck);
    bool serviceMenuToFuel();
    bool serviceToRefuel();
    bool makeTrack();
    bool trnasHyperMode();


    // ------------------------------------------------- Нижний уровень
    bool aimp(int anMSec, bool &abCheck);
    bool setFrame(int anMSec);
    bool pickUpSpeed(int anMSec);
//    bool faceTheTarget();
    bool enableHelpAim();

    bool waitMenuService(int anMSec, bool &abCheck);
    bool waitEndHyperModeHelp(int anMSec, bool &abCheck);
    bool getCloser(bool &abCheck);
    bool toLanding(int anMSec);



    bool enabledPanel1(QString sName);                  // Включить панель 1
    bool disabledPanel1();
    bool takeOffIntoSpace();

    bool caseStationMenuNav(QString asStation);         // Выбрать станцию в списке навигации
    bool caseSubMenuNav(QString sNameSubMenu, bool &abCheck);

    bool caseMenuDocking(QString sNameDockMenu);           //
    bool caseMenuContact(QString sNameBodyMenu);        // На посадку           переделать и убрать

    bool waitDockingMenuShow(int anMSec, bool &abCheck);    // функция ветвления
    bool waitDockingMenuHide(int anMSec, bool &abCheck);    // функция ветвления
    bool waitMSec(int anMSec);

};

#endif // AICONTROL_H
