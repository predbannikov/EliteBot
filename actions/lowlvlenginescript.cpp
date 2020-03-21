#include "lowlvlenginescript.h"

#define     WAIT_MSEC_PUSH_KEY      35
#define     WAIT_MSEC_PRESS_KEY     30
#define     WAIT_MSEC_RELEASE_KEY   5
#define     WAIT_MSEC_MOUSE_EVENT   5

LowLvlEngineScript::LowLvlEngineScript()
{
    m_pointOffsetScreen = QPoint( m_screen.x(), m_screen.y());
}

LowLvlEngineScript::~LowLvlEngineScript()
{
    delete sock;
    delete capture;
}

void LowLvlEngineScript::push_key()
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "push_key";
    jMsg["code"] = sPush_key;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_PUSH_KEY);
}

void LowLvlEngineScript::push_key(QString aChar)
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "push_key";
    jMsg["code"] = aChar;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_PUSH_KEY);
}

void LowLvlEngineScript::press_key(QString aChar)
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "press_key";
    jMsg["code"] = aChar;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_PRESS_KEY);
}

void LowLvlEngineScript::release_key(QString aChar)
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "release_key";
    jMsg["code"] = aChar;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_RELEASE_KEY);
}

void LowLvlEngineScript::press_key()
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "press_key";
    jMsg["code"] = sPress_key;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_PRESS_KEY);
}

void LowLvlEngineScript::release_key()
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "release_key";
    jMsg["code"] = sRelease_key;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_RELEASE_KEY);
}

void LowLvlEngineScript::typingText()
{
    for(int i = 0; i < sTypingText.size(); i++) {
        QString aChar = sTypingText.at(i);
        QJsonObject jMsg;
        jMsg["target"] = "keyboard";
        jMsg["method"] = "push_key";
        jMsg["code"] = aChar;
        sendDataToSlave(QJsonDocument(jMsg).toJson());
        QThread::msleep(WAIT_MSEC_PRESS_KEY);
        QThread::msleep(WAIT_MSEC_RELEASE_KEY);

    }
}

void LowLvlEngineScript::move_mouse_rel(int x, int y)
{
    QJsonObject jMsg;
    jMsg["target"] = "mouse";
    jMsg["method"] = "move";
    jMsg["move_type"] = "REL";
    jMsg["x"] = x ;
    jMsg["y"] = y;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_MOUSE_EVENT);
}

void LowLvlEngineScript::mouse_move_click(cv::Point cvPoint)
{

    QJsonObject jMsg;
    jMsg["target"] = "mouse";
    jMsg["method"] = "move_click";
    jMsg["code"] = "BTN_LEFT";
    jMsg["x"] = cvPoint.x + m_pointOffsetScreen.x();
    jMsg["y"] = cvPoint.y + m_pointOffsetScreen.y();
    sendDataToSlave(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_MOUSE_EVENT);
}



void LowLvlEngineScript::sendDataToSlave(QByteArray a_data)
{
    sock->write(a_data);
    sock->waitForBytesWritten();
}
