#include "socketio.h"


#define     WAIT_MSEC_PUSH_KEY      70
#define     WAIT_MSEC_PRESS_KEY     35
#define     WAIT_MSEC_RELEASE_KEY   35
#define     WAIT_MSEC_MOUSE_EVENT   35
#define     WAIT_MSEC_MOUSE_CLICK   70

SocketIO::SocketIO(QPoint aPointOffset)
{
    sock = new QTcpSocket;
//    qDebug() << "sock create" << sock->thread()->currentThreadId();
    m_pointOffset = aPointOffset;
//    sock->connectToHost("192.168.16.128", 10101);
    sock->connectToHost("127.0.0.1", 10101);
}

void SocketIO::send(QByteArray a_data)
{
//    qDebug() << "socket id =" <<sock->thread()->currentThreadId();
    sock->write(a_data);
    sock->waitForBytesWritten();
}

void SocketIO::push_key(QString aChar)
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "push_key";
    jMsg["code"] = aChar;
    send(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_PUSH_KEY);
}

void SocketIO::press_key(QString aChar)
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "press_key";
    jMsg["code"] = aChar;
    send(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_PRESS_KEY);
}

void SocketIO::release_key(QString aChar)
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "release_key";
    jMsg["code"] = aChar;
    send(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_RELEASE_KEY);
}

void SocketIO::typingText(QString asText)
{
    for(int i = 0; i < asText.size(); i++) {
        QString aChar = asText.at(i);
        QJsonObject jMsg;
        jMsg["target"] = "keyboard";
        jMsg["method"] = "push_key";
        jMsg["code"] = aChar;
        send(QJsonDocument(jMsg).toJson());
        QThread::msleep(WAIT_MSEC_PUSH_KEY);
        QThread::msleep(100);
    }
}

void SocketIO::mouse_click()
{
    QJsonObject jMsg;
    jMsg["target"] = "mouse";
    jMsg["method"] = "click";
    jMsg["code"] = "BTN_LEFT";
    send(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_MOUSE_CLICK);
}

void SocketIO::mouse_move(int x, int y)
{
    QJsonObject jMsg;
    jMsg["target"] = "mouse";
    jMsg["method"] = "move";
    jMsg["x"] = x + m_pointOffset.x();
    jMsg["y"] = y + m_pointOffset.y();
    send(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_MOUSE_EVENT);
}

void SocketIO::mouse_move_rel(int x, int y)
{
    QJsonObject jMsg;
    jMsg["target"] = "mouse";
    jMsg["method"] = "move";
    jMsg["move_type"] = "REL";
    jMsg["x"] = x;
    jMsg["y"] = y;
    send(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_MOUSE_EVENT);
}

void SocketIO::mouse_move_click(int x, int y)
{

    QJsonObject jMsg;
    jMsg["target"] = "mouse";
    jMsg["method"] = "move_click";
    jMsg["code"] = "BTN_LEFT";
    jMsg["x"] = x + m_pointOffset.x();
    jMsg["y"] = y + m_pointOffset.y();
    send(QJsonDocument(jMsg).toJson());
    QThread::msleep(WAIT_MSEC_MOUSE_CLICK);
}
