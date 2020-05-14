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
    return capture->srchAreaOnceInRect(sSearchImage.toStdString(), coeff, point, nCount, iStart, iEnd);
}

bool BaseAction::perform(QStringList &asListParam)
{
    switch (transition) {
    case TRANSITION_1:
        transition = TRANSITION_2;
        sys_timeWaitMSec = 1000000;
        sys_timeElapsed.restart();
        sys_elapsedFunc.restart();
        sys_listCommand = asListParam;
        sys_debugLog = false;
        sys_resulLogic = false;
        timer.restart();
        confirmTimer.restart();
        trigger = false;
        m_bCompass = false;
        target = false;
        coeff = 0;
        diffCoef = 0.9;
        radius = 0;
        distance = 0;
        waitEnable = 0;
        waitMSec = 0;
        confirmTime = 0;
        whileWaitMSec = 0;
        nCount = 0;
        iStart = 0;
        iEnd = 0;
        lastElapse = 0;
        sSearchImage = "";
        sPanHeadName = "";
        sName = "";
        sTypingText = "";
        list.clear();
        listForSearch.clear();
        point = QPoint(0, 0);

        init(asListParam);
        if(m_sActionName != "DEBUG")
            qDebug() << "START " << m_sActionName;
    [[clang::fallthrough]];
    case TRANSITION_2:
        qint64 elaps = sys_timeElapsed.elapsed();
        if(elaps < sys_timeWaitMSec) {
            sys_resulLogic = logic(asListParam);
            if(sys_debugLog) {
                printDebug();
            }
            if(sys_resulLogic) {
                transition = TRANSITION_1;
                if(m_sActionName != "DEBUG")
                    qDebug() << "END   " << m_sActionName;
                return true;
            }
        } else {
            transition = TRANSITION_1;
            asListParam[1] = "1";
            return true;
        }
        break;

    }
    return false;
}

void BaseAction::printDebug()
{
    //    qDebug() << "elapse =" << sys_timeElapsed.elapsed();
}

bool BaseAction::expectedImage(QString asStr, double aDiffCoeff, int anCount, int anStart, int anEnd)
{
    double coefficient = 0;
    capture->srchAreaOnceInRect(asStr.toStdString(), coefficient, point, anCount, anStart, anEnd);
    qDebug() << QString::number(coefficient, 'f', 2 ) << QString::number(aDiffCoeff, 'f', 2 );
    if(coefficient > aDiffCoeff) {
        return true;
    } else {

        return false;
    }
}

//bool BaseAction::expectedImageClose(QString sStr)
//{
//    sSearchImage = sStr;
//    whileWaitMSec = asListParam[3].toUInt();
//    diffCoef = asListParam[4].toDouble();
////    coeff = asListParam[4].toDouble();
//    nCount = asListParam[5].toInt();
//    iStart = asListParam[6].toInt();
//    iEnd = asListParam[7].toInt();

//}

void BaseAction::resetBase()
{
    transition = TRANSITION_1;
    reset();
}

void BaseAction::push_f_key(QString aStr)
{
    qDebug() << "push_f_key(" << aStr << ")";
    m_sock->push_f_key(aStr);
}

void BaseAction::push_key(QString aChar)
{
    qDebug() << "push_key(" << aChar << ")";
    m_sock->push_key(aChar);
}

void BaseAction::press_key(QString aChar)
{
    qDebug() << "press_key(" << aChar << ")";
    m_sock->press_key(aChar);
}

void BaseAction::release_key(QString aChar)
{
    qDebug() << "release_key(" << aChar << ")";
    m_sock->release_key(aChar);
}

void BaseAction::typingText(QString asText)
{
    qDebug() << "typingText(" << asText << ")";
    m_sock->typingText(asText);
}

void BaseAction::mouse_move(int x, int y)
{
    qDebug() << "mouse_move(" << x << y << ")";
    m_sock->mouse_move(x, y);
}

void BaseAction::mouse_click(int anSideType)
{
    qDebug() << "mouse_click(" << anSideType << ")";
    m_sock->mouse_click(anSideType);
}

void BaseAction::mouse_press(int anSideType)
{
    qDebug() << "mouse_press(" << anSideType << ")";
    m_sock->mouse_press(anSideType);
}

void BaseAction::mouse_release(int anSideType)
{
    qDebug() << "mouse_release(" << anSideType << ")";
    m_sock->mouse_release(anSideType);
}

void BaseAction::mouse_wheel(int anCount)
{
    qDebug() << "mouse_wheel(" << anCount << ")";
    m_sock->mouse_wheel(anCount);
}

QString BaseAction::getName()
{
    return m_sActionName;
}

void BaseAction::mouse_move_rel(int x, int y)
{
    qDebug() << "mouse_move_rel(" << x << y << ")";
    m_sock->mouse_move_rel(x, y);
}

void BaseAction::mouse_move_click(int x, int y)
{
    qDebug() << "mouse_move_click(" << x << y << ")";
    m_sock->mouse_move_click(x, y);
}
