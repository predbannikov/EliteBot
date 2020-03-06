#ifndef PARSINGSCRIPT_H
#define PARSINGSCRIPT_H

#include <QThread>

#include "global.h"


typedef struct {
    bool toggle = false;
    QJsonArray jArray;
    int index;
    QList<QJsonObject> list;

} StackFunk;


class ParsingScript : QObject
{
    Q_OBJECT

public:
    ParsingScript(QObject *parent = nullptr);
    ~ParsingScript();
    void parse();
//    void setScript(QString a_sScriptName);
//    int findImageIndex = 0;
//    int m_index = 0;
//    bool loop = false;
//    bool draw_match_rect = false;
//    bool m_flowFrame= true;
//    std::map<std::string, Enginer> *m_dataSet;
//    std::map <std::string, cv::Rect> m_rectMap;
////    IOData *m_ioData;
//    QJsonArray *m_arrayScript;
//    QJsonArray m_jArrRecord;
//    QStringList m_listRecord;
//    QStringList m_fieldToRecord;
//    QList<QJsonObject> m_listScript;
//    cv::Point m_pointTemp;
//    StackFunk m_stack;
//    void sendData();
//    void parse();
//    void parseScript();
//    void setListScript(QJsonArray t_jArray);
//    void setListScript(QJsonObject t_jObj);
//    void nextScript();
//    void initFieldToRecord();
//public:
//    void update();
//    void callBackMouse(int event, int x, int y, int flags);
//    void addTrackBar(int pos);
//    void callbackButton(int iState);




//    void showRoi();
//    void drawRectangles();
//    void drawDataSet();
//    void findImage(cv::Mat _mat);
//    bool findPointRoi(cv::Mat &t_mat, cv::Mat &t_whereFind, cv::Point &t_point, double t_factor = 0.97);
//    bool findRectRoi(cv::Mat t_mat, cv::Rect &t_rect, double t_factor = 0.97);
//    void setCaptureImage(QJsonObject jobj);
//    cv::Point findMatchPoint(std::vector<VarForTemplateMatch> _vec, double t_factor = 0.97);
//    QElapsedTimer timeElapsed;
//    QElapsedTimer timeElapsedFive;
//    cv::Mat win;
//    cv::Mat m_srcWin;
//    cv::Mat matroi;
////    cv::Mat temp;
//    Mouse mouse;
//    StateApplication state;
////    std::vector<int> vec;
//    bool test = false;

//    bool cycle = true;

//    quint16     m_nNextBlockSize;
//public slots:

//    void slotDrawMatchRect(Qt::CheckState t_state);
//    void slotSetLoop(Qt::CheckState t_state);
//    void performScript(QJsonArray t_jArray);
//    cv::Mat *slotCheckRoiMat();
//    cv::Rect *slotCheckRoiRect();

//signals:
//    void sendDataToSlave(QJsonObject);
};

#endif // PARSINGSCRIPT_H
