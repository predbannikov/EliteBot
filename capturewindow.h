#ifndef CAPTUREWINDOW_H
#define CAPTUREWINDOW_H
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QDateTime>
#include <QThread>
#include <QRect>
#include <QJsonDocument>
#include <QElapsedTimer>
#include <QScreen>
#include "iodata.h"
#include "displays.h"
#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"
#ifdef __WIN32__
# include <winsock2.h>
#else

#endif
#include "main.h"


static QMultiMap <QString, QString> mapForDiffPanelNav;

extern QRect g_screen;


struct AreaCmp {
    AreaCmp(const std::vector<double>& _areas) : areas(&_areas) {}
    bool operator()(size_t a, size_t b) const {
        return (*areas)[a] > (*areas)[b];
    }
    const std::vector<double>* areas;
};

struct FactorCmp {
    FactorCmp(const std::vector<double>&_factor) : factor(&_factor) {}
    bool operator()(size_t a, size_t b) const {
        return (*factor)[a] > (*factor)[b];
    }
    const std::vector<double>* factor;
};

struct DigitCmp {
    DigitCmp(const std::vector<int>&_digit) : digit(&_digit) {}
    bool operator()(size_t a, size_t b) const {
        return (*digit)[a] > (*digit)[b];
    }
    const std::vector<int>* digit;
};


class CaptureWindow : public  QObject, Displays
{
    Q_OBJECT

    cv::Scalar cvRad = cv::Scalar(0, 0, 255);
    cv::Scalar cvGreen = cv::Scalar(0, 255, 0);
    cv::Scalar cvBlue = cv::Scalar(255, 0, 0);

    int m_nXOffset;
    int m_nYOffset;

    bool m_bDrawLine = false;
    int findImageIndex = 0;
    bool draw_match_rect = false;
    bool m_flowFrame= true;
    tesseract::TessBaseAPI *myOCREng;
    tesseract::TessBaseAPI *myOCRRus;
    tesseract::TessBaseAPI *myOCRRusDigits;
//    CvMemStorage *storage;

    std::map<std::string, ImageROI> *mp_dataSet;
    std::map <std::string, cv::Rect> m_rectMap;
//    IOData *m_ioData;

    QJsonArray m_jArrRecord;
    QStringList m_listRecord;
    QStringList m_fieldToRecord;
    cv::Point m_pointTemp;

    cv::Scalar minScalar;
    cv::Scalar maxScalar;
    int minNumber;
    int midNumber;
    int maxNumber;
    int m_side;
    int testError = 0;
    int testCounter = 0;
    QElapsedTimer timeElapse;
    QElapsedTimer timeElapseForVideoSave;
    QElapsedTimer timeElapseForVideoSave2;
//    int maxContours;
    CursorPanel m_cursorPan;
    CursorTarget m_cursorTarget;
    Compass m_compas;
    Distance distance;
    Primitives primitives;
//    std::vector< std::vector<cv::Point > > m_veclinesMenu;

//    void sendData();
//    void setListScript(QJsonArray t_jArray);
    //    void setListScript(QJsonObject t_jObj);
    void callBackMouse(int event, int x, int y, int flags);
    static void my_mouse_callback(int event, int x, int y, int flags, void *param);
    void loadJson();
public:
    CaptureWindow(std::map<std::string, ImageROI> *ap_dataSet, int x = 0, int y = 0, int width = 0, int heith = 0, QObject *parent = nullptr);
    ~CaptureWindow();
//    int nWaitKey;
    void update();
    cv::Mat *getSelectMatROI();
    cv::Rect *getNamedRect(QString asName);
    bool checkRectName(QString asName);
    void drawLine(cv::Point p1, cv::Point p2);


    bool srchAreaOnceInMat(std::string asImageROI, cv::Mat acvMat, double &coeff);
    cv::Point getPointAreaInMat(std::string asImageROI, cv::Mat acvMat, double factor = 0.99);
    cv::Point getPointOfPattern(cv::Mat acvMat, std::string sPattern, double factor = 0.25);
    cv::Point getPointAndFactorOfSPattern(cv::Mat acvMat, std::string sPattern, double &factor);
    bool srchAreaOnceInRect(std::string asImageROI, double &coeff, QPoint &aPoint, int anCount = 3, int anStart = 0, int anEnd = 8);
//    bool srchAreaOnceInRect(std::string as_rectWhichInLook, std::string as_imageROI);
    cv::Point getPointAfterLookAreaOnceInRect(std::string as_rectWhichInLook, std::string as_imageROI);
    cv::Point getPointAfterLookAreaInRect(std::string asImageROI, cv::Rect acvRect, double &factor);
    double getCoeffImageInRect(std::string asImageROI, cv::Rect acvRect);



    cv::Mat checkRoiMat();
    cv::Rect checkRoiRect();
    void showRoi();
    void drawRectangles();
    void drawLines();
    void drawDataSet();
//    void findImage(cv::Mat _mat);
    bool findPointRoi(cv::Mat &t_mat, cv::Mat &t_whereFind, cv::Point &t_point, double &aFactor);
    bool findRectRoi(cv::Mat t_mat, cv::Rect &t_rect, double t_factor = 0.97);
//    void setCaptureImage(QJsonObject jobj);
    cv::Point findMatchPoint(std::vector<VarForTemplateMatch> _vec, double &aFactor);


    // ----------------------- Целенапарвленные для игы функции ----------------------------

    bool containTextMat(cv::Rect aRect, cv::Point &aPoint, QString asTextSrch, QString asLang = "ru", int abApproxim = 0);     // abApproxim - использовать compareStr

    QString getStrStaticField(std::string asImageROI);
    QString getTextStaticField(std::string asImageROI, cv::Scalar aMinScalar, cv::Scalar aMaxScalar, QString asLang = "ru");
    QString getTextApproximArea(cv::Rect aRect, cv::Point &aPoint, QString asLang = "ru");
    QString getTextArea(cv::Rect aRect, cv::Point &aPoint, QString asLang = "ru");
    QString getTextMat(cv::Mat aMat, cv::Point &aPoint, QString asLang = "ru");
    QString getTextApproximBoundingRect(cv::Rect aRect, cv::Point &aPoint, QString asLang = "ru");
    bool containTextApproximBoundingRect(cv::Rect aRect, cv::Point &aPoint, QString asTextSrch, QString asLang = "ru", int abApproxim = 0);     // abApproxim - использовать compareStr
    bool containTextApproximBoundingMat(cv::Mat aMat, QString asTextSrch, QString asLang = "ru", int abApproxim = 0);     // abApproxim - использовать compareStr

    bool imageExpectedCloseAutoPilot(std::string asImageROI, double &coeff, int &ret, int anCount = 3, int anStart = 0, int anEnd = 8);
    Distance *recognizDistance();
    void testColor();



    //      Наводка
    CursorTarget *takeAimp();
    Primitives *getPrimitives(int aSide);
    Compass *compass();
    void testTarget2();
    void imageOverlay();

    cv::Mat getMaskPanel1(cv::Mat &aRectMat, int aMinVal3);
    CursorPanel *panel1Header(QString sName);
    CursorPanel *panel1Body(QString sName, QString asSystem);
    CursorPanel *panelBodyNav(QString aName, QString asSystem);
    CursorPanel *panelBodyCont();
    CursorPanel *panelBodyContNotice();

    std::vector<cv::Point> findPoints(std::vector< cv::Point>  contours, cv::Mat &dst);
    bool transformMenu1(cv::Mat &acvMatRet);
    bool transformSubNavMenu1(cv::Mat &acvMatRet);
    bool checkContour(std::vector< cv::Point > &acvVecPointCont, cv::Mat &cvMatOut);
    bool findLeftPoint(std::vector< std::vector< cv::Point > > &acvVecPointCont, cv::Mat &cvMatOut, cv::Point &acvpRight, cv::Point &acvpLeft);
    bool checkApproximRect(const cv::Rect acRectSrs, const cv::Rect acRectCurCMP, int anDelta);
    void getRectsInContour(std::vector< std::vector< cv::Point > > &acvVecPointCont, std::vector<cv::Rect> &vecRects);
    void getRectsOfPatternMenu1(std::vector< std::vector< cv::Point > > &acvVecPointCont, std::vector<cv::Rect> &vecRects, std::vector<cv::Rect> &vecRectsShort);
    void getRectsOfPatternMenu1SubNav(std::vector< std::vector< cv::Point > > &acvVecPointCont, std::vector<cv::Rect> &vecRects);
    bool getLeftAndUpInVecRects(const std::vector<cv::Rect> &vecRects, int &nXLeft, int &nYUp);
    cv::Rect getRectInContour(std::vector< cv::Point > &acvVecPoint);
    cv::Mat makeBinHeadMenu(cv::Mat &aMatWord, cv::Rect aRectCursor);
    cv::Mat parsBinLineNavList(cv::Mat &aMatList, cv::Rect aRectCursor);
//    void determinCursorHeader(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst);
//    void determinCursorBody(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst);
    std::vector<cv::Rect> getRectsBigInContours(cv::Mat &binMat);

    CursorPanel *subPanel1Nav();
    void transformSubMenu(cv::Mat &acvMat);
//    void determinCursorSubNavList(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst);

    // Меню посадки
    CursorPanel *menuDocking();

    cv::VideoWriter  writeVideo;
    cv::Mat win;
    cv::Mat m_srcWin;
    cv::Mat matroi;
    SaveROI m_saveRoi;
    Mouse mouse;
    StateApplication state;
//    std::vector<int> vec;
    int m_nCountCell;

    bool cycle = true;

    quint16     m_nNextBlockSize;

    // Получение данных из DataSet
    cv::Rect getRect(QString sName);
    QPoint getPoint(QString sName);


    // Вспомогательные функции
    bool resizeImage = false;
    void enableResizeImage();
    bool saveVideo = false;
    void enableSaveVideo();
    void freeze();
    bool blackLessWhite(cv::Mat &aBinMat, int &anWhite, int &anBlack);
    void setSide(int aSide);

    // Функции для подготовки матриц
    void getPrepMatsForMenu(cv::Mat &aColorMat, cv::Mat &aMaskMat);
    void getMaskOfMatHSV(cv::Mat &aColorMat, cv::Mat &aMaskMat, cv::Scalar aMinScalar =  cv::Scalar(10, 210, 230), cv::Scalar aMaxScalar = cv::Scalar(50, 255, 255));
    bool getMatsOfContour(cv::Mat &aColorMat, cv::Mat &aMaskMat, std::vector<cv::Point> contour);
    bool getContoursIndexSort(cv::Mat &aMaskMat, std::vector <std::vector <cv::Point> > &contours, std::vector<size_t> &idx, cv::RetrievalModes aAlgoType = cv::RETR_EXTERNAL);


public slots:

//    void sendDataToSlave(QJsonObject a_ObjData);

    void slotDrawMatchRect(Qt::CheckState t_state);
    void slotSetDrawLine(bool aCheck, int anCount);
    void setMinScalar(cv::Scalar acvMinScalar);
    void setMaxScalar(cv::Scalar acvMaxScalar);
    void setMinNumber(int i);
    void setMidNumber(int i);
    void setMaxNumber(int i);

//    void slotSetMaxContourForLength(int anMaxContour);
    cv::Rect calcRectFromPartOfIndex(int anCount = 3, int aiStart = 1, int aiEnd = 0);




public:    //debug function
    void addDrawRect(cv::Rect acvRect);
signals:
    void openGUI();
//    void sendKey(QChar aChar);
    void exitCapture();
    void signalSaveImageForDebug(cv::Mat acvMat, QString asName = "debug");

};

#endif // CAPTUREWINDOW_H
