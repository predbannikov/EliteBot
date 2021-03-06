#ifndef SOCKETIO_H
#define SOCKETIO_H
#include <QTcpSocket>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QElapsedTimer>

class SocketIO
{
    QTcpSocket      *sock;
    QPoint          m_pointOffset;
    QElapsedTimer   m_timeElapse;
public:
    SocketIO(QPoint aPointOffset);
    void send(QByteArray a_data);
    void push_f_key(QString aStr);
    void push_key(QString aChar);
    void press_key(QString aChar);
    void release_key(QString aChar);
    void typingText(QString asText);
    void mouse_click(int typeSide);
    void mouse_press(int typeSide);
    void mouse_release(int typeSide);
    void mouse_wheel(int nCount);
    void mouse_move(int x, int y);
    void mouse_move_rel(int x, int y);
    void mouse_move_click(int x, int y);

};

#endif // SOCKETIO_H
