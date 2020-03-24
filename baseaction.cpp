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
    transition = TRANSITION_1;
}

BaseAction::~BaseAction()
{

}

bool BaseAction::srchAreaOnceInPart()
{
    return capture->srchAreaOnceInRect(sSearchImage.toStdString(), nCount, iStart, iEnd, coeff);
}

bool BaseAction::perform(QStringList &asListParam)
{
    switch (transition) {
    case TRANSITION_1:
        init(asListParam);
        sys_timeWaitMSec = 300000;
        sys_timeElapsed.restart();
        timer.restart();
        confirmTimer.restart();
        trigger = false;
        transition = TRANSITION_2;
        if(m_sActionName != "DEBUG")
            qDebug() << "START " << m_sActionName;
    [[clang::fallthrough]];
    case TRANSITION_2:
        qint64 elaps = sys_timeElapsed.elapsed();
        if(elaps < sys_timeWaitMSec) {
            sys_resulLogic = logic(asListParam);
            printDebug();
            if(sys_resulLogic) {
                asListParam[1] = "1";
                transition = TRANSITION_1;
                if(m_sActionName != "DEBUG")
                    qDebug() << "END   " << m_sActionName;
                return true;
            }
        } else {
            transition = TRANSITION_1;
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
    transition = TRANSITION_1;
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

QString BaseAction::getName()
{
    return m_sActionName;
}

void BaseAction::move_mouse_rel(int x, int y)
{
    m_sock->move_mouse_rel(x, y);
}

void BaseAction::mouse_move_click(int x, int y)
{
    m_sock->mouse_move_click(x, y);
}
