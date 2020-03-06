#ifndef ENGINESCRIPT_H
#define ENGINESCRIPT_H

#include <QObject>
#include <QJsonObject>
#include "capturewindow.h"
#include "guiinfo.h"
#include "global.h"
#include "iodata.h"
#include "aicontrol.h"




class EngineScript : public QObject
{
    Q_OBJECT

    int counter = 0;
    QJsonObject script;
    QPoint m_pointOffsetScreen;
public:
    IOData *mp_ioData;
    explicit EngineScript(QObject *parent = nullptr);
    void setScript(QJsonObject script);
    QJsonObject readAction();
    bool srchAreaOnceInRect(QString as_ImageROI, QString as_rectInWhichLook);
    cv::Point getPointAfterLookAreaOnceInRect(QString as_ImageROI, QString as_rectInWhichLook);
    cv::Point getPointAfterLookAreaInRect(QString asImageROI, int anCount = 3, int anStart = 0, int anEnd = 8);
    // Нумерация с 0
//    bool srchAreaOnceInPart(QString as_imageROI, int anXCount = 3, int anYCount = 3, int anXStart = 0, int anYStart = 0, int anXEnd = 2, int anYEnd = 2);
    bool srchAreaOnceInPart(QString as_imageROI, int anCount = 3, int anStart = 0, int anEnd = 8);
    cv::Rect calcRectFromPart(int anXCount = 3, int anYCount = 3, int anXStart = 0, int anYStart = 0, int anXEnd = 2, int anYEnd = 2);
    cv::Rect calcRectFromPartOfIndex(int anCount = 3, int aiStart = 1, int aiEnd = 0);
    QString getImageMoreSimilarInRect(QStringList alistsStations, int anCount = 3, int anStart = 0, int anEnd = 8);



    void engine();

private:
    std::map<std::string, ImageROI> *mp_dataSet;
    QList<QJsonObject> m_listScript;
    void push_key();
    void typingText();
    void mouse_move_click(cv::Point cvPoint);


    QTcpSocket      *sock;
    CaptureWindow   *capture;
    GuiInfo         *ginfo;
    AIControl       *m_pControl;
    bool            cycle = true;
//    int             m_index = 0;
    QElapsedTimer   timeElapsed;
    QElapsedTimer   timeElapsedForFunc;

    void sendDataToSlave(QByteArray a_data);

    bool menuDocking();

//    void nextScript();
//    void setScript(QString as_scriptName);
//    void setListScript();
//    void setListScript(QJsonArray t_jArray);
//    void setListScript(QJsonObject t_jObj);
public slots:
//    void performScript(QJsonArray t_jArray);
//    void parsePushKey(QChar ac_key);
    void update();
    void openGUI();
    void exitEngine();
    void slotSaveImage(cv::Mat acvMat, QString asName);

signals:

};

#endif // ENGINESCRIPT_H
