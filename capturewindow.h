#ifndef CAPTUREWINDOW_H
#define CAPTUREWINDOW_H
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QThread>
#include <QJsonDocument>
#include <QElapsedTimer>
#include <QScreen>
#include "guiinfo.h"
#include "iodata.h"
#include "displays.h"
#include "parsingscript.h"
#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"
#ifdef __WIN32__
# include <winsock2.h>
#else

#endif
#include "main.h"

//typedef struct {
//    bool toggle = false;
//    QJsonArray jArray;
//    int index;
//    QList<QJsonObject> list;

//} StackFunk;




class CaptureWindow : public  QObject, Displays
{
    Q_OBJECT
    int m_nXOffset;
    int m_nYOffset;

    bool m_bDrawLine = false;
    int findImageIndex = 0;
    bool loop = false;
    bool draw_match_rect = false;
    bool m_flowFrame= true;
    tesseract::TessBaseAPI *myOCREng;
    tesseract::TessBaseAPI *myOCRRus;
//    CvMemStorage *storage;

    std::map<std::string, ImageROI> *mp_dataSet;
    std::map <std::string, cv::Rect> m_rectMap;
//    IOData *m_ioData;

    QJsonArray m_jArrRecord;
    QStringList m_listRecord;
    QStringList m_fieldToRecord;
    cv::Point m_pointTemp;
    StackFunk m_stack;

    cv::Scalar minScalar;
    cv::Scalar maxScalar;
    int maxContours;
    CursorPanel m_cursorPan;
//    std::vector< std::vector<cv::Point > > m_veclinesMenu;

//    void sendData();
//    void setListScript(QJsonArray t_jArray);
    //    void setListScript(QJsonObject t_jObj);
    void callBackMouse(int event, int x, int y, int flags);
static void my_mouse_callback(int event, int x, int y, int flags, void *param);
public:
    CaptureWindow(std::map<std::string, ImageROI> *ap_dataSet, int x = 0, int y = 0, int width = 0, int heith = 0, QObject *parent = nullptr);
    ~CaptureWindow();
    void update();
    cv::Mat *getSelectMatROI();
    cv::Rect *getNamedRect(QString asName);
    bool checkRectName(QString asName);


    bool srchAreaOnceInMat(std::string as_imageROI, cv::Mat acvMat);
    cv::Point getPointAreaInMat(std::string asImageROI, cv::Mat acvMat);
    bool srchAreaOnceInRect(std::string as_imageROI, cv::Rect acvRect);
    bool srchAreaOnceInRect(std::string as_rectWhichInLook, std::string as_imageROI);
    cv::Point getPointAfterLookAreaOnceInRect(std::string as_rectWhichInLook, std::string as_imageROI);
    cv::Point getPointAfterLookAreaInRect(std::string asImageROI, cv::Rect acvRect);
    double getCoeffImageInRect(std::string asImageROI, cv::Rect acvRect);



    void showRoi();
    void drawRectangles();
    void drawLines();
    void drawDataSet();
    void findImage(cv::Mat _mat);
    bool findPointRoi(cv::Mat &t_mat, cv::Mat &t_whereFind, cv::Point &t_point, double t_factor = 0.97);
    bool findRectRoi(cv::Mat t_mat, cv::Rect &t_rect, double t_factor = 0.97);
    void setCaptureImage(QJsonObject jobj);
    cv::Point findMatchPoint(std::vector<VarForTemplateMatch> _vec, double t_factor = 0.97);






    // Панель 1
    CursorPanel *panel1();
    void transformMenu(cv::Mat &acvMat);
    bool checkContour(std::vector< cv::Point > &acvVecPointCont, cv::Mat &cvMatOut);
    bool findLeftPoint(std::vector< std::vector< cv::Point > > &acvVecPointCont, cv::Mat &cvMatOut, cv::Point &acvpRight, cv::Point &acvpLeft);
    bool checkApproximRect(const cv::Rect acRectSrs, const cv::Rect acRectCurCMP, int anDelta);
    void getRectsInContour(std::vector< std::vector< cv::Point > > &acvVecPointCont, std::vector<cv::Rect> &vecRects);
    cv::Rect getRectInContour(std::vector< cv::Point > &acvVecPoint);
    cv::Mat makeBinHeadMenu(cv::Mat &aMatWord, cv::Rect aRectCursor);
    cv::Mat parsBinLineNavList(cv::Mat &aMatList, cv::Rect aRectCursor);
    void determinCursorHeader(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst);
    void determinCursorBody(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst);
    std::vector<cv::Rect> getRectsBigInContours(cv::Mat &binMat);

    CursorPanel *subPanel1Nav();
    void transformSubMenu(cv::Mat &acvMat);
    void determinCursorSubNavList(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst);

    // Меню посадки
    CursorPanel *menuDocking();

    cv::Mat win;
    cv::Mat m_srcWin;
    cv::Mat matroi;
//    cv::Mat temp;
    Mouse mouse;
    StateApplication state;
//    std::vector<int> vec;
    bool test = false;
    int m_nCountCell;

    bool cycle = true;

    quint16     m_nNextBlockSize;
public slots:

//    void sendDataToSlave(QJsonObject a_ObjData);

    void slotDrawMatchRect(Qt::CheckState t_state);
    void slotSetLoop(Qt::CheckState t_state);
    cv::Mat *slotCheckRoiMat();
    cv::Rect *slotCheckRoiRect();
    void slotSetDrawLine(bool aCheck, int anCount);
    void setMinScalar(cv::Scalar acvMinScalar);
    void setMaxScalar(cv::Scalar acvMaxScalar);
    void slotSetMaxContourForLength(int anMaxContour);




public:    //debug function
    void addDrawRect(cv::Rect acvRect);
signals:
    void openGUI();
    void exitCapture();
    void signalSaveImageForDebug(cv::Mat acvMat, QString asName = "debug");

};

#endif // CAPTUREWINDOW_H
