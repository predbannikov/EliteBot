#ifndef AICONTROL_H
#define AICONTROL_H
#include <QObject>
#include <QDebug>
#include <QStringList>
#include "global.h"





class AIControl : QObject
{
    Q_OBJECT
public:
    SYS_STATE state;
    AIControl(QObject *parent = nullptr);
    ~AIControl();
    void next();
    QString getPathToTarget();

    bool            check;
    QString         push_key;
    QString         panHeadName;
    QString         searchImage;
    QString         typingText;
    QStringList     listForSearch;
    QString         tmpStr;
    QStringList     tmpList;
    QElapsedTimer   timeElapsed;
    CursorPanel     *cursorPanel;
    int             timeWaitMsec;
    int             nCount;
    int             iStart;
    int             iEnd;
private:
    enum {WAIT, WHERE_IAM, GET_THE_GOAL, BUILD_TRACK,
            ENABLED_PANEL1, FIND_ITEM, ENABLED_HYPER, REQ_DOCKING,
         MENU_DOCKING_AUTOSTART, WAIT_DOCKING_MENU_SHOW, WAIT_DOCKING_MENU_HIDE} action;
//    enum {STAGE_1, STAGE_2, STAGE_3, STAGE_4, STAGE_5, STAGE_6, STAGE_7, STAGE_8, STAGE_9} stage;
//    enum {STAGE_PUSH_1, STAGE_PUSH_2} stagePush;
//    enum {STAGE_TRANS_RL_1, STAGE_TRANS_RL_2, STAGE_TRANS_RL_3} stage_trans_rl;
//    enum {STAGE_PAN_NAV, STAGE_PAN_TRAN, STAGE_PAN_CON} stage_pan;
    QStringList     m_slistStations;
    QStringList     m_slistCommand;

    QString         m_sStation;
    QString         m_sTarget;
//    void where_iam();
//    void build_track();
    bool enabledPanel1(QString sName);

    bool caseStationMenuNav(QString asStation);         // Выбрать станцию в списке навигации
    bool caseSubMenuNav(QString sNameSubMenu);

    bool caseMenuContact(QString sNameBodyMenu);        // На посадку

    bool waitDockingMenuShow(int anMSec, bool &abCheck);    // функция ветвления
    bool waitDockingMenuHide(int anMSec, bool &abCheck);    // функция ветвления
    bool waitMSec(int anMSec);
    bool caseMenuDocking(QString sNameDockMenu);           //
};

#endif // AICONTROL_H
