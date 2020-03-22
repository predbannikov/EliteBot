#ifndef ENGINESCRIPT_H
#define ENGINESCRIPT_H

#include <QObject>
#include <QJsonObject>
#include <QMessageBox>
#include <QThread>
#include <QEvent>
#include "capturewindow.h"
#include "global.h"
#include "iodata.h"
#include "aicontrol.h"
#include "lowlvlenginescript.h"





class EngineScript : public QObject, public LowLvlEngineScript
{
    Q_OBJECT

    int counter = 0;
    QJsonObject script;
    QPoint m_pointOffsetScreen;
public:
//    CaptureWindow   *capture;
    explicit EngineScript(IOData *apIOData, QObject *parent = nullptr);
    ~EngineScript();

    void readCommand(QStringList &aslistCommand);

    bool srchAreaOnceInRect(QString as_ImageROI, QString as_rectInWhichLook);
    cv::Point getPointAfterLookAreaOnceInRect(QString as_ImageROI, QString as_rectInWhichLook);
    cv::Point getPointAfterLookAreaInRect(QString asImageROI, int anCount = 3, int anStart = 0, int anEnd = 8);
    // Нумерация с 0
//    bool srchAreaOnceInPart(QString as_imageROI, int anXCount = 3, int anYCount = 3, int anXStart = 0, int anYStart = 0, int anXEnd = 2, int anYEnd = 2);
    bool srchAreaOnceInPart(QString as_imageROI, int anCount = 3, int anStart = 0, int anEnd = 8, double coeff = 0.9);
    cv::Rect calcRectFromPart(int anXCount = 3, int anYCount = 3, int anXStart = 0, int anYStart = 0, int anXEnd = 2, int anYEnd = 2);
    cv::Rect calcRectFromPartOfIndex(int anCount = 3, int aiStart = 1, int aiEnd = 0);
    QString getImageMoreSimilarInRect(QStringList alistsStations, int anCount = 3, int anStart = 0, int anEnd = 8);



    void engine();
    void initDisplay();


private:
    void push_key();
    void push_key(QString aChar);
    void press_key(QString aChar);
    void release_key(QString aChar);
    void press_key();
    void release_key();
    void typingText();
    void move_mouse_rel(int x, int y);
    void mouse_move_click(cv::Point cvPoint);


    QTcpSocket      *sock;
    AIControl       *m_pControl;
//    int             m_index = 0;
    QElapsedTimer   timeElapsed;
    QElapsedTimer   timeElapsedForFunc;

    void sendDataToSlave(QByteArray a_data);


public slots:
//    void performScript(QJsonArray t_jArray);
//    void parsePushKey(QChar ac_key);
    void run();
    void slotSaveImage(cv::Mat acvMat, QString asName);
    void slotEngineEnable(bool aState);
    void slotSetCurStation(QString asStation);
    cv::Mat slotCheckRoiMat ();
    cv::Rect slotCheckRoiRect ();
    void slotSetDrawLine(bool abCheck, int anCount);
    void setMinScalar(int n1, int n2, int n3);
    void setMaxScalar(int n1, int n2, int n3);
    void setMinNumber(int aNumber);
    void setMidNumber(int aNumber);
    void setMaxNumber(int aNumber);
signals:
    void signalOpenGui();
};


class key_filter : public QObject
{
    Q_OBJECT
public:
    key_filter(QObject *parent = nullptr) : QObject(parent) {}
    bool eventFilter(QObject *obj, QEvent *event) override {
        if(event->type() == QEvent::KeyPress) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(event);
            if(ke->key() == Qt::Key_A) {
//                QKeyEvent * eve1 = new QKeyEvent (QEvent::KeyPress,Qt::Key_Left,Qt::NoModifier);
//                QKeyEvent * eve2 = new QKeyEvent (QEvent::KeyRelease,Qt::Key_Left,Qt::NoModifier);


//                qDebug() << "press arrow key left";
            }
        }
        return QObject::eventFilter(obj, event);
    }
protected:


};




#endif // ENGINESCRIPT_H
