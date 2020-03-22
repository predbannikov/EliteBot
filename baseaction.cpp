/*
 *  QStringList &asListParam - содержит параметры во порядке изложенном ниже
 *  0 - имя метода
 *  1 - булевый результат выполнения
 *  2 - остальные параметры такие как имя QString и тп...
 *  ...
 */

#include "baseaction.h"

BaseAction::BaseAction(CaptureWindow *aCapture, SocketIO *aSock)
{
    m_sock = aSock;
    capture = aCapture;
    trans = TRANS_1;
}

BaseAction::~BaseAction()
{

}

bool BaseAction::perform(QStringList &asListParam)
{
    switch (trans) {
    case TRANS_1:
        timeElapsed.restart();
    [[clang::fallthrough]];
    case TRANS_2:
        qint64 elaps = timeElapsed.elapsed();
        if(elaps < timeWaitMsec) {
            if(logic(asListParam)) {
                asListParam[1] = "1";
                trans = TRANS_1;
                return true;
            }
        } else {
            trans = TRANS_1;
            asListParam[1] = "0";
            return true;
        }
        break;

    }
    return false;
}

void BaseAction::printDebug()
{
//    qDebug() << "debug text will be printed here";
}

void BaseAction::resetBase()
{
    trans = TRANS_1;
    reset();
}

void BaseAction::push_key(QString aChar)
{
    m_sock->push_key(aChar);
}

void BaseAction::press_key(QString aChar)
{
    m_sock->press_key(aChar);
}

void BaseAction::release_key(QString aChar)
{
    m_sock->release_key(aChar);
}

void BaseAction::typingText(QString asText)
{
    m_sock->typingText(asText);
}



//    bool LowLvlEngineScript::panel1CaseNav(QString sName, bool &abCheck, int anMSec)
//    {

//            timeWaitMsec = anMSec;
//            timeElapsed.restart();
//            check = false;
//            panel1CaseNavTrans = PANEL_1_CASE_NAV_TRANS_2;

//            qint64 elaps = timeElapsed.elapsed();
//            if(elaps < timeWaitMsec) {
//                CursorPanel *pan = capture->panel1Body();
//                if(pan->activeBody) {
//                    qDebug() << "RECOGNIZE: "<< pan->sBodyName << " сравниваем с" << sName;
//                    QStringList list = buttonLeftNav.filter(pan->sBodyName);
//                    if(list.isEmpty()) {
//                        int res = comparisonStr(pan->sBodyName, sName);
//                        if(res <= 2) {
//                            push_key(" ");
//                            abCheck = true;
//                            panel1CaseNavReset();
//                            return true;
//                        } else {
//                            push_key("s");
//                        }
//                    } else {
//                        push_key("d");
//                    }
//                }
//            } else {
//                abCheck = false;
//                panel1CaseNavReset();
//                return true;
//            }
//            break;


//bool BaseAction::proc()
//{
//        return proc();
//        CursorPanel *pan = capture->panel1Body();
//        if(pan->activeBody) {
//            qDebug() << "RECOGNIZE: "<< pan->sBodyName << " сравниваем с" << sName;
//            QStringList list = buttonLeftNav.filter(pan->sBodyName);
//            if(list.isEmpty()) {
//                int res = comparisonStr(pan->sBodyName, sName);
//                if(res <= 2) {
//                    push_key(" ");
//                    abCheck = true;
//                    panel1CaseNavReset();
//                    return true;
//                } else {
//                    push_key("s");
//                }
//            } else {
//                push_key("d");
//            }
//        }

