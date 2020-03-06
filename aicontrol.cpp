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
    m_slistStations << "Celebi City" << "Gabriel Enterprise";
    m_slistCommand << "transport";
    action = ENABLED_PANEL1;
}

AIControl::~AIControl()
{

}

void AIControl::next()
{
    switch (action) {
    case ENABLED_PANEL1:
        if(enabledPanel1("контакты"))
            action = REQ_DOCKING;

        break;
    case REQ_DOCKING:
        if(caseMenuContact("celebi city"))
            action = WAIT_DOCKING_MENU_SHOW;
        break;
    case WAIT_DOCKING_MENU_SHOW: {
            bool menuDock = false;
            if(waitDockingMenuShow(35000, menuDock)) {
                if(menuDock) {
                    action = MENU_DOCKING_AUTOSTART;
                } else {
                    action = ENABLED_PANEL1;
                }
            }
        }
        break;
    case MENU_DOCKING_AUTOSTART:
        if(caseMenuDocking("menu_docking_autostart")) {

            action = WAIT_DOCKING_MENU_HIDE;
        }
        break;
    case WAIT_DOCKING_MENU_HIDE: {
            bool menuDock = false;
            if(waitDockingMenuHide(40000, menuDock)) {
                if(menuDock) {
                    qDebug() << "Взлёт закончен";
                    action = ENABLED_PANEL1;
                } else {
                    action = WAIT;
                }
            }
        }
        break;
    case FIND_ITEM:
        if(caseStationMenuNav("celebi city")) {
            qDebug() << "Hit the target";
            action = ENABLED_HYPER;
        }
        break;
    case ENABLED_HYPER:
        if(caseSubMenuNav("enable_hypermode")) {
            qDebug() << "Hit the target enable_hypermode";
            action = WAIT;
        }
        break;
    case WAIT:
        qDebug() << "Hit the target wait";

        break;
    }
}

QString AIControl::getPathToTarget()
{
    return m_slistStations[0];
}

//void AIControl::where_iam()
//{
//    switch (stage) {
//    case STAGE_1:
//        push_key = "1";
//        state = PUSH_KEY;
//        stage = STAGE_2;
//        break;
//    case STAGE_2:               // Отправляем на поиск картинки
//        check = false;
//        timeElapsed.restart();
//        searchImage = "menu1_nav";
//        timeWaitMsec = 3000;
//        nCount = 4;
//        iStart = 8;
//        iEnd = 12;
//        stage = STAGE_3;
//        state = SEARCH_IMAGE_CONTINUOUS;
//        break;
//    case STAGE_3:               // Проверяем результат
//        if(check) {
//            qDebug() << searchImage << "this picture found";
//            timeElapsed.restart();
//            push_key = "q";
//            state = PUSH_KEY;
//            stage = STAGE_4;
//        } else {
//            qDebug() << "PANIC: this picture >" << searchImage << "not found";
//            action = WAIT;
//        }
//        break;
//    case STAGE_4:
//        check = false;
//        timeElapsed.restart();
//        searchImage = "menu1_contact";
//        timeWaitMsec = 3000;
//        nCount = 3;
//        iStart = 1;
//        iEnd = 1;
//        stage = STAGE_5;
//        state = SEARCH_IMAGE_CONTINUOUS;
//        break;
//    case STAGE_5:
//        stage = STAGE_6;
//        if(check) {
//            qDebug() << searchImage << "this picture found";
//            timeElapsed.restart();
//            listForSearch = m_slistStations;
//            state = WHICH_IMAGE_MORE_SIMILAR;
//            nCount = 5;
//            iStart = 7;
//            iEnd = 8;
//        } else {
//            qDebug() << "PANIC: this picture >" << searchImage << "not found";
//            action = WAIT;
//        }
//        break;
//    case STAGE_6:
//        stage = STAGE_1;
//        if(searchImage.isEmpty()) {
//            qDebug() << "name station not detected";
//            action = WAIT;
//        } else {
//            qDebug() << "Stateon:" << searchImage;
//            m_sStation = searchImage;
//            stage = STAGE_7;
//            timeElapsed.restart();
//            push_key = "e";
//            state = PUSH_KEY;
//        }
//        break;
//    case STAGE_7:
//        check = false;
//        timeElapsed.restart();
//        searchImage = "menu1_nav";
//        timeWaitMsec = 3000;
//        nCount = 4;
//        iStart = 8;
//        iEnd = 12;
//        stage = STAGE_8;
//        state = SEARCH_IMAGE_CONTINUOUS;
//        break;
//    case STAGE_8:
//        if(check) {
//            qDebug() << searchImage << "this picture found";
//            timeElapsed.restart();
//            action = GET_THE_GOAL;
//            stage = STAGE_1;
//        } else {
//            qDebug() << "PANIC: this picture >" << searchImage << "not found";
//            action = WAIT;
//        }
//        break;
//    }
//}

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
        trans = TRANS_2;
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_PANEL1;
        break;
    case TRANS_2:
        if(check) {
            int res = comparisonStr(cursorPanel->sHeaderName, sName);
            if(res <= 1) {
                qDebug() << "comparisonStr(cursorPanel->sHeaderName, sName)" << cursorPanel->sHeaderName << sName << res;
                trans = TRANS_1;
                return true;
            } else {
                push_key = "e";
                qDebug() << "push key 'e'";
            }
        } else {
            push_key = "1";
            qDebug() << "Enabled panel";
        }
        trans = TRANS_1;
        state = PUSH_KEY;
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
//        qDebug() << cursorPanel->sBodyName;
        if(check) {
            QStringList list = buttonLeftNav.filter(cursorPanel->sBodyName);
            if(list.isEmpty()) {
                int res = comparisonStr(cursorPanel->sBodyName, asStation);
                if(res <= 1) {
                    push_key = " ";
                    state = PUSH_KEY;
                    trans = TRANS_1;
                    return true;
                } else
                    push_key = "s";
            } else
                push_key = "d";
            trans = TRANS_1;
            state = PUSH_KEY;

        } else {
            qDebug() << "PANIC: cursors not found";
        }
        break;
    }
    return false;
}

bool AIControl::caseSubMenuNav(QString sNameSubMenu)
{
    static enum {TRANS_1, TRANS_2} trans ;
    switch (trans) {
    case TRANS_1:
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_SUB_CURSOR;
        trans = TRANS_2;
        break;
    case TRANS_2:
        qDebug() << cursorPanel->sBodyName;
        if(check) {
            int res = comparisonStr(cursorPanel->sSubNavName, sNameSubMenu);
            if(res <= 1) {
                push_key = " ";
                state = PUSH_KEY;
                trans = TRANS_1;
                return true;
            } else
                push_key = "d";
            trans = TRANS_1;
            state = PUSH_KEY;

        } else {
            qDebug() << "PANIC: cursors not found";
        }
        break;
    }
    return false;
}

bool AIControl::caseMenuContact(QString sNameBodyMenu)                          // Меню посадкиы
{
    static enum {TRANS_0, TRANS_1, TRANS_2, TRANS_3, TRANS_4, TRANS_5} trans ;
    switch (trans) {
    case TRANS_0:
        push_key = "x";
        state = PUSH_KEY;
        trans = TRANS_1;
        break;
    case TRANS_1:
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_BODY_CURSOR;
        trans = TRANS_2;
        break;
    case TRANS_2:
        qDebug() << cursorPanel->sBodyName;
        if(check) {
            int res = comparisonStr(cursorPanel->sBodyName, sNameBodyMenu);
            if(res <= 1) {
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
            if(res <= 1) {
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
            trans = TRANS_0;
            qDebug() << "PANIC: cursors caseMenuCont req_docking not found timeout" << sNameBodyMenu;
        }
        break;
    case TRANS_5:
        push_key = "1";
        state = PUSH_KEY;
        trans = TRANS_0;
        return true;
    }
    return false;
}

bool AIControl::caseMenuDocking(QString sNameDockMenu)                                  // Нажать кнопку автостарт
{
    static enum {TRANS_1, TRANS_2, TRANS_3, TRANS_4} trans ;
    switch (trans) {
    case TRANS_1:
        timeWaitMsec = 3000;
        timeElapsed.restart();
        check = false;
        state = TRANS_MENU_DOCKING;
        trans = TRANS_2;
        break;
    case TRANS_2:
//        qDebug() << cursorPanel->sBodyName;
        if(check) {
            int res = comparisonStr(cursorPanel->sNameMenuDocking, sNameDockMenu);
            if(res <= 1) {
                qDebug() << "push autostart";
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
            qDebug() << "PANIC: cursors not found menuDock";
        }
        break;
    case TRANS_3:
        timeWaitMsec = 10000;
        timeElapsed.restart();
        check = false;
        state = TRANS_MENU_DOCKING;
        break;
    case TRANS_4:
        if(check) {
            qDebug() << "PANIC: cursors not found menuDock";
            trans = TRANS_1;
        } else {
            trans = TRANS_1;
            return true;
        }
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

bool AIControl::waitDockingMenuHide(int anMSec, bool &abCheck)
{
    static enum {TRANS_1, TRANS_2} trans ;
    switch (trans) {
    case TRANS_1:
        check = false;
        timeElapsed.restart();
        searchImage = "stopAfterAutoStart";
        timeWaitMsec = anMSec;
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

