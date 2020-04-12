#ifndef GLOBAL_H
#define GLOBAL_H
#include <QJsonObject>
#include <QJsonArray>
#include <QElapsedTimer>
#include <QDebug>
#include <QList>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <map>
#include "main.h"

#define     PATH_TO_TESSERACT   ".\\tesseract4.1\\tessdata"

#define     M_PI                3.1415926535
#define     PATH_CONFIG         ".\\config"
#define     PATH_LOG            ".\\log"
#define     PATH_VIDEO_LOG      ".\\videolog"
#define     PATH_CONFIG_DISPLAY ".\\config\\display.json"

#define     DEBUG1               0
#define     DEBUG2               0



enum SYS_STATE{ DEBUG_STATE, RESTOR_GAME, AICONTROL, PUSH_KEY, PRESS_KEY, RELEASE_KEY, SEARCH_IMAGE_CONTINUOUS, WHILE_IMAGE_CONTINUOUS, CLICK_POINT_IMAGE_AFTER_LOOK,
                TRANS_MENU_DOCKING,
                TYPING_TEXT, TRANS_PANEL1, TRANS_BODY_CURSOR, TRANS_SUB_CURSOR,
                WAIT_MSEC,
              CLICK_TO_POINT,
              AIMING, COMPASS,
                APPROACH,
              SET_FRAME_FREQ};



class StateApplication {
public:
    bool roi = false;
};

extern bool state_debug;

extern QRect  g_screen;


const QStringList buttonLeftNav = {"push_filter", "close_push_filter", "push_galaxy_map", "push_system_map"};
static const QMap<int, QString> sNameMenu1Head = { {0, "навигация"}, {1, "транзакции"}, {2, "контакты"} };
static const QMap<int, QString> sNamePicNavList = { {0, "muneLineNavListStar"}, {1, "muneLineNavListStat"}, {2, "muneLineNavListMay"},
                                                    {3,"muneLineNavListGas"}, {4, "muneLineNavListSys"} };
static const QMap<int, QString> sNamePicNavListTar = { {0, "muneLineNavListTar"}, {1, "muneLineNavListCur"} };

static const QMap<int, QString> subNamePicMenu1NavList = { {0, "subMenuNavFix"}, {1, "subMenuNavUnFix"}, {2, "subMenuNavGipmod"},
                                                           {3, "subMenuNavGipmodHelp"}, {4, "subMuneLineNavListSys"}};
struct SaveROI {
    bool active = false;
    cv::Mat dst;
    cv::Rect rect;
};

struct Compass {
    cv::Point pDot;
    cv::Point pCenter;
    cv::Size size;
    bool face = 0;
    bool active = false;
};

struct Distance {
    QList<QString> distanceList;
    double distance;
};

struct CursorPanel {
    cv::Rect rectHeader;
    cv::Rect rectBody;
    cv::Rect rectSubNavList;
    cv::Rect rectMenuDocking;
//    int numberSubNavList;
    bool activeMenuDocking = false;
    bool activeSubNav = false;
    bool activeBody = false;
    bool activeHeader = false;
    QString sHeaderName;
    QString sBodyName;
    QString sBodyNameNotice;
    QString sSubNavName;
    QString sMenuDocking;

};

struct CursorTarget {
    cv::Rect rectSrc;
    cv::Rect rectTarget;
    cv::Point pointCenter;
    cv::Point pointTarget;
    bool    active = false;
};

struct Primitives {
    std::vector< cv::Point > contour;
    cv::Rect rect;
    cv::Rect screen;
    double area;
    bool found;
};


typedef struct {
    cv::Rect rect;
    cv::Mat mat;
    std::string name;
    bool active = false;
} ImageROI;

class Mouse {
public:
    Mouse() {}
    ~Mouse() {}
    int getX() { return x1 > x2 ? x2 : x1; }
    int getY() { return y1 > y2 ? y2 : y1; }
    int getWidth()  { return x1 <= x2 ? x2 - x1: x1 - x2; }
    int getHeight() { return y1 <= y2 ? y2 - y1: y1 - y2; }
    void setX1(int x) { if(x<0) x = 0; x1 = x; x2 = x;}
    void setY1(int y) { if(y<0) y = 0; y1 = y; y2 = y;}
    void setX2(int x) { if(x<0) x = 0; x2 = x;}
    void setY2(int y) { if(y<0) y = 0; y2 = y;}
    void setDeltaX(int x) {x_new = x;}
    void setDeltaY(int y) {y_new = y;}
    void calcDeltaX(int _x) {
        int delta = _x - x_new;
        setX1(lastRect.x + delta);
        setX2(lastRect.width + delta + lastRect.x);
    }
    void calcDeltaY(int _y) {
        int delta = _y - y_new;
        setY1(lastRect.y + delta);
        setY2(lastRect.height + delta + lastRect.y);

    }
    cv::Rect getRect() {
        int x = getX();
        int y = getY();
        int width = getWidth();
        int height = getHeight();


        if(width+x > g_screen.width() )
            width = g_screen.width() - x;
        if(width <= 0) {
            x = g_screen.width() - 1;
            width = 1;
        }
        if(height+y > g_screen.height())
            height = g_screen.height() - y;
        if(height <= 0) {
            y = g_screen.height() - 1;
            height = 1;
        }

        return cv::Rect(x, y, width, height);
    }
    cv::Rect getAndSaveRect() {
        lastRect = getRect();
        return lastRect;
    }
    cv::Point getPointA() { return cv::Point(x1, y1);}
    cv::Point getPointB() { return cv::Point(x2, y2);}


    bool leftButton = false;
    bool rightButton = false;
    bool middleButton = false;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int x_new = 0;
    int y_new = 0;
    int flag = 0;
    cv::Rect lastRect;
};

typedef struct {
    cv::Point p1;
    cv::Point p2;
    double m1 = 0.0;
    double m2 = 0.0;
    int matchP1 = 0;
    int matchP2 = 0;
    int method;
//    bool operator == (const TIME & timeB) const {
//       return this->min == timeB.min && this->sec == timeB.sec;
//    }
} VarForTemplateMatch;

inline bool isPointInsideElypse(cv::Vec3f aCircl, cv::Point aPoint) {       // Если внутри то истина
    if(pow(aPoint.x - aCircl[0], 2) +
            pow(aPoint.y - aCircl[1], 2) <=
            pow(aCircl[2], 2))
        return true;
    else
        return false;
}


template<typename P> struct CmpP1
{
    bool operator()(const P &p1, const P &p2) {
        if(p1.matchP1 > p2.matchP1) return true;
        else return false;
    }
};
template<typename P> struct CmpP2
{
    bool operator()(const P &p1, const P &p2) {
        if(p1.matchP2 > p2.matchP2) return true;
        else return false;
    }
};

inline void deleteCharExtra(QString &aStr) {
    for(int i = 0; i < aStr.size(); i++) {
        if(aStr[i] == '[')
            aStr[i] = ' ';
        else if(aStr[i] == ']')
            aStr[i] = ' ';
        else if(aStr[i] == '_')
            aStr[i] = ' ';
        else if(aStr[i] == '{')
            aStr[i] = ' ';
        else if(aStr[i] == '}')
            aStr[i] = ' ';
        else if(aStr[i] == '(')
            aStr[i] = ' ';
        else if(aStr[i] == ')')
            aStr[i] = ' ';
        else if(aStr[i] == ':')
            aStr[i] = ' ';
        else if(aStr[i] == ';')
            aStr[i] = ' ';
        else if(aStr[i] == '&')
            aStr[i] = ' ';
        else if(aStr[i] == '@')
            aStr[i] = ' ';
        else if(aStr[i] == '|')
            aStr[i] = ' ';
        else if(aStr[i] == '\'')
            aStr[i] = ' ';
        else if(aStr[i] == '<')
            aStr[i] = ' ';
        else if(aStr[i] == '>')
            aStr[i] = ' ';

    }
    aStr.replace(" ", "");
}

inline int comparisonStr(QString asSrc, QString asCurStr)
{
    qDebug() << "Сравнивание " << asSrc << " и " << asCurStr;
    asSrc = asSrc.toLower();
    asCurStr = asCurStr.toLower();
    if(asSrc.indexOf("<") != -1) {
        asSrc.remove(0, asSrc.indexOf("<") + 1);
    }
    if( asSrc.indexOf(">") != -1) {
        asSrc.remove(asSrc.indexOf(">"), asSrc.size() );
    }
    asSrc.replace(" ", "");
    asCurStr.replace(" ", "");
#define     COUNT_CHAR_SLOG         2
    int count = asCurStr.size() / COUNT_CHAR_SLOG;
    QString modulo = asCurStr;
    int count_error = 0;
    for(int i = 0; i < count; i++) {
        QString str;
        if(i == count - 1) {
            str = modulo;
        } else {
            str = modulo.left(COUNT_CHAR_SLOG);
            modulo.remove(0, COUNT_CHAR_SLOG);
        }
        if(!asSrc.contains(str)) {
            count_error++;
        }
    }
    count_error += abs(asSrc.size() - asCurStr.size());
    return count_error;
}

static void printPoint(cv::Point _point)
{
    qDebug() << "POINT:" << "x=" << _point.x << " y=" << _point.y;
}

static void printRect(cv::Rect _rect)
{
    qDebug() << "RECT" << "x=" << _rect.x
              << " y=" << _rect.y
              << " width=" << _rect.width
              << " heigth=" << _rect.height;
}
static void printMat(cv::Mat _mat)
{
    qDebug() << "MAT" << "countRowInBytes=" << _mat.step
              << " chanals=" << _mat.channels()
              << " width=" << _mat.cols
              << " heigth=" << _mat.rows;
}

/***********************************************************************
 *                          Helper class                               *
 * *********************************************************************/




class Helper {

public:

    static void printInfoMat(cv::Mat _mat)
    {
        std::cout << "rowInBytes=" << _mat.step
                  << " chanals=" << _mat.channels()
                  << " width=" << _mat.cols
                  << " heigth=" << _mat.rows
                  << std::endl;
    }
    static void printRect(cv::Rect _rect)
    {
        std::cout << "x=" << _rect.x
                  << " y=" << _rect.y
                  << " width=" << _rect.width
                  << " heigth=" << _rect.height
                  << std::endl;
    }

    static void printMapPoint(std::vector<VarForTemplateMatch> _vec)
    {
        for(size_t i = 0; i < _vec.size(); i++) {
            std::cout << " min=" << std::setw(15) << std::fixed  << std::setprecision(3 )<<  _vec[i].m1
                      << " max=" << std::setw(15) << std::fixed << std::setprecision(3) << _vec[i].m2
                      << std::setw(16) << " p_min=" << _vec[i].p1.x << ":" << _vec[i].p1.y
                      << std::setw(16) << " p_max=" << _vec[i].p2.x << ":" << _vec[i].p2.y
                      << std::setw(15) << " matchP1=" << _vec[i].matchP1
                      << std::setw(15) << " matchP2=" << _vec[i].matchP2 << std::endl;
        }

    }

};



/***********************************************************************
 *                                                       *
 * *********************************************************************/



// *************************************************
const int count_match_methods = 6;

#endif // GLOBAL_H
