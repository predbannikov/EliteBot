/*

0	1	2	3	4
5	6	7	8	9
10	11	12	13	14
15	16	17	18	19
20	21	22	23	24

0	1	2	3
4	5	6	7
8	9	10	11
12	13	14	15

0	1	2
3	4	5
6	7	8

*/
#include "aicontrol.h"

AIControl::AIControl(QObject *parent) : QObject(parent)
{
    m_slistStations << "celebicity" << "gabrielenterprise";
    m_slistCommand << "transport";
    m_sTarget = m_slistStations[1];             //  текущую станцию указать

    queue.enqueue(QStringList {"PANEL1ENABLE", "0", "навигация"});
}

AIControl::~AIControl()
{

}

bool AIControl::smallRing()
{
    static enum {BUILD_TRACK, AUTOSTART, PICK_UP_SPEED, FLY_TO_THE_TARGET, AIMING_ON_TARGET, ENABLE_HELP_AIM,
                 WAIT_END_HYPER_MODE, GET_CLOSER, REQ_LANDING, TO_REFUEL,
                 STATE_TEST, WAIT} action = STATE_TEST;

    switch (action) {
    case BUILD_TRACK:
        if(makeTrack()) {
            qDebug() << "BUILD_TRACK";
            action = AUTOSTART;
        }
        break;
    case AUTOSTART: {
        if(takeOffIntoSpace()) {
            action = PICK_UP_SPEED;
            qDebug() << "AUTOSTART";
        }
        break;
    }
    case PICK_UP_SPEED: {
        if(pickUpSpeed(5000)) {
            action = AIMING_ON_TARGET;
            qDebug() << "PICK_UP_SPEED";
        }
        break;
    }
    case AIMING_ON_TARGET: {
        bool checkState = false;
        if(aimp(100000, checkState)) {
            action = FLY_TO_THE_TARGET;
            qDebug() << "AIMING_ON_TARGET";
        }
        break;
    }
    case FLY_TO_THE_TARGET:
        if(trnasHyperMode()) {
            qDebug() << "FLY_TO_THE_TARGET";
            action = ENABLE_HELP_AIM;
        }
        break;
    case ENABLE_HELP_AIM:
        if(enableHelpAim()) {
            action = WAIT_END_HYPER_MODE;
            qDebug() << "ENABLE_HELP_AIM";
        }
        break;
    case WAIT_END_HYPER_MODE: {
        bool checkState = false;
        if(waitEndHyperModeHelp(1800000, checkState)) {
            if(checkState) {
                qDebug() << "WAIT_END_HYPER_MODE";
                action = GET_CLOSER;
            } else {
                qDebug() << "WAIT_END_HYPER_MODE not success";
                action = WAIT;
            }
        }
        break;
    }
    case GET_CLOSER: {
        bool checkState = false;
        if(getCloser(checkState)) {
            if(checkState) {
                qDebug() << "GET_CLOSER";
                action = REQ_LANDING;
            } else {
                qDebug() << "GET_CLOSER not success";
                action = WAIT;
            }
        }
        break;
    }
    case REQ_LANDING: {
        if(toLanding(120000)) {
            action = TO_REFUEL;
            qDebug() << "REQ_LANDING";
        }
        break;
    }
    case TO_REFUEL:
        if(serviceToRefuel()) {
            action = BUILD_TRACK;
            qDebug() << "TO_REFUEL";
        }
        break;
    case STATE_TEST: {
        bool checkState = false;
        if(getCloser(checkState)) {
            if(checkState) {
                qDebug() << "Test success";
                action = WAIT;
            } else {
                qDebug() << "Test not success";
                action = WAIT;
            }
        }
        break;
    }
    case WAIT:
        if(waitMSec(45000)) {
            qDebug() << "AIControl::smallRing: state WAIT";
        }

        break;
    }
    return false;
}

QStringList AIControl::readCommand()
{
    if(!queue.empty()) {
        return queue.dequeue();
    } else {
        QStringList list = { "WAIT", "0" };
        return list;
    }
}

void AIControl::resetState()
{

}

bool AIControl::test(bool &bCheck)
{
    static enum {TRANS_1} action;

    switch (action) {

    case TRANS_1: {
        if(enableHelpAim()) {
            action = TRANS_1;
            qDebug() << "exit test";
            bCheck = true;
            return true;
        }
        break;
    }

    }
    return false;
}

bool AIControl::serviceToRefuel()
{
    static enum {TRANS_1, TRANS_2, TRANS_3} trans;

    switch (trans) {
    case TRANS_1:
        if(caseMenuDocking("menu_docking_service"))
            trans = TRANS_2;
        break;
    case TRANS_2: {
            bool checkState = false;
            if(waitMenuService(5000, checkState)) {
                if(checkState)  {
                    trans = TRANS_3;
                } else {
                    trans = TRANS_1;
                }
            }
        }
        break;
    case TRANS_3:
        if(serviceMenuToFuel()) {
            trans = TRANS_1;
            return true;
        }
        break;
    }
    return false;
}

bool AIControl::makeTrack()
{
    static enum {PREP_TRACK, ENABLED_PANEL_1, CASE_STATION, CASE_MAKE_TARGET } action;

    switch (action) {
    case PREP_TRACK: {
        int iTarget = m_slistStations.indexOf(m_sTarget);
        if(iTarget == m_slistStations.size() - 1)
            iTarget = 0;
        else
            iTarget++;
        m_sTarget = m_slistStations[iTarget];
        action = ENABLED_PANEL_1;
        break;
    }
    case ENABLED_PANEL_1: {
        if(enabledPanel1("навигация")) {
            qDebug() << "ENABLED_PANEL_1";
            action = CASE_STATION;
        }
        break;
    }
    case CASE_STATION: {
        if(caseStationMenuNav(m_sTarget)) {
            action = CASE_MAKE_TARGET;
            qDebug() << "CASE_STATION";
        }
        break;
    }
    case CASE_MAKE_TARGET: {
            bool checkState = false;
            if(caseSubMenuNav("fix_target", checkState)) {
                if(checkState) {
                    action = PREP_TRACK;
                    qDebug() << "CASE_MAKE_TARGET";
                    return true;
                } else {
                    action = ENABLED_PANEL_1;
                    qDebug() << "CASE_MAKE_TARGET not success" ;
                }
            }


        break;
    }

    }
    return false;
}

bool AIControl::trnasHyperMode()
{
    static enum {/*AUTOSTART*/ ENABLED_PANEL_1, CASE_STATION, CASE_MAKE_HYPER_MODE, WAIT_HYPER_CHARG} action;

    switch (action) {
    case ENABLED_PANEL_1:
        if(enabledPanel1("навигация")) {
            qDebug() << "ENABLED_PANEL_1";
            action = CASE_STATION;
        }
        break;
    case CASE_STATION:
        if(caseStationMenuNav(m_sTarget)) {
            action = CASE_MAKE_HYPER_MODE;
            qDebug() << "CASE_STATION";
        }
        break;
    case CASE_MAKE_HYPER_MODE: {
        bool checkState = false;
        if(caseSubMenuNav("enable_hypermode", checkState)) {
            if(checkState) {
                qDebug() << "CASE_MAKE_HYPER_MODE";
                action = WAIT_HYPER_CHARG;
            } else {
                qDebug() << "CASE_MAKE_HYPER_MODE not success";
                action = ENABLED_PANEL_1;
            }
        } else {
            qDebug() << "PANIC: AIControl::trnasHyperMode() not success";

        }
        break;
    }
    case WAIT_HYPER_CHARG:
        if(waitMSec(13000)) {
            action = ENABLED_PANEL_1;
            return true;
        }

        break;
    }
    return false;
}

bool AIControl::aimp(int anMSec, bool &abCheck)
{
    static enum {TRANS_0, TRANS_1, TRANS_2, TRANS_3, TRANS_4, TRANS_5, TRANS_6, TRANS_7, TRANS_8} trans ;
    switch (trans) {
    case TRANS_0:
        frameFreq = 30;
        trans = TRANS_1;
        state = SET_FRAME_FREQ;
        break;
    case TRANS_1:
        check = false;
        compass = false;
        timeWaitMsec = anMSec;
        timeElapsed.restart();
        state = COMPASS;
        trans = TRANS_2;
        break;
    case TRANS_2:
        push_key = " ";
        trans = TRANS_3;
        state = PUSH_KEY;
        break;
    case TRANS_3:
        timeElapsed.restart();
        timeWaitMsec = 1500;
        state = WAIT_MSEC;
        trans = TRANS_4;
        break;
    case TRANS_4:
        check = false;
        target = false;
        timeWaitMsec = anMSec;
        timeElapsed.restart();
        state = AIMING;
        trans = TRANS_5;
        break;
    case TRANS_5:
        if(target) {
            trans = TRANS_1;
        } else {
            push_key = " ";
            trans = TRANS_6;
            state = PUSH_KEY;
        }
        break;
    case TRANS_6:
        frameFreq = 100;
        trans = TRANS_7;
        state = SET_FRAME_FREQ;
        break;
    case TRANS_7:
        timeElapsed.restart();
        timeWaitMsec = 1500;
        state = WAIT_MSEC;
        trans = TRANS_8;
        break;
    case TRANS_8:
        trans = TRANS_0;
        abCheck = check;
        return true;
    }
    return false;
}

bool AIControl::setFrame(int anMSec)
{
    static enum {TRANS_1, TRANS_2} trans;
    switch (trans) {
    case TRANS_1:
        frameFreq = anMSec;
        trans = TRANS_2;
        state = SET_FRAME_FREQ;
        break;
    case TRANS_2:
        trans = TRANS_1;
        return true;
    }
    return false;
}

bool AIControl::pickUpSpeed(int anMSec)
{
    static enum {TRANS_1, TRANS_2, TRANS_3, TRANS_4, TRANS_5} trans ;
    switch (trans) {
    case TRANS_1:
        press_key = "w";
        state = PRESS_KEY;
        trans = TRANS_2;
        qDebug() << "AIControl::pickUpSpeed::TRANS_1 -> press_key 'w'";
        break;
    case TRANS_2:
        timeWaitMsec = anMSec;
        timeElapsed.restart();
        state = WAIT_MSEC;
        trans = TRANS_3;
        qDebug() << "AIControl::pickUpSpeed::TRANS_2 -> (WAIT_MSEC) timeElapse.reset =" << timeElapsed.elapsed();
        break;
    case TRANS_3:
        release_key = "w";
        qDebug() << "AIControl::pickUpSpeed::TRANS_3 => release_key 'w' timeElapse.elapse =" << timeElapsed.elapsed();
        state = RELEASE_KEY;
        trans = TRANS_4;
        break;
    case TRANS_4:
        timeWaitMsec = anMSec;
        timeElapsed.restart();
        state = WAIT_MSEC;
        trans = TRANS_5;
        break;
    case TRANS_5:
        trans = TRANS_1;
        return true;
    }
    return false;
}

//bool AIControl::faceTheTarget()
//{

//    return false;
//}

bool AIControl::enableHelpAim()
{
    static enum {ENABLE_PANEL_NAVI, SELECT_STAT, ENABLE_HELP_AIM_MIDDLE} action ;
    switch (action) {
    case ENABLE_PANEL_NAVI:
        if(enabledPanel1("навигация")) {
            action = SELECT_STAT;
            qDebug() << "ENABLE_PANEL_NAVI";
        }
        break;
    case SELECT_STAT:
        if(caseStationMenuNav(m_sTarget)) {
            action = ENABLE_HELP_AIM_MIDDLE;
            qDebug() << "SELECT_STAT";
        }
        break;
    case ENABLE_HELP_AIM_MIDDLE:
        bool checkState = false;
        if(caseSubMenuNav("enable_hypermode_helper", checkState)) {
            if(checkState) {
                qDebug() << "ENABLE_HELP_AIM";
                action = ENABLE_PANEL_NAVI;
                return true;
            } else {
                qDebug() << "ENABLE_HELP_AIM not succeess";
                action = ENABLE_PANEL_NAVI;
            }
        }
        break;
    }
    return false;
}

bool AIControl::serviceMenuToFuel()
{
    static enum {TRANS_1, TRANS_2, TRANS_3, TRANS_4, TRANS_5} trans ;
    switch (trans) {
    case TRANS_1:
        qDebug() << "AIControl::serviceMenuToFuel() click to fuel";
        searchImage = "rectServiceFuel";
        state = CLICK_TO_POINT;
        trans = TRANS_2;
        break;
    case TRANS_2:
        if(waitMSec(1500)) {
            trans = TRANS_3;
        }
        break;
    case TRANS_3:
        qDebug() << "AIControl::serviceMenuToFuel() click to exit";
        searchImage = "rectServiceExit";
        state = CLICK_TO_POINT;
        trans = TRANS_4;
        break;
    case TRANS_4: {
        bool checkState = false;
        if(waitMenuService(1500, checkState)) {
            if(checkState)  {
                qDebug() << "AIControl::serviceMenuToFuel() закрытие меню сервиса не состоялось";
                trans = TRANS_1;
            } else {
                trans = TRANS_5;
                qDebug() << "AIControl::serviceMenuToFuel() меню сервиса закрыто";
            }
        }
        break;
    }
    case TRANS_5:
        if(waitMSec(1000)) {
            trans = TRANS_1;
            return true;
        }
        break;

    }
    return false;
}

//QString AIControl::getPathToTarget()
//{
//    return m_slistStations[0];
//}

bool AIControl::waitMenuService(int anMSec, bool &abCheck)
{
    static enum {TRANS_1, TRANS_2} trans ;
    switch (trans) {
    case TRANS_1:
        check = false;
        timeElapsed.restart();
        searchImage = "menuServiceExit";
        timeWaitMsec = anMSec;
        coeff = 0.9;
        nCount = 4;
        iStart = 12;
        iEnd = 13;
        state = SEARCH_IMAGE_CONTINUOUS;
        trans = TRANS_2;
        break;
    case TRANS_2:
        trans = TRANS_1;
        abCheck = check;
        return  true;
    }
    return false;
}

bool AIControl::waitEndHyperModeHelp(int anMSec, bool &abCheck)
{
    static enum {TRANS_0, TRANS_1, TRANS_2, TRANS_3} trans ;
    switch (trans) {
    case TRANS_0:
        state = WAIT_MSEC;
        timeElapsed.restart();
        timeWaitMsec = 7000;
        trans = TRANS_1;
        break;
    case TRANS_1:
        check = false;
        timeElapsed.restart();
        searchImage = "helpInHypModeTriangle";
        timeWaitMsec = anMSec;
        whileWaitMsec = 9000;
        coeff = 0.70;
        nCount = 14;
        iStart = 61;
        iEnd = 78;
        state = WHILE_IMAGE_CONTINUOUS;
        trans = TRANS_2;
        break;
    case TRANS_2:
        timeWaitMsec = 50;
        timeElapsed.restart();
        state = WAIT_MSEC;
        trans = TRANS_3;
        break;
    case TRANS_3:
        trans = TRANS_0;
        abCheck = check;
        return  true;
    }
    return false;
}

bool AIControl::getCloser(bool &abCheck)
{
    static enum {TRANS_1, TRANS_2} trans ;
    switch (trans) {
    case TRANS_1:
        check = false;
        timeWaitMsec = 45000;
        timeElapsed.restart();
        distance = 7.;
        state = APPROACH;               // Не работает как надо, основано на простой задержке по времени
        trans = TRANS_2;
        break;
    case TRANS_2: {
        if(check) {
            abCheck = true;
            trans = TRANS_1;
            return true;
        } else {
            qDebug() << "approach not success";
            abCheck = false;
            trans = TRANS_1;
            return true;
        }
    }

    }
    return false;
}

bool AIControl:: toLanding(int anMSec)
{
    static enum {ENABLED_PANEL_1, CASE_STATION, WAIT_DOCKING_MENU_SHOW, WAIT_SHORT} action;

    switch (action) {
    case ENABLED_PANEL_1:
        if(enabledPanel1("контакты")) {
            qDebug() << "ENABLED_PANEL_1";
            action = CASE_STATION;
        }
        break;
    case CASE_STATION:
        if(caseMenuContact(m_sTarget)) {
            action = WAIT_DOCKING_MENU_SHOW;
            qDebug() << "CASE_STATION";
        }
        break;
    case WAIT_DOCKING_MENU_SHOW: {
        bool checkState = false;
        if(waitDockingMenuShow(anMSec, checkState)) {
            if(checkState) {
                qDebug() << "WAIT_DOCKING_MENU_SHOW";
                action = WAIT_SHORT;
            } else {
                qDebug() << "посадка не состаялась";
                action = ENABLED_PANEL_1;
            }
        }
        break;
    }
    case WAIT_SHORT:
        if(waitMSec(3000)) {
            action = ENABLED_PANEL_1;
            return true;
        }
        break;
    }
    return false;
}


//void AIControl::build_track()
//{
//    switch (stage) {
//    case STAGE_1:
//        push_key = "m";
//        state = PUSH_KEY;
//        stage = STAGE_2;
//        break;
//    case STAGE_2:
//        check = false;
//        timeElapsed.restart();
//        searchImage = "menuM";
//        timeWaitMsec = 3000;
//        nCount = 4;
//        iStart = 0;
//        iEnd = 0;
//        stage = STAGE_3;
//        state = SEARCH_IMAGE_CONTINUOUS;
//        break;
//    case STAGE_3:
//        if(check) {
//            qDebug() << searchImage << "this picture found";
//            timeElapsed.restart();
//            check = false;
//            searchImage = "searchTrack_push";
//            state = SEARCH_IMAGE_CONTINUOUS;
//            stage = STAGE_4;
//        } else {
//            qDebug() << "PANIC: this picture not found";
//            action = WAIT;
//        }
//        break;
//    case STAGE_4:
//        if(check) {
//            timeElapsed.restart();
//            qDebug() << searchImage << "this picture found";
//            state = CLICK_POINT_IMAGE_AFTER_LOOK;
//            stage = STAGE_5;
//        } else {
//            qDebug() << "PANIC: this picture >" << searchImage << "not found";
//            action = WAIT;
//        }
//        break;
//    case STAGE_5:
//        check = false;
//        timeElapsed.restart();
//        searchImage = "fieldSearch";
//        timeWaitMsec = 3000;
//        nCount = 3;
//        iStart = 0;
//        iEnd = 0;
//        stage = STAGE_6;
//        state = SEARCH_IMAGE_CONTINUOUS;
//        break;
//    case STAGE_6:
//        if(check) {
//            timeElapsed.restart();
//            qDebug() << searchImage << "this picture found";
//            state = CLICK_POINT_IMAGE_AFTER_LOOK;
//            stage = STAGE_7;
//        } else {
//            qDebug() << "PANIC: this picture >" << searchImage << "not found";
//            action = WAIT;
//        }
//        break;
//    case STAGE_7:
//        state = TYPING_TEXT;
//        typingText = m_sTarget;
//        stage = STAGE_8;
//        break;
//    case STAGE_8:
//        check = false;
//        timeElapsed.restart();
//        searchImage = "push_search";
//        stage = STAGE_9;
//        state = SEARCH_IMAGE_CONTINUOUS;
//        break;
//    case STAGE_9:
//        if(check) {
//            timeElapsed.restart();
//            qDebug() << searchImage << "this picture found";
//            state = CLICK_POINT_IMAGE_AFTER_LOOK;
//            stage = STAGE_1;
//        } else {
//            qDebug() << "PANIC: this picture >" << searchImage << "not found";
//            action = WAIT;
//        }
//        action = WAIT;                                          // Незабыть заглушку!
//        break;
//    }
//}


bool AIControl::enabledPanel1(QString sName)                // Включение меню с нужным разделом
{
    static enum {TRANS_1, TRANS_2} trans ;

    switch (trans) {
    case TRANS_1:               // Чтение состояния панели
        qDebug() << "Считываем состояние панели 1";
        trans = TRANS_2;
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_PANEL1;
        break;
    case TRANS_2:
        if(check) {
            qDebug() << "RECOGNIZE: "<< cursorPanel->sHeaderName;
            int res = comparisonStr(cursorPanel->sHeaderName, sName);
            if(res <= 2) {
                trans = TRANS_1;
                return true;
            } else {
                push_key = "e";
                trans = TRANS_1;
                state = PUSH_KEY;
            }
        } else {
            push_key = "1";
//            qDebug() << "Enabled panel";
            trans = TRANS_1;
            state = PUSH_KEY;
        }
        break;

    }
    return false;
}

bool AIControl::takeOffIntoSpace()
{
    static enum { MENU_DOCKING_AUTOSTART, WAIT_TAKE_OFF_INFO_SPACE} action;
    switch (action) {
    case MENU_DOCKING_AUTOSTART:
        if(caseMenuDocking("menu_docking_autostart")) {

            action = WAIT_TAKE_OFF_INFO_SPACE;
            qDebug() << "MENU_DOCKING_AUTOSTART";
        }
        break;
    case WAIT_TAKE_OFF_INFO_SPACE: {
            bool menuDock = false;
            if(waitDockingMenuHide(100000, menuDock)) {
                action = MENU_DOCKING_AUTOSTART;
                qDebug() << "WAIT_TAKE_OFF_INFO_SPACE";
                if(menuDock) {
                    qDebug() << "Взлёт закончен";
                    return true;
                } else {
                    qDebug() << "Взлёт не состоялся";
                    return false;
                }
            }
        }
        break;
    }
    return false;
}

bool AIControl::caseStationMenuNav(QString asStation)
{
    static enum {TRANS_1, TRANS_2} trans ;
    switch (trans) {
    case TRANS_1:
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_BODY_CURSOR;
        trans = TRANS_2;
        break;
    case TRANS_2:
        if(check) {
            qDebug() << "RECOGNIZE: "<< cursorPanel->sBodyName << " сравниваем с" << asStation;
            QStringList list = buttonLeftNav.filter(cursorPanel->sBodyName);
            if(list.isEmpty()) {
                int res = comparisonStr(cursorPanel->sBodyName, asStation);
                if(res <= 2) {
                    push_key = " ";
                    state = PUSH_KEY;
                    trans = TRANS_1;
                    return true;
                } else {
                    push_key = "s";
                    trans = TRANS_1;
                    state = PUSH_KEY;
                }
            } else {
                push_key = "d";
                trans = TRANS_1;
                state = PUSH_KEY;
            }
        } else {
            qDebug() << "PANIC: AIControl::caseStationMenuNav(QString asStation) state not handled";
            trans = TRANS_1;
            state = PUSH_KEY;
        }
        break;
    }
    return false;
}

bool AIControl::caseSubMenuNav(QString sNameSubMenu, bool &abCheck)
{
    static enum {TRANS_1, TRANS_2, TRANS_3} trans ;
    switch (trans) {
    case TRANS_1:
        qDebug() << "AIControl::caseSubMenuNav state TRANS_1 " ;
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_SUB_CURSOR;
        trans = TRANS_2;
        break;
    case TRANS_2:
        qDebug() << "AIControl::caseSubMenuNav check resuld" << check;
        if(check) {
            qDebug() << "AIControl::caseSubMenuNav" << cursorPanel->sSubNavName << sNameSubMenu;
            int res = comparisonStr(cursorPanel->sSubNavName, sNameSubMenu);
            if(res <= 2) {
                qDebug() << "Сравнивание прошло успешно, нажимаем 'пробел'";
                push_key = " ";
                state = PUSH_KEY;
                trans = TRANS_3;
            } else {
                if(comparisonStr(cursorPanel->sSubNavName, "missed_the_mark") <= 2)
                {
                    qDebug() << "Не опознанные кнопки";
                    trans = TRANS_1;
                    abCheck = false;
                    return true;
                } else {
                    qDebug() << "Сдвигаем курсор";
                    push_key = "d";
                    trans = TRANS_1;
                    state = PUSH_KEY;
                }
            }

        } else {
            qDebug() << "PANIC: AIControl::caseSubMenuNav not handled state";
            trans = TRANS_1;
            abCheck = false;
            return true;
        }
        break;
    case TRANS_3:
        abCheck = true;
        push_key = "1";
        state = PUSH_KEY;
        trans = TRANS_1;
        return true;
    }
    return false;
}

bool AIControl::caseMenuContact(QString sNameBodyMenu)                          // Меню посадкиы
{
    static enum {TRANS_1, TRANS_2, TRANS_3, TRANS_4, TRANS_5} trans ;
    switch (trans) {
    case TRANS_1:
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_BODY_CURSOR;
        trans = TRANS_2;
        break;
    case TRANS_2:
        qDebug() << "RECOGNIZE: "<< cursorPanel->sBodyName << " сравниваем с" << sNameBodyMenu;
        if(check) {
            int res = comparisonStr(cursorPanel->sBodyName, sNameBodyMenu);
            if(res <= 2) {
                push_key = "d";
                state = PUSH_KEY;
                trans = TRANS_3;
            } else {
                push_key = "s";
                trans = TRANS_1;
                state = PUSH_KEY;
            }
        } else {
            push_key = "s";
            trans = TRANS_1;
            state = PUSH_KEY;
            qDebug() << "отсутствует курсор в контактах, пробуем нажать вниз" << sNameBodyMenu;
        }
        break;
    case TRANS_3:
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_BODY_CURSOR;
        trans = TRANS_4;
        break;
    case TRANS_4:
        if(check) {
            int res = comparisonStr(cursorPanel->sBodyName, "req_docking");                 // Требуется посадка
            if(res <= 2) {
                push_key = " ";
                state = PUSH_KEY;
                trans = TRANS_5;
            } else {
                push_key = " ";
                state = PUSH_KEY;
                trans = TRANS_3;
                qDebug() << "Нажал вправо но несработало, пытаемся нажать пробел и проверить что получится" << sNameBodyMenu;
            }
        } else {
            state = PUSH_KEY;
            trans = TRANS_1;
            qDebug() << "PANIC: cursors caseMenuCont req_docking not found timeout" << sNameBodyMenu;
        }
        break;
    case TRANS_5:
        push_key = "1";
        state = PUSH_KEY;
        trans = TRANS_1;
        return true;
    }
    return false;
}

bool AIControl::caseMenuDocking(QString sNameDockMenu)                                  // Нажать кнопку автостарт
{

    static enum {TRANS_1, TRANS_2/*, TRANS_3, TRANS_4*/} trans ;
    switch (trans) {
    case TRANS_1:
        qDebug() << "Считываем меню докинга ";
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_MENU_DOCKING;
        trans = TRANS_2;
        break;
    case TRANS_2:
//        qDebug() << cursorPanel->sBodyName;
        if(check) {
            qDebug() << "Сравниваем " << cursorPanel->sMenuDocking << sNameDockMenu;
            int res = comparisonStr(cursorPanel->sMenuDocking, sNameDockMenu);
            if(res <= 2) {
                qDebug() << "нажимаем 'пробел'" << sNameDockMenu;
                push_key = " ";
                state = PUSH_KEY;
                trans = TRANS_1;
                return true;
            } else {
                push_key = "s";
                trans = TRANS_1;
                state = PUSH_KEY;
            }
        } else {
            qDebug() << "Считывание не удалось";
            trans = TRANS_1;
        }
        break;
//    case TRANS_3:
//        timeWaitMsec = 10000;
//        timeElapsed.restart();
//        check = false;
//        state = TRANS_MENU_DOCKING;
//        break;
//    case TRANS_4:
//        if(check) {
//            qDebug() << "PANIC: cursors not found menuDock";
//            trans = TRANS_1;
//        } else {
//            trans = TRANS_1;
//            return true;
//        }
    }
    return false;
}

bool AIControl::waitDockingMenuShow(int anMSec, bool &abCheck)
{
    static enum {TRANS_1, TRANS_2} trans ;
    switch (trans) {
    case TRANS_1:
        timeWaitMsec = anMSec;
        timeElapsed.restart();
        check = false;
        state = TRANS_MENU_DOCKING;
        trans = TRANS_2;
        break;
    case TRANS_2:
        trans = TRANS_1;
        abCheck = check;
        return  true;
    }
    return false;

}

bool AIControl::waitDockingMenuHide(int anMSec, bool &abCheck)          // Остановка после автостарта
{
    static enum {TRANS_1, TRANS_2} trans ;
    switch (trans) {
    case TRANS_1:
        check = false;
        timeElapsed.restart();
        searchImage = "stopAfterAutoStart";
        timeWaitMsec = anMSec;
        coeff = 0.87;
        nCount = 4;
        iStart = 2;
        iEnd = 7;
        state = SEARCH_IMAGE_CONTINUOUS;
        trans = TRANS_2;
        break;
    case TRANS_2:
        trans = TRANS_1;
        abCheck = check;
        return  true;
    }
    return false;
}

bool AIControl::waitMSec(int anMSec)
{
    static enum {TRANS_1, TRANS_2} trans ;
    switch (trans) {
    case TRANS_1:
        timeWaitMsec = anMSec;
        timeElapsed.restart();
        state = WAIT_MSEC;
        trans = TRANS_2;
        break;
    case TRANS_2:
        trans = TRANS_1;
        return  true;
    }
    return false;
}

void AIControl::setCurStation(QString asStation)
{
    asStation = asStation.simplified();
    asStation = asStation.toLower();
    asStation = asStation.replace(" ", "");
    m_sTarget = asStation;
}

