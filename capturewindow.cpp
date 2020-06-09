#include "capturewindow.h"
#include "algorithm"
#include "qmath.h"




#define     MIN_COEFF_FOR_FIND              0.99
#define     MIN_COEFF_FOR_FIND_POINT        0.7


#define     CURSOR_DELTA        10


bool state_debug;


void CaptureWindow::my_mouse_callback(int event, int x, int y, int flags, void *param) {
    CaptureWindow *capture = reinterpret_cast<CaptureWindow*>(param);
    capture->callBackMouse(event, x, y, flags);
}


CaptureWindow::CaptureWindow(std::map<std::string, ImageROI> *ap_dataSet, int x, int y, int width, int heith, QObject *parent) :
    QObject (parent),
    Displays(x, y, width, heith),
    m_nXOffset(x),
    m_nYOffset(y),
    minScalar(cv::Scalar(0, 100, 100)),
    maxScalar(cv::Scalar(255, 255, 0)),
    minNumber(1),
    midNumber(3),
    maxNumber(255)
{
    mp_dataSet = ap_dataSet;


//    cv::namedWindow("win1", cv::WND_PROP_FULLSCREEN);
    cv::namedWindow("win1", cv::WND_PROP_AUTOSIZE);
    cv::moveWindow("win1", 0, -STANDART_FULLHD_HEIGHT);

//    cv::setWindowProperty("win1", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    cv::setMouseCallback("win1", my_mouse_callback, this);
    cv::namedWindow("win2", cv::WND_PROP_AUTOSIZE );
    cv::namedWindow("win3", cv::WND_PROP_AUTOSIZE );
    cv::namedWindow("win4", cv::WND_PROP_AUTOSIZE );
    cv::namedWindow("win5", cv::WND_PROP_AUTOSIZE );
    cv::moveWindow("win2", STANDART_FULLHD_WIDTH, 0);
    cv::moveWindow("win3", STANDART_FULLHD_WIDTH + 360, 0);
    cv::moveWindow("win4", STANDART_FULLHD_WIDTH, 920 / 2 + 130);
    cv::moveWindow("win5", STANDART_FULLHD_WIDTH + 360, 920 / 2 + 130);


//    cv::VideoCapture inputVideo(PATH_LOG"\\f.avi");              // Open input
//    cv::VideoWriter  writeVideo;


//    storage = cvCreateMemStorage(0);

    timeElapse.start();

//    cv::Mat _mat = cv::imread("C:\\Users\\user\\AppData\\Local\\autoclicker\\elite\\Gabriel Enterprise.png");
//    cv::Mat _mat2 = cv::imread("C:\\Users\\user\\AppData\\Local\\autoclicker\\elite\\menu1_contact.png");
//    //cv::imshow("win2tmp", _mat);
    myOCREng = new tesseract::TessBaseAPI();
    myOCRRus = new tesseract::TessBaseAPI();
    myOCRRusDigits = new tesseract::TessBaseAPI();
    myOCRRusDigits->SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_LINE);
    if(!myOCRRusDigits->SetVariable("tessedit_char_whitelist",".,1234567890КМ"))
        qDebug() << "Error setVariable";
//    printf("Leptonica version: %s\n", getLeptonicaVersion());


    if (myOCREng->Init(PATH_TO_TESSERACT, "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
    }
    if (myOCRRus->Init(PATH_TO_TESSERACT, "rus")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
    }
    if (myOCRRusDigits->Init(PATH_TO_TESSERACT, "rus")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
    }
    myOCREng->SetVariable("debug_file", "/dev/null");
    myOCRRus->SetVariable("debug_file", "/dev/null");
    myOCRRusDigits->SetVariable("debug_file", "/dev/null");


//    qDebug() << myOCREng->GetUTF8Text() << myOCRRus->GetUTF8Text();
    m_cursorPan.sHeaderName ="навигация";
    enableResizeImage();
//    enableSaveVideo();
    cv::moveWindow("win1", 0, -STANDART_FULLHD_HEIGHT);
//    resizeImage = true;
    m_side = 0;
//    m_cursorPan.rectBody = cv::Rect(796, 424, 414, 77);
//    m_cursorPan.activeBody = true;
    loadJson();
}

CaptureWindow::~CaptureWindow()
{
//    qDebug() << "begin Capture exit in destructor";
    win.release();
    writeVideo.release();
//    cvReleaseMemStorage(&storage);
//    myOCREng->Clear();
//    myOCREng->End();
//    myOCRRus->Clear();
//    myOCRRus->End();
    qDebug() << "end Capture exit";
}

void CaptureWindow::update()
{

    if(m_flowFrame) {                                                   // Отображение стоп кадра по кнопке 's'
        win = imageFromDisplay();
    } else {
        m_srcWin.copyTo(win);
    }

    //        parseScript();

    drawDataSet();
    //        findImage(win);
    drawRectangles();
    drawLines();
    showRoi();


//    cv::Point _p;
//    QPoint _p;
//    double factor = 0.0;
//    int ret = 0;
//    imageExpectedCloseAutoPilot("pic_autoPilot", factor, ret, 17, 92, 128);
//    srchAreaOnceInRect("pic_systemGain", factor, _p, 9, 21, 39);

//    imageOverlay();
//    getPrimitives(m_side);
//    getStrStaticField("pic_fieldSystemName");
//    getTextStaticField("rect_curSystemMenuM", minScalar, maxScalar, "eng");
////    getTextApproximArea(getRect("rect_curSystemMenuM"), _p, "eng");
//    getTextApproximBoundingRect(getRect("rect_curSystemMenuM"), _p, "eng");
//    panel1Body();
//    testColor();
//    panelBodyCont();
//    panelBodyNav("sfdsa", "Xi Ophiuchi");

//    panelBodyContNotice();
//    panel1Header("навигация");
//    compass();
//    takeAimp();
//    subPanel1Nav();
//    qDebug() << m_cursorPan.sSubNavName;

//    menuDocking();
//    recognizDistance();


    if(m_saveRoi.active) {
        m_saveRoi.dst.copyTo(win(m_saveRoi.rect));
        m_saveRoi.active = false;
    }


    if(resizeImage) {
////        cv::Mat winResize;
////        cv::resize(win, winResize, win.size() / 2);
////        imshow("win1", winResize);

    } else {
        imshow("win1", win);
    }

    if(saveVideo) {
//        qDebug() << win.size().width << win.size().height;
        if(timeElapseForVideoSave.elapsed() > 25) {
            writeVideo << win;
            timeElapseForVideoSave.restart();
        }
        if(timeElapseForVideoSave2.elapsed() > 5000) {
            writeVideo.release();
            QString fName;
            fName = QString("%0\\%1.avi").arg(PATH_VIDEO_LOG).arg(QDateTime::currentDateTime().toString("dd-mmss"));
            writeVideo.open(fName.toStdString(), 0, 0, cv::Size(g_screen.width(), g_screen.height()), true);              // Open input
            if(!writeVideo.isOpened()) {
                qDebug()  << "couldn't open video: " << fName;
                saveVideo = false;
                return;
            }
            qDebug() << "create new file" << fName;
            timeElapseForVideoSave2.restart();
        }
    }

}

void CaptureWindow::loadJson()
{
    QString path = PATH_CONFIG;
    path.append("\\map.json");
    QFile file(path);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "file not opening";
    } else {
        QJsonDocument jDoc = QJsonDocument::fromJson(file.readAll());
        QJsonObject jObj = jDoc.object();
        for(QString sKey: jObj.keys()) {
            QJsonArray jArr = jObj[sKey].toArray();
            for(int i = 0; i < jArr.size(); i++) {

                mapForDiffPanelNav.insert(sKey, jArr[i].toObject()["station"].toString()) ;
            }
        }
        file.close();
    }
//    qDebug() << map;
}

cv::Mat CaptureWindow::checkRoiMat()
{
    cv::Mat _mat;
    if(state.roi)
        _mat = matroi;
    return _mat;
}

cv::Rect CaptureWindow::checkRoiRect()
{
    cv::Rect _rect;
    if(state.roi)
        _rect = m_rectMap["selectROI"];
    return _rect;
}

void CaptureWindow::slotSetDrawLine(bool aCheck, int anCount)
{
    m_nCountCell = anCount;
    m_bDrawLine = aCheck;
}

void CaptureWindow::setMinScalar(cv::Scalar acvMinScalar)
{
    qDebug() << "Min scalar" << acvMinScalar.val[0] << "," << acvMinScalar.val[1] << "," << acvMinScalar.val[2];
    minScalar = acvMinScalar;
}

void CaptureWindow::setMaxScalar(cv::Scalar acvMaxScalar)
{
    qDebug() << "Max scalar" << acvMaxScalar.val[0] << ","  << acvMaxScalar.val[1] << "," << acvMaxScalar.val[2];
    maxScalar = acvMaxScalar;
}

void CaptureWindow::setMinNumber(int i)
{
    minNumber = i;
    qDebug() << "minNumber" << i;
}

void CaptureWindow::setMidNumber(int i)
{
    midNumber = i;
    qDebug() << "midNumber" << i;
}

void CaptureWindow::setMaxNumber(int i)
{
    maxNumber = i;
    qDebug() << "maxNumber" << i;
}

void CaptureWindow::slotDrawMatchRect(Qt::CheckState t_state)
{
    draw_match_rect = static_cast<bool>(t_state);
}

cv::Mat *CaptureWindow::getSelectMatROI()
{
    return &matroi;
//    cv::Mat *tmp_roi = nullptr;
//    if(matroi.isContinuous())
//        tmp_roi = &matroi;
//    return tmp_roi;
}

cv::Rect *CaptureWindow::getNamedRect(QString asName)
{
    return &m_rectMap[asName.toStdString()];
}



bool CaptureWindow::checkRectName(QString asName)
{
    if(m_rectMap.find(asName.toStdString()) != m_rectMap.end())
        return true;
    return false;
}

void CaptureWindow::drawLine(cv::Point p1, cv::Point p2)
{
    cv::line(win, p1, p2, cv::Scalar(0, 255, 0), 2);
}

bool CaptureWindow::srchAreaOnceInMat(std::string asImageROI, cv::Mat acvMat, double &coeff)
{
//    cv::Mat _matXRoi;
    cv::Point _point;
    cv::Mat _roiMat;
//    win(acvMat).copyTo(_matXRoi);
    _roiMat = mp_dataSet->at(asImageROI).mat;
    if(findPointRoi(_roiMat, acvMat, _point, coeff)) {
        return true;
    } else
        return false;
}

cv::Point CaptureWindow::getPointAreaInMat(std::string asImageROI, cv::Mat acvMat, double factor)
{
    cv::Point _point = cv::Point(-1, -1);
    cv::Mat _roiMat;

    _roiMat = mp_dataSet->at(asImageROI).mat;
//    qDebug() << QString::fromStdString(asImageROI);
//    printMat(_roiMat);

    if(findPointRoi(_roiMat, acvMat, _point, factor)) {

        _point.x = _point.x + _roiMat.size().width / 2;
        _point.y = _point.y + _roiMat.size().height / 2;
    }
    return _point;
}

cv::Point CaptureWindow::getPointOfPattern(cv::Mat acvMat, std::string sPattern, double factor)
{
    cv::Point _point = cv::Point(-1, -1);
    cv::Mat _roiMat;
    _roiMat = mp_dataSet->at(sPattern).mat;

    cv::Mat result;
    VarForTemplateMatch varForTemplate;
    VarForTemplateMatch _f;
    int method = CV_TM_CCORR_NORMED;

    if(_roiMat.rows > acvMat.rows || _roiMat.cols > acvMat.cols) {
        qDebug() << "In 'cv::Point CaptureWindow::getPointOfPattern(cv::Mat acvMat, std::string sPattern, double factor)' error rows pattern > src";
        return _point;
    }
    cv::matchTemplate(acvMat, _roiMat, result, method);
    cv::minMaxLoc(result, &_f.m1, &_f.m2, &_f.p1, &_f.p2);

    if(_f.m2 > factor)
        _point = _f.p2;

    _point.x = _point.x + _roiMat.size().width / 2;
    _point.y = _point.y + _roiMat.size().height / 2;
    return _point;
}

cv::Point CaptureWindow::getPointAndFactorOfSPattern(cv::Mat acvMat, std::string sPattern, double &factor)
{
    double dFactorForPattern = 0.15;
    if(factor > dFactorForPattern)
        dFactorForPattern = factor;
    cv::Point _point = cv::Point(-1, -1);
    cv::Mat _roiMat;
    _roiMat = mp_dataSet->at(sPattern).mat;

    cv::Mat result;
    VarForTemplateMatch varForTemplate;
    VarForTemplateMatch _f;
    int method = CV_TM_CCORR_NORMED;
    if(_roiMat.rows > acvMat.rows || _roiMat.cols > acvMat.cols) {
        qDebug() << "error rows pattern > src";
        return _point;
    }

    cv::matchTemplate(acvMat, _roiMat, result, method);
    cv::minMaxLoc(result, &_f.m1, &_f.m2, &_f.p1, &_f.p2);

    if(_f.m2 > dFactorForPattern) {
        factor = _f.m2;
        _point = _f.p2;
    }

    _point.x = _point.x + _roiMat.size().width / 2;
    _point.y = _point.y + _roiMat.size().height / 2;
    return _point;
}

bool CaptureWindow::srchAreaOnceInRect(std::string asImageROI, double &coeff, QPoint &aPoint, int anCount, int anStart, int anEnd)
{
//    if(DEBUG2)
//        qDebug() << ".";
    if(mp_dataSet->find(asImageROI) == mp_dataSet->end()) {
        return false;
    }
    cv::Mat _matXRoi;
    cv::Point _point;
    cv::Mat _roiMat;
    cv::Rect cvRect = calcRectFromPartOfIndex(anCount, anStart, anEnd);
    win(cvRect).copyTo(_matXRoi);
    _roiMat = mp_dataSet->at(asImageROI).mat;
    if(findPointRoi(_roiMat, _matXRoi, _point, coeff)) {
        aPoint.setX(_point.x + cvRect.x + _roiMat.size().width / 2);
        aPoint.setY(_point.y + cvRect.y + _roiMat.size().height / 2);
        return true;
    } else
        return false;
}



void CaptureWindow::callBackMouse(int event, int x, int y, int flags)
{
    if(DEBUG2)
        qDebug() << ".";
    mouse.flag = flags & 7;
    switch (event) {
    case cv::EVENT_MOUSEMOVE: {
        switch (mouse.flag) {
        case cv::EVENT_FLAG_LBUTTON: {
            if(state.roi) {
                mouse.calcDeltaX(x);
                mouse.calcDeltaY(y);
                cv::Rect rect = mouse.getRect();
                m_rectMap["selectROI"] = rect;

            } else {
                mouse.setX2(x);
                mouse.setY2(y);
            }
//            std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
            break;
        }
        default:
            break;
        }

    }
        break;
    case cv::EVENT_LBUTTONDOWN:

        if(state.roi) {
            mouse.setDeltaX(x);
            mouse.setDeltaY(y);
        } else {
            mouse.setX1(x);
            mouse.setY1(y);
            mouse.leftButton = true;
        }
        break;
    case cv::EVENT_LBUTTONUP:
        if(state.roi) {
            m_rectMap.insert(std::pair<std::string, cv::Rect >("selectROI", mouse.getAndSaveRect()));
        } else {
            mouse.leftButton = false;
            cv::namedWindow("selectWin");
            cv::moveWindow("selectWin", 100, 100);
            state.roi = true;
            m_rectMap.insert(std::pair<std::string, cv::Rect >("selectROI", mouse.getAndSaveRect()));
//            ginfo->show();
//            std::cout << mouse.getRect().x << " " << mouse.getRect().y << " " << mouse.getRect().width << " " << mouse.getRect().height <<  std::endl;
        }
        break;
    case cv::EVENT_RBUTTONDOWN:
        m_rectMap.erase("selectROI");
        cv::destroyWindow("selectWin");
        state.roi = false;
        break;
    }
}

//bool CaptureWindow::srchAreaOnceInRect(std::string as_rectWhichInLook, std::string as_imageROI)
//{
//    cv::Mat _matXRoi;
//    cv::Rect _rect = mp_dataSet->at(as_rectWhichInLook).rect;
//    cv::Point _point;
//    cv::Mat _roiMat;
//    win(_rect).copyTo(_matXRoi);

//    _roiMat = mp_dataSet->at(as_imageROI).mat;
//    double coeff = MIN_COEFF_FOR_FIND;
//    if(findPointRoi(_roiMat, _matXRoi, _point, coeff)) {
//        return true;
//    } else
//        return false;
//}

cv::Point CaptureWindow::getPointAfterLookAreaOnceInRect(std::string as_rectWhichInLook, std::string as_imageROI)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Mat _matXRoi;
    cv::Rect _rect = mp_dataSet->at(as_rectWhichInLook).rect;
    cv::Point _point;
    cv::Mat _roiMat;
    win(_rect).copyTo(_matXRoi);

    _roiMat = mp_dataSet->at(as_imageROI).mat;
    double coeff = MIN_COEFF_FOR_FIND;
    if(findPointRoi(_roiMat, _matXRoi, _point, coeff)) {
        cv::Rect rectMatch(_rect.x + _point.x , _rect.y + _point.y, _roiMat.size().width, _roiMat.size().height);
        // Внести найденый квадрат для отрисовки
        {
            if(m_rectMap.find("rectMatch") == m_rectMap.end())
                m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
            else
                m_rectMap["rectMatch"] = rectMatch;
        }

        _point.x = _rect.x + _point.x  + _roiMat.size().width / 2;
        _point.y = _rect.y + _point.y + _roiMat.size().height / 2;
    }
    return _point;  //cv::Point(_point.x + m_nXOffset, _point.y + m_nYOffset);
}

cv::Point CaptureWindow::getPointAfterLookAreaInRect(std::string asImageROI, cv::Rect acvRect, double &factor)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Mat _matXRoi;
    cv::Point _point;
    cv::Mat _roiMat;
    win(acvRect).copyTo(_matXRoi);

    _roiMat = mp_dataSet->at(asImageROI).mat;
    if(findPointRoi(_roiMat, _matXRoi, _point, factor)) {
        cv::Rect rectMatch(acvRect.x + _point.x , acvRect.y + _point.y, _roiMat.size().width, _roiMat.size().height);
        // Внести найденый квадрат для отрисовки
        {
            if(m_rectMap.find("rectMatch") == m_rectMap.end())
                m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
            else
                m_rectMap["rectMatch"] = rectMatch;
        }

        _point.x = acvRect.x + _point.x  + _roiMat.size().width / 2;
        _point.y = acvRect.y + _point.y + _roiMat.size().height / 2;
    }
    return _point;  //cv::Point(_point.x + m_nXOffset, _point.y + m_nYOffset);
}

double CaptureWindow::getCoeffImageInRect(std::string asImageROI, cv::Rect acvRect)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Mat _matXRoi;
    cv::Mat _roiMat;
    win(acvRect).copyTo(_matXRoi);
    _roiMat = mp_dataSet->at(asImageROI).mat;
    cv::Mat result[count_match_methods];
    std::vector<VarForTemplateMatch> vec;
    std::vector<int> const_vec;
    const_vec.push_back(CV_TM_CCORR_NORMED);
    const_vec.push_back(CV_TM_CCOEFF_NORMED);
    for(size_t i = 0; i != const_vec.size(); ++i) {
        VarForTemplateMatch _f;
        if(_roiMat.rows > _matXRoi.rows || _roiMat.cols > _roiMat.cols) {
            qDebug() << "error rows pattern > src";
            return 0;
        }

        cv::matchTemplate(_matXRoi, _roiMat, result[i], const_vec[i]);
        _f.method = const_vec[i];
        cv::minMaxLoc(result[i], &_f.m1, &_f.m2, &_f.p1, &_f.p2);
        vec.push_back(_f);
    }
    vec.erase(std::remove_if(vec.begin(), vec.end(), [vec] (VarForTemplateMatch aItem) -> bool {
                   return aItem.m2 < MIN_COEFF_FOR_FIND_POINT;
               }), vec.end());

    double coeff = 0;
    if(vec.empty())
        return coeff;
    for(size_t i = 0; i < vec.size(); i++) {
         if(coeff < vec[i].m2)
             coeff = vec[i].m2;
    }
    return coeff;
}

CursorPanel *CaptureWindow::panel1Body(QString sName, QString asSystem)
{
    if(DEBUG2)
        qDebug() << ".";
    m_cursorPan.activeBody = false;

    if(comparisonStr(m_cursorPan.sHeaderName, "навигация") <= 2) {
        return panelBodyNav(sName, asSystem);
    } else if(comparisonStr(m_cursorPan.sHeaderName, "контакты") <= 2 ) {
        return panelBodyCont();
    }
    return &m_cursorPan;
}

CursorPanel *CaptureWindow::panelBodyNav(QString aName, QString asSystem)
{
    if(DEBUG2)
        qDebug() << ".";
    m_cursorPan.activeBody = false;
    m_cursorPan.sBodyName = "";
//    return &m_cursorPan;
    {
//        cv::drawContours( rectMat, vecRects, -1, cvBlue );

        cv::Mat rectMat;
        cv::Mat hsv;
        cv::Rect rectToCopy = cv::Rect(200, 350, 250, g_screen.height() - 500);
        win(rectToCopy).copyTo(rectMat);
        cv::cvtColor( rectMat, hsv, CV_BGR2HSV );
        cv::Mat mask;
        std::vector< std::vector< cv::Point> > vecRects;
        int minVal3 = 230;
        int minVal2 = 230;
        bool ret = false;
        while(minVal2 > 50 && !ret) {
            vecRects.clear();
    //        cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);
            cv::inRange(hsv, cv::Scalar(10, minVal2, minVal3), cv::Scalar(50, 255, 255), mask);
    //        cv::inRange(hsv, minScalar, maxScalar, mask);
    //        mask = getMaskPanel1(rectMat, minVal3);

            std::vector< std::vector< cv::Point> > contours2;
            cv::findContours(mask, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            for(size_t i = 0; i < contours2.size(); i++) {
                double area = cv::contourArea(contours2[i]);
                if(area < 1500)
                    continue;
                if(area > 2500 && area < 7500 ) {
                    cv::RotatedRect minRect = cv::minAreaRect(contours2[i]);
                    int widthLeftPanel = minRect.size.width > minRect.size.height ? minRect.size.width : minRect.size.height;
                    if(widthLeftPanel > 90 && widthLeftPanel < 130) {
                        //            putText(rectMat, QString::number(minRect[i].angle, 'f', 2).toStdString(), minRect[i].center, 1, 1, cvBlue, 2);
                        vecRects.push_back(contours2[i]);
                    }
                }
        //        qDebug() << QString::number(area, 'f', 3);
            }
            minVal3 -= 15;
            if(minVal3 <= 150) {
                minVal3 = 230;
                minVal2 -= 15;
            }

        }
//        qDebug() << "minVal2 =" << minVal2 << " minVal3 =" << minVal3;
//        cv::imshow("win3", mask);
        if(vecRects.size() == 1) {
            cv::Mat foundMat;
            rectMat(cv::boundingRect(vecRects[0])).copyTo(foundMat);
            cv::imshow("win2", foundMat);


            cv::Mat rotateMat;
            rectMat(boundingRect(vecRects[0])).copyTo(rotateMat);
            cv::Mat binTmp, hsvTmp;
            cv::cvtColor(rotateMat, hsvTmp, CV_BGR2HSV);
            cv::inRange(hsvTmp, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), binTmp);
    //        cv::rectangle(rectMat, boundingRect(vecRects[0]), cvRad, 1);
            std::vector< std::vector< cv::Point> > contourSubLeftPush;
            //**************************************************************************
            cv::findContours(binTmp, contourSubLeftPush, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            for(size_t i = 0; i < contourSubLeftPush.size(); i++) {
                double area = contourArea(contourSubLeftPush[i]);
                if(area > 2500 && area < 7500 ) {
                    cv::RotatedRect minRect = cv::minAreaRect(contourSubLeftPush[0]);
                    if(minRect.angle < -45)
                        minRect.angle += 90.;
                    cv::Mat rMatToWarp = cv::getRotationMatrix2D(minRect.center, minRect.angle, 1);
                    cv::warpAffine(rotateMat, rotateMat, rMatToWarp, rotateMat.size(), cv::INTER_CUBIC);
                    cv::Mat bin;
                    cv::Mat rotateMatHSV;
                    cv::cvtColor(rotateMat, rotateMatHSV, CV_BGR2HSV);
                    inRange(rotateMatHSV, cv::Scalar(12, 190, 190), cv::Scalar(50, 255, 255), bin);
                    std::vector< std::vector< cv::Point> > cont;
                    cv::findContours(bin, cont, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                    for(size_t i = 0; i < cont.size(); i++) {
                        double area = contourArea(cont[i]);
                        if(area > 2500 && area < 7500 ) {
                            cv::Mat binMatToRecognize;
                            bin(cv::boundingRect(cont[i])).copyTo(binMatToRecognize);
                            cv::Mat rColorRecognize;
                            cv::cvtColor(binMatToRecognize, rColorRecognize, CV_GRAY2BGR);
                            double coeff = 0.95;
                            if(srchAreaOnceInMat("menuNavSetFilter", rColorRecognize, coeff)) {
                                m_cursorPan.sBodyName = "push_filter";
                            } else if(srchAreaOnceInMat("menuNavResetFilter", rColorRecognize, coeff)) {
                                m_cursorPan.sBodyName = "close_push_filter";
                            } else if(srchAreaOnceInMat("menuNavMapGalaxy", rColorRecognize, coeff)) {
                                m_cursorPan.sBodyName = "push_galaxy_map";
                            } else if(srchAreaOnceInMat("menuNavMapSys", rColorRecognize, coeff)) {
                                m_cursorPan.sBodyName = "push_system_map";
                            }
                            cv::imshow("win4", binMatToRecognize);
                            m_cursorPan.activeBody = true;

                        }
                    }
                }
            }

        }
//        ////cv::imshow("win5", rectMat);
    }

    if(!m_cursorPan.activeBody) {                   //  если слева кнопка не активна



        cv::Mat rectMat;
        cv::Mat hsv;
        cv::Rect rectToCopy = cv::Rect(200, 350, g_screen.width() - 800, g_screen.height() - 500);
        win(rectToCopy).copyTo(rectMat);
        cv::cvtColor( rectMat, hsv, CV_BGR2HSV );
        cv::Mat mask;
        std::vector< std::vector< cv::Point> > vecRects;
        int minVal3 = 250;
        int minVal2 = 250;
        int minVal1 = 10;
        bool ret = false;
        while(minVal1 < 17 && vecRects.size() != 1) {
            vecRects.clear();
    //        cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);

            cv::inRange(hsv, cv::Scalar(minVal1, minVal2, minVal3), cv::Scalar(50, 255, 255), mask);
            cv::imshow("win3", mask);
            QApplication::processEvents();
    //        cv::inRange(hsv, minScalar, maxScalar, mask);
    //        mask = getMaskPanel1(rectMat, minVal3);

            std::vector< std::vector< cv::Point> > contours2;
            cv::findContours(mask, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            for(size_t i = 0; i < contours2.size(); i++) {
                double area = cv::contourArea(contours2[i]);
                if(area > 20000 && area < 30000 ) {
        //            cv::drawContours( rectMat, contours2, static_cast<int>(i), cvBlue );
        //            qDebug() << QString::number(area, 'f', 3);
                    //            putText(rectMat, QString::number(minRect[i].angle, 'f', 2).toStdString(), minRect[i].center, 1, 1, cvBlue, 2);
                    cv::RotatedRect minRect = cv::minAreaRect(contours2[i]);
                    int widthRightPanel, heightRightPanel;
                    if(minRect.size.width > minRect.size.height) {
                        widthRightPanel = minRect.size.width;
                        heightRightPanel = minRect.size.height;
                    } else {
                        widthRightPanel = minRect.size.height;
                        heightRightPanel = minRect.size.width;
                    }
    //                int widthRightPanel = minRect.size.width > minRect.size.height ? minRect.size.width : minRect.size.height;



                    if(widthRightPanel > 730 && widthRightPanel < 810 && heightRightPanel > 30 && heightRightPanel < 42) {
                        ret = true;
//                        qDebug() << "area =" << area << " width =" << minRect.size.width << " height =" << minRect.size.height;
                        vecRects.push_back(contours2[i]);
                        break;
                    }
                }
        //        qDebug() << QString::number(area, 'f', 3);
            }
            minVal3 -= 15;
            if(minVal3 <= 150) {
                minVal3 = 230;
                minVal2 -= 15;
                if(minVal2 < 100) {
                    minVal2 = 250;
                    minVal2 -= 15;
                    minVal1 += 2;
                }
            }
        }
//        qDebug() << "minVal2 =" << minVal2 << " minVal3 =" << minVal3;
        if(vecRects.size() == 1) {
            cv::Mat foundMat;
            rectMat(cv::boundingRect(vecRects[0])).copyTo(foundMat);
            cv::imshow("win2", foundMat);



                cv::RotatedRect minRect = cv::minAreaRect(vecRects[0]);
                int widthRightPanel = minRect.size.width > minRect.size.height ? minRect.size.width : minRect.size.height;
                if(widthRightPanel > 700) {
                    cv::Mat rotateMat;
                    int hightLine = minRect.size.width < minRect.size.height ? minRect.size.width : minRect.size.height;
                    cv::Rect rectToCut(boundingRect(vecRects[0]));
                    rectToCut.y -= hightLine / 2;
                    rectToCut.height += hightLine / 2;
                    rectMat(rectToCut).copyTo(rotateMat);
                    if(minRect.angle < -45)
                        minRect.angle += 90.;
                    cv::Mat rMatToWarp = cv::getRotationMatrix2D(minRect.center, minRect.angle, 1);
                    cv::warpAffine(rotateMat, rotateMat, rMatToWarp, rotateMat.size(), cv::INTER_CUBIC);


                    cv::Mat bin;
                    cv::Mat rotateMatHSV;
                    int minVal22, minVal23;
                    minVal22 = minVal23 = 230;
                    std::vector< std::vector< cv::Point> > cont;
                    bool ret2 = false;
                    while(minVal22 > 50 && !ret2) {
                        cont.clear();
                        cv::cvtColor(rotateMat, rotateMatHSV, CV_BGR2HSV);
        //                inRange(rotateMatHSV, minScalar, cv::Scalar(50, 255, 255), bin);
                        inRange(rotateMatHSV, cv::Scalar(10, minVal22, minVal23), cv::Scalar(50, 255, 255), bin);
                        cv::findContours(bin, cont, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                        cv::imshow("win4", bin);
                        for(size_t i = 0; i < cont.size(); i++) {
                            cv::Rect foundRectToRec = cv::boundingRect(cont[i]);
                            if(foundRectToRec.width > (rotateMat.size().width - 35)) {
                                double area = contourArea(cont[i]);
//                                qDebug() << area;
                                if(area > 19000 && area < 30000 ) {
                                    cv::Mat binRecognize;
                                    bin(cv::Rect(foundRectToRec.x + 5, foundRectToRec.y + 3, foundRectToRec.width - 10, foundRectToRec.height - 3)).copyTo(binRecognize);

                                    cv::Mat colorMapToSrchPattern;
                                    cv::cvtColor(binRecognize, colorMapToSrchPattern, cv::COLOR_GRAY2BGR);
                                    cv::Point point;
                                    int seekXDistance = colorMapToSrchPattern.size().width - 300;
                                    int cutXLeft = 0;
                                    for(int i = 0; i < sNamePicNavList.size(); i++) {
//                                        printMat(colorMapToSrchPattern);
                                        point = getPointAreaInMat(sNamePicNavList[i].toStdString(), colorMapToSrchPattern, 0.95);
                                        if(point.x > 0) {
//                                            cv::circle(colorMapToSrchPattern, point, 1, cv::Scalar(0, 0, 255),2);
                                            int widthMat = mp_dataSet->at(sNamePicNavList[i].toStdString()).mat.size().width;
                                            if(point.x + (widthMat / 2) > cutXLeft)
                                                cutXLeft = point.x + (widthMat / 2);
                                        }
//                                        qDebug() << sNamePicNavList[i] << ":";
                                    }
                                    int cutXRight = seekXDistance;
                                    int widthRightPic = 0;
                                    for(int i = 0; i < sNamePicNavListTar.size(); i++) {
                                        point = getPointAreaInMat(sNamePicNavListTar[i].toStdString(), colorMapToSrchPattern, 0.95);
                                        if(point.x > 0) {
//                                            cv::circle(colorMapToSrchPattern, point, 1, cv::Scalar(0, 0, 255),2);
                                            widthRightPic = mp_dataSet->at(sNamePicNavListTar[i].toStdString()).mat.size().width;
                                            if(point.x - (widthRightPic / 2) < cutXRight)
                                                cutXRight = point.x - (widthRightPic / 2);
                                        }
//                                        qDebug() << sNamePicNavListTar[i] << ":";
                                    }
                                    qDebug() << "";
                                    cv::Mat binCut;
                                    if(cutXRight - (cutXLeft + 5) < 0) {
                                        qDebug() << "fix bug отрицательная ширина";
                                        return &m_cursorPan;
                                    }
                                    binRecognize(cv::Rect(cutXLeft + 5, 0, cutXRight - (cutXLeft + 5), colorMapToSrchPattern.size().height)).copyTo(binCut);
//                                    cv::Mat binCut(cutMat.size(), CV_8U, 1);
//                                    inRange(cutMat, cv::Scalar(7, 190, 190), cv::Scalar(50, 255, 255), binCut);

                                    cv::imshow("win5", binCut);
                                    myOCREng->SetImage( (uchar*)binCut.data, binCut.size().width, binCut.size().height, binCut.channels(), binCut.step1());
                                    myOCREng->Recognize(nullptr);
                                    m_cursorPan.sBodyName = myOCREng->GetUTF8Text();
                                    if(m_cursorPan.sBodyName.isEmpty())
                                        m_cursorPan.sBodyName = "not recognized";
                                    m_cursorPan.sBodyName = m_cursorPan.sBodyName.simplified();
                                    m_cursorPan.sBodyName = m_cursorPan.sBodyName.toLower();
                                    deleteCharExtra(m_cursorPan.sBodyName);
                                    m_cursorPan.activeBody = true;
                                    if(comparisonStr(aName, m_cursorPan.sBodyName) <= 2) {

                                        qDebug() << "Успешное определение" << m_cursorPan.sBodyName;
                                        ret2 = true;
                                        return &m_cursorPan;
                                    }
                                    for(QString sStation: mapForDiffPanelNav.values(asSystem)) {
                                        sStation = sStation.simplified();
                                        sStation = sStation.toLower();
                                        deleteCharExtra(sStation);
                                        if(comparisonStr(sStation, m_cursorPan.sBodyName) <= 2) {

                                            qDebug() << m_cursorPan.sBodyName << "идём дальше";
                                            ret2 = true;
                                        } else if(m_cursorPan.sBodyName.contains(sStation)) {
                                            qDebug() << m_cursorPan.sBodyName << "идём дальше";
                                            ret2 = true;
                                        }

                                    }
                                    if(m_cursorPan.sBodyName.contains("has.mank")) {
                                        qDebug() << m_cursorPan.sBodyName << "идём дальше";
                                        ret2 = true;
                                    }
                                    qDebug() << "cont.size = " << cont.size() << " asSystem =" << asSystem;
                                    break;
                                }
                            }
                        }
                        minVal23 -= 25;
                        if(minVal23 <= 150) {
                            minVal23 = 230;
                            minVal22 -= 25;
                        }
                    }


                }


        }

        qDebug() << "Не найдено" << m_cursorPan.sBodyName << testError;
        testError++;



    } else {
//        qDebug() << "Панели навигации не определены";
    }
//    cv::imshow("win3", mask);
//    //cv::imshow("win2", rectMat);
    return &m_cursorPan;
}

CursorPanel *CaptureWindow::panelBodyCont()
{
    if(DEBUG2)
        qDebug() << ".";
//    cv::Mat hsv;
    cv::Mat rectMat;
//    win(cv::Rect(100, 100, g_screen.width() - 500, g_screen.height() - 200)).copyTo(rectMat);
//    cv::cvtColor( rectMat, hsv, CV_BGR2HSV );
    cv::Mat mask;

    std::vector< std::vector< cv::Point> > vecRects;
    std::vector< std::vector< cv::Point> > contours2;
    int minVal3 = 230;
    while(minVal3 > 150 && vecRects.size() != 1) {
        contours2.clear();
        vecRects.clear();
//        cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);
        mask = getMaskPanel1(rectMat, minVal3);
//        cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);
        cv::findContours(mask, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        for(size_t i = 0; i < contours2.size(); i++) {
            double area = cv::contourArea(contours2[i]);
            if(area < 1500)
                continue;
            if(area > 16000 && area < 30000 ) {
                vecRects.push_back(contours2[i]);
            }
        }
        minVal3 -= 15;
    }



    if(vecRects.size() == 1) {
        // Вставка на обрезку прокрутки
        if(cv::boundingRect(vecRects[0]).height > 400) {
            cv::Rect tmp = cv::boundingRect(vecRects[0]);
            tmp.width -= 50;
            mask(tmp).copyTo(mask);
            rectMat(tmp).copyTo(rectMat);
            contours2.clear();
            cv::findContours(mask, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            vecRects.clear();
            for(size_t i = 0; i < contours2.size(); i++) {
                double area = cv::contourArea(contours2[i]);
                if(area < 1500)
                    continue;
                if(area > 16000 && area < 25000 ) {
                    vecRects.push_back(contours2[i]);
                }
            }
        }
        for(size_t i = 0; i < vecRects.size(); i++) {
            cv::RotatedRect minRect = cv::minAreaRect(vecRects[i]);
            cv::Rect rectToCut(boundingRect(vecRects[i]));
            if(rectToCut.width > 500) {
                cv::Mat rotateMat;
                rectMat(rectToCut).copyTo(rotateMat);
                if(minRect.angle < -45)
                    minRect.angle += 90.;
                cv::Mat rMatToWarp = cv::getRotationMatrix2D(minRect.center, minRect.angle, 1);
                cv::warpAffine(rotateMat, rotateMat, rMatToWarp, rotateMat.size(), cv::INTER_CUBIC);
                //cv::imshow("win5", rotateMat);

                cv::Mat bin;
                cv::Mat rotateMatHSV;
                cv::cvtColor(rotateMat, rotateMatHSV, CV_BGR2HSV);
                inRange(rotateMatHSV, cv::Scalar(12, 190, 190), cv::Scalar(50, 255, 255), bin);
                std::vector< std::vector< cv::Point> > cont;
                cv::findContours(bin, cont, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                for(size_t i = 0; i < cont.size(); i++) {
                    double area = contourArea(cont[i]);
                    if(area > 15000 && area < 25000 ) {
                        cv::Rect rectRecognize = cv::boundingRect(cont[i]);
                        cv::Mat binRecognize;
                        bin(cv::Rect(rectRecognize.x + 5, rectRecognize.y + 1, rectRecognize.width - 30, rectRecognize.height - 2)).copyTo(binRecognize);


                        myOCREng->SetImage( (uchar*)binRecognize.data, binRecognize.size().width, binRecognize.size().height, binRecognize.channels(), binRecognize.step1());
                        myOCREng->Recognize(nullptr);
                        m_cursorPan.sBodyName = myOCREng->GetUTF8Text();
                        if(m_cursorPan.sBodyName.isEmpty())
                            m_cursorPan.sBodyName = "not recognized";
                        m_cursorPan.sBodyName = m_cursorPan.sBodyName.simplified();
                        m_cursorPan.sBodyName = m_cursorPan.sBodyName.toLower();
                        deleteCharExtra(m_cursorPan.sBodyName);
                        m_cursorPan.activeBody = true;

                        cv::imshow("win4", binRecognize);
                    }
                }

            } else if(rectToCut.width < 500 && rectToCut.width > 300) {
                cv::Mat rotateMat;
                rectMat(rectToCut).copyTo(rotateMat);
                if(minRect.angle < -45)
                    minRect.angle += 90.;
//                cv::circle(rotateMat, minRect.center, 2, cvBlue);
                cv::Mat rMatToWarp = cv::getRotationMatrix2D(cv::Point(rectToCut.width / 2, rectToCut.height / 2), minRect.angle, 1);
                cv::warpAffine(rotateMat, rotateMat, rMatToWarp, rotateMat.size(), cv::INTER_CUBIC);

                cv::Mat bin;
                cv::Mat rotateMatHSV;
                cv::cvtColor(rotateMat, rotateMatHSV, CV_BGR2HSV);
                inRange(rotateMatHSV, cv::Scalar(12, 190, 190), cv::Scalar(50, 255, 255), bin);
                std::vector< std::vector< cv::Point> > cont;
                cv::findContours(bin, cont, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                for(size_t i = 0; i < cont.size(); i++) {
                    double area = contourArea(cont[i]);
                    if(area > 15000 && area < 25000 ) {
                        cv::Rect rectRecognize = cv::boundingRect(cont[i]);
                        cv::Mat binRecognize;
                        bin(cv::Rect(rectRecognize.x + 5, rectRecognize.y + 3, rectRecognize.width - 30, rectRecognize.height - 6)).copyTo(binRecognize);
                        myOCRRus->SetImage( (uchar*)binRecognize.data, binRecognize.size().width, binRecognize.size().height, binRecognize.channels(), binRecognize.step1());
                        myOCRRus->Recognize(nullptr);
                        m_cursorPan.sBodyName = myOCRRus->GetUTF8Text();
                        if(m_cursorPan.sBodyName.isEmpty())
                            m_cursorPan.sBodyName = "not recognized";
                        m_cursorPan.sBodyName = m_cursorPan.sBodyName.simplified();
                        m_cursorPan.sBodyName = m_cursorPan.sBodyName.toLower();
                        deleteCharExtra(m_cursorPan.sBodyName);
//                        qDebug() << "recognize: " << m_cursorPan.sBodyName << " set req_docking";
                        m_cursorPan.sBodyName = "req_docking";
                        m_cursorPan.activeBody = true;
                        cv::imshow("win4", binRecognize);
                    }
                }
            }
            m_cursorPan.rectBody = rectToCut;

        }
    }  else {
//        qDebug() << "Панели навигации не определены";
    }
//    qDebug() << "m_cursorPan.sBodyName" << m_cursorPan.sBodyName;
    //cv::imshow("win3", mask);
    //cv::imshow("win2", rectMat);
    return &m_cursorPan;
}

CursorPanel *CaptureWindow::panelBodyContNotice()
{
    if(DEBUG2)
        qDebug() << ".";
    if(m_cursorPan.activeBody) {
        cv::Mat hsv;
        cv::Mat rectMat;
        cv::Mat rotateMat;
        cv::Mat mask;
        win(cv::Rect(100, 100, g_screen.width() - 500, g_screen.height() - 200)).copyTo(rectMat);
        cv::cvtColor( rectMat, hsv, CV_BGR2HSV );
        cv::Rect rect(m_cursorPan.rectBody.x, m_cursorPan.rectBody.y, m_cursorPan.rectBody.width, m_cursorPan.rectBody.height + 90);
        cv::rectangle(rectMat, rect, cvBlue, 1);
        rectMat(rect).copyTo(rotateMat);
        cv::Mat rMatToWarp = cv::getRotationMatrix2D(cv::Point(rect.width / 2, rect.height / 2), -4, 1);
        cv::warpAffine(rotateMat, rotateMat, rMatToWarp, rotateMat.size(), cv::INTER_CUBIC);
//        getMaskOfMat(rotateMat, mask, cv::Scalar(125, 0, 82), cv::Scalar(163, 128, 255));
//        getMaskOfMat(rotateMat, mask, minScalar, maxScalar);
        cv::inRange(rotateMat, cv::Scalar(157, 116, 132), cv::Scalar(255, 255, 255), mask);
//        cv::inRange(rotateMat, minScalar, maxScalar, mask);
        cv::bitwise_not(mask, mask);
        cv::Mat recognizeMat;
        mask(cv::Rect(50, mask.size().height - 70, mask.size().width - 50, 50)).copyTo(recognizeMat);
//        cv::rectangle(rectMat, rect, cvBlue);
        myOCRRus->SetImage( (uchar*)recognizeMat.data, recognizeMat.size().width, recognizeMat.size().height, recognizeMat.channels(), recognizeMat.step1());
        myOCRRus->Recognize(nullptr);
        m_cursorPan.sBodyNameNotice = myOCRRus->GetUTF8Text();
        if(m_cursorPan.sBodyNameNotice.isEmpty())
            m_cursorPan.sBodyNameNotice = "not recognized";
        m_cursorPan.sBodyNameNotice = m_cursorPan.sBodyNameNotice.simplified();
        m_cursorPan.sBodyNameNotice = m_cursorPan.sBodyNameNotice.toLower();
        deleteCharExtra(m_cursorPan.sBodyNameNotice);
        qDebug() << "notice recognize: " << m_cursorPan.sBodyNameNotice;


//        cv::imshow("win3", rotateMat);
        cv::imshow("win5", recognizeMat);
//        cv::imshow("win4", rectMat);

    } else {
        m_cursorPan.sBodyNameNotice = "Notice: not active";
    }
    return &m_cursorPan;
}


cv::Mat CaptureWindow::getMaskPanel1(cv::Mat &aRectMat, int aMinVal3)
{
    cv::Rect rectToCopy = cv::Rect(100, 100, g_screen.width() - 500, g_screen.height() - 200);
    win(rectToCopy).copyTo(aRectMat);
    cv::Mat hsv;

//    m_saveRoi.active = true;
//    SaveROI saveRoi;
//    saveRoi.dst = rectMat;
//    saveRoi.rect = rectToCopy;

    cv::cvtColor( aRectMat, hsv, CV_BGR2HSV );
    cv::Mat mask;
//    cv::inRange(hsv, minScalar, maxScalar, mask);
    cv::inRange(hsv, cv::Scalar(10, 210, aMinVal3), cv::Scalar(50, 255, 255), mask);

    return  mask;
}


CursorPanel *CaptureWindow::panel1Header(QString sName)
{
    if(DEBUG2)
        qDebug() << ".";
    m_cursorPan.sHeaderName = "";
    m_cursorPan.activeHeader = false;
    cv::Mat rectMat;

    cv::Mat mask;
    cv::Mat hsv;
    cv::Rect rectToCopy = cv::Rect(150, 150, 1000, 250);
    win(rectToCopy).copyTo(rectMat);
////    cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);
//    cv::inRange(hsv, minScalar, maxScalar, mask);
    std::vector< std::vector< cv::Point> > vecRects;
    int minVal3 = 255;
    int minVal1 = 10;
    while(minVal1 < 50 && (vecRects.empty() || vecRects.size() > 2)) {
        vecRects.clear();
        std::vector< std::vector< cv::Point> > contours2;


//        mask = getMaskPanel1(rectMat, minVal3);


        cv::cvtColor( rectMat, hsv, CV_BGR2HSV );
    //    cv::inRange(hsv, minScalar, maxScalar, mask);
//        cv::inRange(hsv, cv::Scalar(10, 210, minVal3), cv::Scalar(50, 255, 255), mask);
        cv::inRange(hsv, cv::Scalar(minVal1, minVal3, minVal3), cv::Scalar(minVal1 + 10, 255, 255), mask);
//        cv::inRange(hsv, cv::Scalar(10, minVal3, minVal3), cv::Scalar(50, maxNumber, maxNumber), mask);

//        cv::imshow("win2", mask);
//        QApplication::processEvents();


        cv::findContours(mask, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//        cv::drawContours(rectMat, contours2, -1, cvGreen, 2);
        for(size_t i = 0; i < contours2.size(); i++) {
            cv::Rect push = cv::boundingRect(contours2[i]);
            if(push.width < 240 || push.width > 310 || push.height > 60)
                continue;

            cv::RotatedRect rotRect = cv::minAreaRect(contours2[i]);
            cv::Rect rectToCut(boundingRect(contours2[i]));
            int hightLine = rotRect.size.width < rotRect.size.height ? rotRect.size.width : rotRect.size.height;
            rectToCut.y -= hightLine / 2;
            rectToCut.height += hightLine / 2;
            if(rectToCut.x < 0 || rectToCut.y < 0 || rectToCut.x + rectToCut.width > rectMat.size().width || rectToCut.y + rectToCut.height > rectMat.size().height) {
                continue;
            }



            double area = cv::contourArea(contours2[i]);
            if(area > 5000 && area < 15000) {
    //            qDebug() << push.width << area;
//                qDebug() << "width =" << push.width << " height =" << push.height;
                vecRects.push_back(contours2[i]);
            }
        }
        minVal3 -= 15;
        if(minVal3 < 50) {
            minVal3 = 255;
            minVal1 += 10;
        }


    }
//    cv::imshow("win4", mask);
//    cv::imshow("win2", rectMat);
//    return &m_cursorPan;

    if(vecRects.size() != 1) {
        cv::imshow("win2", mask);
//        cv::imshow("win2", rectMat);
        qDebug() << "Шаблон прямоугольника header menu не найден";

        return &m_cursorPan;
    }



    m_cursorPan.activeHeader = true;

//    cv::imshow("win4", mask);





    cv::Mat rotateMat;
    cv::Rect rectToCut(boundingRect(vecRects[0]));
    rectMat(rectToCut).copyTo(rotateMat);



    cv::RotatedRect rotRect = cv::minAreaRect(vecRects[0]);
    if(rotRect.angle < -45)
        rotRect.angle += 90.;
    cv::Point pointCenter = cv::Point(rotRect.center.x - rectToCut.x, rotRect.center.y - rectToCut.y);
    cv::Mat rMatToWarp = cv::getRotationMatrix2D(pointCenter, rotRect.angle, 1);
    cv::warpAffine(rotateMat, rotateMat, rMatToWarp, rotateMat.size(), cv::INTER_CUBIC);


//    cv::imshow("win3", edgeMatToRotate);
//    cv::imshow("win4", rotateMat);
//    return &m_cursorPan;



#define WIDTH_TO_CUT 4
    rotateMat(cv::Rect(WIDTH_TO_CUT, WIDTH_TO_CUT, rotateMat.size().width - WIDTH_TO_CUT * 2, rotateMat.size().height - WIDTH_TO_CUT * 2)).copyTo(rotateMat);



            cv::Mat dst;
            dst = rotateMat;
            cv::Mat mask2;
            cv::Mat edgeMat;


            cv::Mat gray;
            cv::Mat recognizMask;
//            cv::imshow("win1", foundMat);
            cv::cvtColor(dst, gray, CV_BGR2GRAY);

            QString sText;
            int minVal11 = 70;
            bool ret = false;
            while(minVal11 < 256 && !ret) {
        //        cv::threshold(gray, mask2, minNumber, maxNumber, cv::THRESH_BINARY);
                cv::threshold(gray, mask2, minVal11, 153, cv::THRESH_BINARY);
                cv::inRange(mask2, cv::Scalar(0, 0, 0), cv::Scalar(1, 1, 1), recognizMask);

                int black, white;
                if(!blackLessWhite(recognizMask, black, white)) {
                    cv::bitwise_not(recognizMask, recognizMask);
                }

                myOCRRus->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
                myOCRRus->Recognize(nullptr);
                sText = myOCRRus->GetUTF8Text();

                qDebug() << "source text recognize =" << sText;
                minVal11 += 70;
                sText = sText.simplified();
                sText = sText.toLower();
                deleteCharExtra(sText);

                if(sText.contains(sName) || comparisonStr(sText, sName) <= 2) {
                    m_cursorPan.sHeaderName = sName;
                    ret = true;
                }

//                cv::imshow("win4", mask2);
//                cv::imshow("win5", recognizMask);
//                QApplication::processEvents();
//                QThread::msleep(100);
            }


        //    qDebug() << "minVal1 =" << minVal1 << "text: " << sText;
//            cv::imshow("win4", dst);





//            m_cursorPan.sHeaderName = myOCRRus->GetUTF8Text();

            qDebug() << "RECOGNIZE: " << m_cursorPan.sHeaderName << testCounter;
            testCounter++;


    cv::imshow("win3", rotateMat);
    cv::imshow("win2", mask);
    return &m_cursorPan;
}

std::vector<cv::Point> CaptureWindow::findPoints(std::vector< cv::Point>  contours2, cv::Mat &dst) {
    if(DEBUG2)
        qDebug() << ".";
    std::vector<cv::Point> vec;
    std::vector<cv::Point> min_x;
    min_x.push_back(cv::Point(STANDART_HD_WIDTH, STANDART_HD_HEIGHT));
    std::vector<cv::Point> max_x;
    max_x.push_back(cv::Point(0, 0));
    std::vector<cv::Point> min_y;
    min_y.push_back(cv::Point(STANDART_HD_WIDTH, STANDART_HD_HEIGHT));
    std::vector<cv::Point> max_y;
    max_y.push_back(cv::Point(0, 0));
    for(size_t i = 0; i < contours2.size(); i++) {
        if(contours2[i].x < min_x.at(0).x) {
            min_x.clear();
            min_x.push_back(contours2[i]);
        } else if(contours2[i].x == min_x.at(0).x) {
            min_x.push_back(contours2[i]);
        }
        if(contours2[i].x > max_x.at(0).x) {
            max_x.clear();
            max_x.push_back(contours2[i]);
        } else if(contours2[i].x == max_x.at(0).x){
            max_x.push_back(contours2[i]);
        }
        if(contours2[i].y < min_y.at(0).y) {
            min_y.clear();
            min_y.push_back(contours2[i]);
        } else if(contours2[i].y == min_y.at(0).y)
        {
            min_y.push_back(contours2[i]);
        }
        if(contours2[i].y > max_y.at(0).y) {
            max_y.clear();
            max_y.push_back(contours2[i]);
        } else if(contours2[i].y == max_y.at(0).y){
            max_y.push_back(contours2[i]);
        }
    }




    //            cv::Rect rectTmp = cv::Rect(min_x.x, min_y.y, max_x.x - min_x.x, max_y.y - min_y.y);

    //            vec.push_back(rectTmp);
    // left up
    cv::Point leftUp = min_x.at(0);
    for(size_t i = 0; i < min_x.size(); i++) {
        if(leftUp.y > min_x.at(i).y)
            leftUp = min_x.at(i);
    }
    cv::Point upRight = min_y.at(0);
    for(size_t i = 0; i < min_y.size(); i++) {
        if(upRight.x < min_y.at(i).x)
            upRight = min_y.at(i);
    }
    cv::Point rightBot = max_x.at(0);
    for(size_t i = 0; i < max_x.size(); i++) {
        if(rightBot.y < max_x.at(i).y)
            rightBot = max_x.at(i);
    }
    cv::Point botLeft = max_y.at(0);
    for(size_t i = 0; i < max_y.size(); i++) {
        if(botLeft.x > max_y.at(i).x)
            botLeft = max_y.at(i);
    }
    vec.push_back(leftUp);
    vec.push_back(upRight);
    vec.push_back(rightBot);
    vec.push_back(botLeft);
    cv::circle(win, leftUp, 2, cv::Scalar(0, 0, 255), 2);
//    cv::putText(dst, "leftUp", leftUp, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

    cv::circle(win, upRight, 2, cv::Scalar(0, 0, 255), 2);
//    cv::putText(dst, "upRight", upRight, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

    cv::circle(win, rightBot, 2, cv::Scalar(0, 0, 255), 2);
//    cv::putText(dst, "rightBot", rightBot, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);

    cv::circle(win, botLeft, 2, cv::Scalar(0, 0, 255), 2);
//    cv::putText(dst, "botLeft", botLeft, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 2);
    return vec;
}

bool CaptureWindow::transformMenu1(cv::Mat &acvMatRet)
{
    if(DEBUG2)
        qDebug() << ".";
#define PANEL_1_WIDTH       1020
#define PANEL_1_HEIGHT      680
#define PANEL_1_Y_SEEK      50
    cv::Mat tmp(win.size().height, win.size().width, win.depth(), win.channels());
    cv::resize(win, tmp, tmp.size(), 0, 0, cv::INTER_LINEAR);

//    int centerX = USING_WIDTH / 2;
//    int centerY = USING_HEIGHT / 2;
//    double angle = -3.47;
//    cv::Mat rot_mat = cv::getRotationMatrix2D(cv::Point(centerX, centerY), angle, 1);
//    cv::warpAffine(acvMat, acvMat, rot_mat, acvMat.size(), cv::INTER_CUBIC);

    cv::Point2f srcTri[4], dstQuad[4];
    srcTri[0].x = 0;
    srcTri[0].y = 0;
    srcTri[1].x = tmp.size().width - 1;
    srcTri[1].y = 0;
    srcTri[2].x = 0;
    srcTri[2].y = tmp.size().height - 1;
    srcTri[3].x = tmp.size().width - 1;
    srcTri[3].y = tmp.size().height - 1;


    dstQuad[0].x = 0;  //Top left
    dstQuad[0].y = 0;
    dstQuad[1].x = tmp.size().width - 1;  //Top right
    dstQuad[1].y = -45;
    dstQuad[2].x = -65;  //Bottom left
    dstQuad[2].y = tmp.size().height;
    dstQuad[3].x = tmp.size().width - 30;  //Bottom right
    dstQuad[3].y = tmp.size().height + 315;
    cv::Mat warp_matrix = cv::getPerspectiveTransform(srcTri, dstQuad);
    cv::warpPerspective(tmp, tmp, warp_matrix, tmp.size());



    cv::Mat transformMat, dst;                   // трансформир
    cv::Mat binPrep(transformMat.size(), CV_8U, 1);
    tmp(cv::Rect(50, 100, tmp.size().width - 665, tmp.size().height - 150)).copyTo(transformMat);
    cv::inRange(transformMat, cv::Scalar(0, 93, 200), cv::Scalar(100, 255, 255), binPrep);
    std::vector< std::vector< cv::Point> > contours2Src;
    std::vector< cv::Vec4i > hierarchy;
    cv::findContours(binPrep, contours2Src, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect> vecRects;
    std::vector<cv::Rect> vecRectsShort;
    getRectsOfPatternMenu1(contours2Src, vecRects, vecRectsShort);
    int x1, y2;
    if(!getLeftAndUpInVecRects(vecRects, x1, y2)) {
        return false;
    }
    if(vecRectsShort.empty()) {
        return false;
    }
    cv::Rect rectToCut(x1, y2 - PANEL_1_Y_SEEK, PANEL_1_WIDTH, PANEL_1_HEIGHT);
    if(rectToCut.height + rectToCut.y >= 930 || rectToCut.width + rectToCut.x >= 1250) {
        return false;
    }
    transformMat(rectToCut).copyTo(dst);

//    imshow("win2", binPrep);
//    imshow("win3", dst);
    acvMatRet = dst;
    return true;
}


bool CaptureWindow::transformSubNavMenu1(cv::Mat &acvMatRet)
{
    if(DEBUG2)
        qDebug() << ".";
#define PANEL_1_WIDTH_SUBNAV       119
#define PANEL_1_HEIGHT_SUBNAV      65
    cv::Mat tmp(win.size().height, win.size().width, win.depth(), win.channels());
    cv::resize(win, tmp, tmp.size(), 0, 0, cv::INTER_LINEAR);

//    int centerX = USING_WIDTH / 2;
//    int centerY = USING_HEIGHT / 2;
//    double angle = -3.47;
//    cv::Mat rot_mat = cv::getRotationMatrix2D(cv::Point(centerX, centerY), angle, 1);
//    cv::warpAffine(acvMat, acvMat, rot_mat, acvMat.size(), cv::INTER_CUBIC);

    cv::Point2f srcTri[4], dstQuad[4];
    srcTri[0].x = 0;
    srcTri[0].y = 0;
    srcTri[1].x = tmp.size().width - 1;
    srcTri[1].y = 0;
    srcTri[2].x = 0;
    srcTri[2].y = tmp.size().height - 1;
    srcTri[3].x = tmp.size().width - 1;
    srcTri[3].y = tmp.size().height - 1;


    dstQuad[0].x = 0;  //Top left
    dstQuad[0].y = 0;
    dstQuad[1].x = tmp.size().width - 1;  //Top right
    dstQuad[1].y = -45;
    dstQuad[2].x = -65;  //Bottom left
    dstQuad[2].y = tmp.size().height;
    dstQuad[3].x = tmp.size().width - 30;  //Bottom right
    dstQuad[3].y = tmp.size().height + 315;
    cv::Mat warp_matrix = cv::getPerspectiveTransform(srcTri, dstQuad);
    cv::warpPerspective(tmp, tmp, warp_matrix, tmp.size());



    cv::Mat transformMat, dst;                   // трансформир
    cv::Mat binPrep(transformMat.size(), CV_8U, 1);
    tmp(cv::Rect(250, 550, tmp.size().width - 865, tmp.size().height - 550)).copyTo(transformMat);
    cv::inRange(transformMat, cv::Scalar(0, 93, 200), cv::Scalar(100, 255, 255), binPrep);
    std::vector< std::vector< cv::Point> > contours2Src;
    std::vector< cv::Vec4i > hierarchy;
    cv::findContours(binPrep, contours2Src, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect> vecRects;
    getRectsOfPatternMenu1SubNav(contours2Src, vecRects);
    for(size_t i = 0; i < vecRects.size(); i++) {
        cv::rectangle(transformMat, vecRects[i], cv::Scalar(0xFF, 0x0, 0x0));
    }

    int x1, y2;
    if(!getLeftAndUpInVecRects(vecRects, x1, y2)) {
//        imshow("win2", binPrep);
//        imshow("win3", transformMat);
        return false;
    }
    cv::Rect rectToCut(x1, y2, PANEL_1_WIDTH_SUBNAV, PANEL_1_HEIGHT_SUBNAV);
//    printRect(rectToCut);
//    printMat(transformMat);
    if(rectToCut.height + rectToCut.y >= 530 || rectToCut.width + rectToCut.x >= 1055) {
//        imshow("win2", binPrep);x
//        imshow("win3", transformMat);
        return false;
    }
    transformMat(rectToCut).copyTo(dst);

//    imshow("win2", binPrep);
//    imshow("win3", dst);
    acvMatRet = dst;
    return true;
}

void CaptureWindow::addDrawRect(cv::Rect acvRect)
{
    if(DEBUG2)
        qDebug() << ".";
    m_rectMap["test"] = acvRect;
}

void CaptureWindow::showRoi()
{
    if(!state.roi)
        return;
    if(m_rectMap.find("selectROI") != m_rectMap.end()) {
        cv::Rect _rect = m_rectMap["selectROI"];
        _rect.x++;
        _rect.y++;
        _rect.width -= 2;
        _rect.height -= 2;
        if(_rect.width < 1 || _rect.height < 1)
            return;
        win(_rect).copyTo(matroi);
//        std::cout << matroi.step << " " << matroi.channels()
//                  << " width=" << matroi.step / static_cast<unsigned int>(matroi.channels())
//                  << " heigth=" << sizeof(matroi.data) / static_cast<unsigned int>(matroi.channels())
//                  << std::endl;

        imshow("selectWin", matroi);
    }

}

void CaptureWindow::drawRectangles()
{
    if(mouse.flag == cv::EVENT_FLAG_LBUTTON && mouse.leftButton)            // Рисуется только в момент выделения мышкой
        cv::rectangle(win, mouse.getRect(), cv::Scalar(0x0,0xFF,0x0));



    for(auto pr: m_rectMap) {
        if(pr.first == "test")
            cv::rectangle(win, pr.second, cv::Scalar(0xFF,0x0,0x0));
        if(pr.first == "rectMatch" && draw_match_rect)
            cv::rectangle(win, pr.second, cv::Scalar(0x0,0x0,0xFF));
        if(pr.first == "selectROI")
            cv::rectangle(win, pr.second, cv::Scalar(0x0,0xFF,0x0));

    }


}

void CaptureWindow::drawLines()
{

    if(m_bDrawLine) {
        if(DEBUG2)
            qDebug() << ".";
        int partWidth = g_screen.width() / m_nCountCell;
        int partHeight = g_screen.height() / m_nCountCell;
    //    qDebug() << USING_WIDTH - partWidth * anCount << " " << USING_HEIGHT - partHeight * anCount;
        int x1 = 1, y1 = 1, x2 = 0, y2 = 0;
        while(x1 < g_screen.width()) {
            cv::line(win, cv::Point(x1, 0), cv::Point(x1, g_screen.height()), cvScalar(0x0, 0xFF, 0x0));
            x1 += partWidth;
        }

        while(y1 < g_screen.height()) {
            cv::line(win, cv::Point(0, y1), cv::Point(g_screen.width(), y1), cvScalar(0x0, 0xFF, 0x0));
            y1 += partHeight;
        }

        x1 = 1, y1 = 1, x2 = 0, y2 = 0;
        int i_start = 0;
        int ix_index = 0;
        int iy_index = 0;
        while(i_start < (m_nCountCell * m_nCountCell)) {
            if(ix_index == m_nCountCell ) {
               iy_index++;
               ix_index = 0;
            }
            x1 = ix_index * partWidth;
            y1 = iy_index * partHeight;
            cv::Point p1 = cv::Point(x1 + 15, y1 + partHeight - 15);
            cv::putText(win, QString::number(i_start).toStdString(), p1,  cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0x0, 0xFF, 0x0), 2);

            ix_index++;
            i_start++;
        }
    }
}

void CaptureWindow::drawDataSet()
{
    for(auto pr: *mp_dataSet) {
        if(pr.second.active)
            cv::rectangle(win, pr.second.rect, cv::Scalar(0xFF,0x0,0x0));
    }
}

//void CaptureWindow::setCaptureImage(QJsonObject jobj)
//{
//    Q_UNUSED(jobj)
//}

//void CaptureWindow::findImage(cv::Mat _mat)
//{
//    Q_UNUSED(_mat)
//}

bool CaptureWindow::findPointRoi(cv::Mat &t_mat, cv::Mat &t_whereFind, cv::Point &t_point, double &aFactor)
{
    if(DEBUG2)
        qDebug() << ".";
    if(t_whereFind.size().height < t_mat.size().height)
        return false;
    cv::Mat result[count_match_methods];
    std::vector<VarForTemplateMatch> vec;
    std::vector<int> const_vec;
    const_vec.push_back(CV_TM_CCORR_NORMED);
//    const_vec.push_back(CV_TM_CCOEFF_NORMED);
    for(size_t i = 0; i != const_vec.size(); ++i) {
        VarForTemplateMatch _f;
        if(t_mat.rows > t_whereFind.rows || t_mat.cols > t_whereFind.cols) {
            qDebug() << "bool CaptureWindow::findPointRoi(cv::Mat &t_mat, cv::Mat &t_whereFind, cv::Point &t_point, double &aFactor) error rows pattern > src";
            return false;
        }
        cv::matchTemplate(t_whereFind, t_mat, result[i], const_vec[i]);
        _f.method = const_vec[i];
        cv::minMaxLoc(result[i], &_f.m1, &_f.m2, &_f.p1, &_f.p2);
        vec.push_back(_f);
    }
    for(auto tmp: vec) {
//        if(tmp.m2 < 0.9)
//            qDebug() << tmp.m2;
    }
    cv::Point match = findMatchPoint(vec, aFactor);
    t_point = match;
    //cv::imshow("win4", t_whereFind);
    if(match != cv::Point(0,0)) {
//        cv::Rect rectMatch(match.x , match.y, t_mat.size().width, t_mat.size().height);
//        if(m_rectMap.find("rectMatch") == m_rectMap.end())
//            m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//        else
//            m_rectMap["rectMatch"] = rectMatch;
        return true;
    } else {
        m_rectMap.erase("rectMatch");
        return false;
    }
}

bool CaptureWindow::findRectRoi(cv::Mat t_mat, cv::Rect &t_rect, double t_factor)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Mat result[count_match_methods];
    std::vector<VarForTemplateMatch> vec;
    std::vector<int> const_vec;
    const_vec.push_back(CV_TM_CCORR_NORMED);
//    const_vec.push_back(CV_TM_CCOEFF_NORMED);
    for(size_t i = 0; i != const_vec.size(); ++i) {
        VarForTemplateMatch _f;
//        Helper::printInfoMat(t_mat);
//        Helper::printInfoMat(win);
        if(t_mat.rows > win.rows || t_mat.cols > win.cols) {
            qDebug() << "error rows pattern > src";
            return false;
        }
        cv::matchTemplate(win, t_mat, result[i], const_vec[i]);
        _f.method = const_vec[i];
        cv::minMaxLoc(result[i], &_f.m1, &_f.m2, &_f.p1, &_f.p2);
        vec.push_back(_f);
    }
    cv::Point match;
//    if(t_factor > 0.97)
    match = findMatchPoint(vec, t_factor);
//    else
//    match = findMatchPoint(vec);
    if(match != cv::Point(0,0)) {
        cv::Rect rectMatch(match.x , match.y, t_mat.size().width, t_mat.size().height);
        t_rect = rectMatch;
//        if(m_rectMap.find("rectMatch") == m_rectMap.end())
//            m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//        else
//            m_rectMap["rectMatch"] = rectMatch;
        return true;
    } else {
        m_rectMap.erase("rectMatch");
        return false;
    }
}

cv::Point CaptureWindow::findMatchPoint(std::vector<VarForTemplateMatch> _vec, double &aFactor)
{
//    cv::Point point[count_match_methods * 2];
//    std::vector<cv::Point> point;
//    for(int i = 0; i < count_match_methods; i++) {
//        point.push_back(p1[i]);
//        point.push_back(p2[i]);
//    }
//    std::map<int, int> m;                                                 //------------
//    for(size_t i = 0; i < _vec.size(); i++) {
//        for(size_t k = _vec.size(); k > i; --k) {
//            if(_vec[i].p1 == _vec[k].p1) {
//                if(_vec[i].p1 == cv::Point(0,0) || _vec[i].p2 == cv::Point(0,0)) {
//                    continue;
//                }
//                    _vec[i].matchP1++;
//                    _vec[k].matchP1++;
////                    auto it = m.find(static_cast<int>(i));
////                    if(it == m.end()) {     // Создать новый элемент где ключ равен номеру в массиве
////                        m.insert(std::pair<int, int>(i, 0));
////                    } else {
////                        it->second = it->second + 1;
////                    }
//            } else if(_vec[i].p2 == _vec[k].p2) {
//                _vec[k].matchP2++;
//                _vec[i].matchP2++;
//            }
//        }
//    }
//    printMapPoint(_vec);
//    typedef std::function<bool()
//    std::vector<std::pair<int, int> > vec;
//    std::cout << "-----------" << std::endl;

//    sort(_vec.begin(), _vec.end(), CmpP1<VarForTemplateMatch >());
//    printMapPoint(_vec);
//    std::cout << "-----------" << std::endl;

//    sort(_vec.begin(), _vec.end(), CmpP2<VarForTemplateMatch >());

////    std::cout << "stop";
//    // Declaring the type of Predicate that accepts 2 pairs and return a bool
//    typedef std::function<bool(std::pair<int, int>, std::pair<int, int>)> Comparator;

//    // Defining a lambda function to compare two pairs. It will compare two pairs using second field
//    Comparator compFunctor =
//            [](std::pair<int, int> elem1 ,std::pair<int, int> elem2)
//    {
//        return elem1.second < elem2.second;
//    };
//    // Declaring a set that will store the pairs using above comparision logic
//    std::set<std::pair<int, int>, Comparator> setOfmap(
//                m.begin(), m.end(), compFunctor);
//    // Iterate over a set using range base for loop
//    // It will display the items in sorted order of values
////    for (std::pair<int, int> element : setOfmap)
////        std::cout << element.first << " :: " << element.second << std::endl;






//    std::cout << "----------------" << std::endl;
//    Helper::printMapPoint(_vec);

//    for(auto it = _vec.begin(); it != _vec.end(); it++) {
//        if(it->method == CV_TM_CCORR_NORMED)
//            if(it->m2 < 0.97) {
//                _vec.erase(it);
//                if(it == _vec.begin()) {
//                    qDebug() << "this is begin it";
//                }
//            }
////        if(it->method == CV_TM_CCOEFF_NORMED)
////            if(it->m2 < 0.6)
////                _vec.erase(it--);
//    }
    _vec.erase(std::remove_if(_vec.begin(), _vec.end(), [_vec, aFactor] (VarForTemplateMatch aItem) -> bool {
                   return aItem.m2 < aFactor;
               }), _vec.end());
//    qDebug() << QString::number(_vec[0].m1, 'f', 2) << QString::number(_vec[0].m2, 'f', 2);
    double diffCoeff = aFactor;
    if(!_vec.empty())
        aFactor = _vec.front().m2;
//    if(_vec.size() == 2) {
//        return _vec[0].m2 > _vec[1].m2 ? _vec[0].p2 : _vec[0].p2;
//    } else
    if(_vec.size() == 1 && _vec[0].m2 >  diffCoeff) {
//        qDebug() << QString::number(_vec[0].m1, 'f', 2) << QString::number(_vec[0].m2, 'f', 2);
        return _vec[0].p2;
    } else {
        return cv::Point(0,0);
    }



//    if(_vec[0].matchP1 > _vec[0].matchP2) {
//        if(_vec[0].matchP1 > 0)
//            return _vec[0].p1;
//    } else {
//        if(_vec[0].matchP2 > 0)
//            return _vec[0].p2;
//    }

    //    return cv::Point(0,0);
}

QString CaptureWindow::getStrStaticField(std::string asImageROI)
{
    cv::Rect rect = mp_dataSet->at(asImageROI).rect;
    cv::Mat dst;
    win(rect).copyTo(dst);
    cv::Mat mask;
    cv::inRange(dst, cv::Scalar(0, 0, 50), cv::Scalar(255, 255, 255), mask);
    cv::bitwise_not(mask, mask);
    myOCREng->SetImage( (uchar*)mask.data, mask.size().width, mask.size().height, mask.channels(), mask.step1());
    myOCREng->Recognize(nullptr);
    QString sText = myOCREng->GetUTF8Text();
    if(sText.isEmpty())
        sText = "not recognized";
    sText = sText.simplified();
    sText = sText.toLower();
    deleteCharExtra(sText);
    qDebug() << sText;
    cv::imshow("win5", mask);
    cv::imshow("win4", dst);
    return sText;
}

QString CaptureWindow::getTextStaticField(std::string asImageROI, cv::Scalar aMinScalar, cv::Scalar aMaxScalar, QString asLang)
{
    cv::Rect rect = mp_dataSet->at(asImageROI).rect;
    cv::Mat dst;
    cv::Mat mask;
    win(rect).copyTo(dst);
    cv::inRange(dst, aMinScalar, aMaxScalar, mask);
    int tempBkack, tmpWhite;
    if(!blackLessWhite(mask, tempBkack, tmpWhite))
        cv::bitwise_not(mask, mask);
    QString sText;
    if(asLang == "ru") {
        myOCRRus->SetImage( (uchar*)mask.data, mask.size().width, mask.size().height, mask.channels(), mask.step1());
        myOCRRus->Recognize(nullptr);
        sText = myOCRRus->GetUTF8Text();
    } else {
        myOCREng->SetImage( (uchar*)mask.data, mask.size().width, mask.size().height, mask.channels(), mask.step1());
        myOCREng->Recognize(nullptr);
        sText = myOCREng->GetUTF8Text();
    }
    if(sText.isEmpty())
        sText = "not recognized";
    sText = sText.simplified();
    sText = sText.toLower();
    deleteCharExtra(sText);
    qDebug() << sText;
    cv::imshow("win5", mask);
    cv::imshow("win4", dst);
    return sText;
}

QString CaptureWindow::getTextApproximArea(cv::Rect aRect, cv::Point &aPoint, QString asLang)
{
    cv::Rect rect = aRect;
    cv::Mat dst;
    cv::Mat mask;
    cv::Mat edge;
    win(rect).copyTo(dst);

    std::vector< std::vector< cv::Point> > contours2;

    cv::Canny(dst, edge, 10, 20);
//    cv::Canny(dst, edge, minNumber, maxNumber);
//    cv::drawContours(dst, contours2, -1, cvBlue);


    cv::findContours(edge, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    std::vector<size_t> idx;
    std::vector<double> areas;
    idx.resize(contours2.size());
    areas.resize(contours2.size());
    for(size_t i = 0; i < contours2.size(); i++) {
        idx[i] = i;
        areas[i] = cv::contourArea(contours2[i], false);
    }
    std::sort(idx.begin(), idx.end(), AreaCmp(areas));
    if(contours2.empty()) {
        return QString();
    }
    cv::Rect foundRect = cv::boundingRect(contours2[idx[0]]);
//    cv::rectangle(dst, foundRect, cvBlue, 1);
    cv::Mat recognizMat;
    dst(foundRect).copyTo(recognizMat);
    aPoint.x = rect.x + (recognizMat.size().width / 2);
    aPoint.y = rect.y + (recognizMat.size().height / 2);


    cv::Mat gray;
    cv::Mat recognizMask;
    cv::cvtColor(recognizMat, gray, CV_BGR2GRAY);
    cv::threshold(gray, mask, 76, 153, cv::THRESH_BINARY);
//    cv::adaptiveThreshold(gray, mask, 76, 153, cv::THRESH_BINARY);
    cv::inRange(mask, cv::Scalar(0, 0, 0), cv::Scalar(1, 1, 1), recognizMask);
//    cv::inRange(mask, minScalar, maxScalar, recognizMask);


    QString sText;
    if(asLang == "ru") {
        myOCRRus->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
        myOCRRus->Recognize(nullptr);
        sText = myOCRRus->GetUTF8Text();
    } else {
        myOCREng->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
        myOCREng->Recognize(nullptr);
        sText = myOCREng->GetUTF8Text();
    }
    if(sText.isEmpty())
        sText = "not recognized";
    sText = sText.simplified();
    sText = sText.toLower();
    deleteCharExtra(sText);
    qDebug() << sText;
    cv::imshow("win2", edge);
    cv::imshow("win3", mask);
    cv::imshow("win5", recognizMask);
    cv::imshow("win4", gray);
    return sText;
}

QString CaptureWindow::getTextArea(cv::Rect aRect, cv::Point &aPoint, QString asLang)
{
    cv::Rect rect = aRect;
    cv::Mat dst;
    cv::Mat mask;
    cv::Mat edge;
    win(rect).copyTo(dst);

//    std::vector< std::vector< cv::Point> > contours2;

//    cv::Canny(dst, edge, 10, 20);
////    cv::Canny(dst, edge, minNumber, maxNumber);
////    cv::drawContours(dst, contours2, -1, cvBlue);


//    cv::findContours(edge, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
//    std::vector<size_t> idx;
//    std::vector<double> areas;
//    idx.resize(contours2.size());
//    areas.resize(contours2.size());
//    for(size_t i = 0; i < contours2.size(); i++) {
//        idx[i] = i;
//        areas[i] = cv::contourArea(contours2[i], false);
//    }
//    std::sort(idx.begin(), idx.end(), AreaCmp(areas));
//    if(contours2.empty()) {
//        return QString();
//    }
//    cv::Rect foundRect = cv::boundingRect(contours2[idx[0]]);
//    cv::rectangle(dst, foundRect, cvBlue, 1);
    aPoint.x = rect.x + (dst.size().width / 2);
    aPoint.y = rect.y + (dst.size().height / 2);


    cv::Mat gray;
    cv::Mat recognizMask;
    cv::cvtColor(dst, gray, CV_BGR2GRAY);
    cv::threshold(gray, mask, 76, 153, cv::THRESH_BINARY);
//    cv::adaptiveThreshold(gray, mask, 76, 153, cv::THRESH_BINARY);
    cv::inRange(mask, cv::Scalar(0, 0, 0), cv::Scalar(1, 1, 1), recognizMask);
//    cv::inRange(mask, minScalar, maxScalar, recognizMask);


    QString sText;
    if(asLang == "ru") {
        myOCRRus->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
        myOCRRus->Recognize(nullptr);
        sText = myOCRRus->GetUTF8Text();
    } else {
        myOCREng->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
        myOCREng->Recognize(nullptr);
        sText = myOCREng->GetUTF8Text();
    }
    if(sText.isEmpty())
        sText = "not recognized";
    sText = sText.simplified();
    sText = sText.toLower();
    deleteCharExtra(sText);
    qDebug() << "text recognize: " << sText;
//    cv::imshow("win2", edge);
    cv::imshow("win3", mask);
    cv::imshow("win5", recognizMask);
    cv::imshow("win4", gray);
    return sText;
}

QString CaptureWindow::getTextMat(cv::Mat aMat, cv::Point &aPoint, QString asLang)
{
    cv::Mat dst = aMat;
    cv::Mat mask;


    aPoint.x = (dst.size().width / 2);
    aPoint.y = (dst.size().height / 2);


    cv::Mat gray;
    cv::Mat recognizMask;
    cv::cvtColor(dst, gray, CV_BGR2GRAY);
    cv::threshold(gray, mask, 76, 153, cv::THRESH_BINARY);
//    cv::adaptiveThreshold(gray, mask, 76, 153, cv::THRESH_BINARY);
    cv::inRange(mask, cv::Scalar(0, 0, 0), cv::Scalar(1, 1, 1), recognizMask);
//    cv::inRange(mask, minScalar, maxScalar, recognizMask);

    int black, white;
    if(!blackLessWhite(recognizMask, black, white)) {
        cv::bitwise_not(recognizMask, recognizMask);
    }

    QString sText;
    if(asLang == "ru") {
        myOCRRus->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
        myOCRRus->Recognize(nullptr);
        sText = myOCRRus->GetUTF8Text();
    } else {
        myOCREng->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
        myOCREng->Recognize(nullptr);
        sText = myOCREng->GetUTF8Text();
    }
    if(sText.isEmpty())
        sText = "not recognized";
    sText = sText.simplified();
    sText = sText.toLower();
    deleteCharExtra(sText);
//    qDebug() << "text recognize: " << sText;
//    cv::imshow("win2", edge);
//    cv::imshow("win3", mask);
    cv::imshow("win5", recognizMask);
//    cv::imshow("win4", gray);
    return sText;
}

QString CaptureWindow::getTextApproximBoundingRect(cv::Rect aRect, cv::Point &aPoint, QString asLang)
{
    cv::Rect rect = aRect;
    cv::Mat dst;
    cv::Mat mask;
    cv::Mat edgeMat;
    win(rect).copyTo(dst);


    cv::Canny(dst, edgeMat, 10, 20);
//    cv::Canny(dst, edge, minNumber, maxNumber);
//    cv::drawContours(dst, contours2, -1, cvBlue);


    std::vector< std::vector< cv::Point> > contours2;
    cv::findContours(edgeMat, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    std::vector<size_t> idx;
    std::vector<double> areas;
    idx.resize(contours2.size());
    areas.resize(contours2.size());

    for(size_t i = 0; i < contours2.size(); i++) {

        idx[i] = i;
        areas[i] = cv::boundingRect(contours2[i]).area();
//        areas[i] = cv::contourArea(contours2[i], false);
    }
    std::sort(idx.begin(), idx.end(), AreaCmp(areas));
    if(contours2.empty()) {
        return QString();
    }
    cv::Rect foundRect = cv::boundingRect(contours2[idx[0]]);
//    cv::rectangle(dst, foundRect, cvBlue, 1);
    cv::Mat recognizMat;
    dst(foundRect).copyTo(recognizMat);
    aPoint.x = rect.x + (recognizMat.size().width / 2);
    aPoint.y = rect.y + (recognizMat.size().height / 2);


    cv::Mat gray;
    cv::Mat recognizMask;
    cv::cvtColor(recognizMat, gray, CV_BGR2GRAY);
    cv::threshold(gray, mask, 76, 153, cv::THRESH_BINARY);

//    cv::adaptiveThreshold(gray, mask, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, midNumber, 0);

    cv::inRange(mask, cv::Scalar(0, 0, 0), cv::Scalar(1, 1, 1), recognizMask);
//    cv::inRange(mask, minScalar, maxScalar, recognizMask);


    QString sText;
    if(asLang == "ru") {
        myOCRRus->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
        myOCRRus->Recognize(nullptr);
        sText = myOCRRus->GetUTF8Text();
    } else {
        myOCREng->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
        myOCREng->Recognize(nullptr);
        sText = myOCREng->GetUTF8Text();
    }
    if(sText.isEmpty())
        sText = "not recognized";
    sText = sText.simplified();
    sText = sText.toLower();
//    deleteCharExtra(sText);
    qDebug() << "getTextApproximBoundingRect" << sText;
//    emit signalSaveImageForDebug(dst,  QString("dst-") + QDateTime::currentDateTime().toString("mmsszz"));
//    emit signalSaveImageForDebug(recognizMask, QString("recoginezMask-") + QDateTime::currentDateTime().toString("mmsszz"));
//    emit signalSaveImageForDebug(edgeMat,  QString("edgeMat-") + QDateTime::currentDateTime().toString("mmsszz"));
//    emit signalSaveImageForDebug(mask,  QString("mask-") + QDateTime::currentDateTime().toString("mmsszz"));
    cv::imshow("win2", edgeMat);
    cv::imshow("win3", mask);
    cv::imshow("win5", recognizMask);
    cv::imshow("win4", gray);
    return sText;
}

bool CaptureWindow::containTextApproximBoundingRect(cv::Rect aRect, cv::Point &aPoint, QString asTextSrch, QString asLang, int abApproxim)
{

    cv::Rect rect = aRect;
    cv::Mat dst;
    cv::Mat mask;
    cv::Mat edgeMat;
    win(rect).copyTo(dst);


    cv::Canny(dst, edgeMat, 10, 20);
//    cv::Canny(dst, edge, minNumber, maxNumber);
//    cv::drawContours(dst, contours2, -1, cvBlue);


    std::vector< std::vector< cv::Point> > contours2;
    cv::findContours(edgeMat, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    std::vector<size_t> idx;
    std::vector<double> areas;
    idx.resize(contours2.size());
    areas.resize(contours2.size());

    for(size_t i = 0; i < contours2.size(); i++) {

        idx[i] = i;
        areas[i] = cv::boundingRect(contours2[i]).area();
//        areas[i] = cv::contourArea(contours2[i], false);
    }
    std::sort(idx.begin(), idx.end(), AreaCmp(areas));
    if(contours2.empty()) {
        return false;
    }
    cv::Rect foundRect = cv::boundingRect(contours2[idx[0]]);
//    cv::rectangle(dst, foundRect, cvBlue, 1);
    cv::Mat foundMat;
    dst(foundRect).copyTo(foundMat);
    aPoint.x = rect.x + (foundMat.size().width / 2);
    aPoint.y = rect.y + (foundMat.size().height / 2);



    cv::Mat gray;
    cv::Mat recognizMask;
    cv::cvtColor(foundMat, gray, CV_BGR2GRAY);

    QString sText;
    int minVal1 = 70;
    bool ret = false;
    while(minVal1 < 256 && !ret) {
//        cv::threshold(gray, mask, minNumber, maxNumber, cv::THRESH_BINARY);
        cv::threshold(gray, mask, minVal1, 153, cv::THRESH_BINARY);
    //    cv::adaptiveThreshold(gray, mask, 76, 153, cv::THRESH_BINARY);
        cv::inRange(mask, cv::Scalar(0, 0, 0), cv::Scalar(1, 1, 1), recognizMask);
    //    cv::inRange(mask, minScalar, maxScalar, recognizMask);

        int black, white;
        if(!blackLessWhite(recognizMask, black, white)) {
            cv::bitwise_not(recognizMask, recognizMask);
        }

        if(asLang == "ru") {
            myOCRRus->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
            myOCRRus->Recognize(nullptr);
            sText = myOCRRus->GetUTF8Text();
        } else {
            myOCREng->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
            myOCREng->Recognize(nullptr);
            sText = myOCREng->GetUTF8Text();
        }
        qDebug() << "source text recognize =" << sText;
        minVal1 += 70;
        sText = sText.simplified();
        sText = sText.toLower();
        deleteCharExtra(sText);
        if(abApproxim == 0) {
            if(sText.contains(asTextSrch))
                ret = true;
        } else {
            if(comparisonStr(sText, asTextSrch) <= abApproxim)
                ret = true;
        }
    }


//    qDebug() << "minVal1 =" << minVal1 << "text: " << sText;
//    cv::imshow("win3", mask);
    cv::imshow("win5", recognizMask);
    cv::imshow("win4", dst);
    return ret;
}

bool CaptureWindow::containTextApproximBoundingMat(cv::Mat aMat, QString asTextSrch, QString asLang, int abApproxim)
{
//    cv::Rect rect; // = aRect;
    cv::Mat dst;
    dst = aMat;
    cv::Mat mask;
    cv::Mat edgeMat;
//    win(rect).copyTo(dst);


    cv::Canny(dst, edgeMat, 10, 20);
//    cv::Canny(dst, edge, minNumber, maxNumber);
//    cv::drawContours(dst, contours2, -1, cvBlue);


    std::vector< std::vector< cv::Point> > contours2;
    cv::findContours(edgeMat, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    std::vector<size_t> idx;
    std::vector<double> areas;
    idx.resize(contours2.size());
    areas.resize(contours2.size());

    for(size_t i = 0; i < contours2.size(); i++) {

        idx[i] = i;
        areas[i] = cv::boundingRect(contours2[i]).area();
//        areas[i] = cv::contourArea(contours2[i], false);
    }
    std::sort(idx.begin(), idx.end(), AreaCmp(areas));
    if(contours2.empty()) {
        return false;
    }
    cv::Rect foundRect = cv::boundingRect(contours2[idx[0]]);
//    cv::rectangle(dst, foundRect, cvBlue, 1);
    cv::Mat foundMat;
    dst(foundRect).copyTo(foundMat);
//    aPoint.x = rect.x + (foundMat.size().width / 2);
//    aPoint.y = rect.y + (foundMat.size().height / 2);



    cv::Mat gray;
    cv::Mat recognizMask;
    cv::imshow("win1", foundMat);
    cv::cvtColor(foundMat, gray, CV_BGR2GRAY);

    QString sText;
    int minVal1 = 70;
    bool ret = false;
    while(minVal1 < 256 && !ret) {
//        cv::threshold(gray, mask, minNumber, maxNumber, cv::THRESH_BINARY);
        cv::threshold(gray, mask, minVal1, 153, cv::THRESH_BINARY);
    //    cv::adaptiveThreshold(gray, mask, 76, 153, cv::THRESH_BINARY);
        cv::inRange(mask, cv::Scalar(0, 0, 0), cv::Scalar(1, 1, 1), recognizMask);
    //    cv::inRange(mask, minScalar, maxScalar, recognizMask);

        int black, white;
        if(!blackLessWhite(recognizMask, black, white)) {
            cv::bitwise_not(recognizMask, recognizMask);
        }

        if(asLang == "ru") {
            myOCRRus->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
            myOCRRus->Recognize(nullptr);
            sText = myOCRRus->GetUTF8Text();
        } else {
            myOCREng->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
            myOCREng->Recognize(nullptr);
            sText = myOCREng->GetUTF8Text();
        }
        qDebug() << "source text recognize =" << sText;
        minVal1 += 70;
        sText = sText.simplified();
        sText = sText.toLower();
        deleteCharExtra(sText);
        if(abApproxim == 0) {
            if(sText.contains(asTextSrch))
                ret = true;
        } else {
            if(sText.contains(asTextSrch))
                ret = true;
            else if(comparisonStr(sText, asTextSrch) <= abApproxim)
                ret = true;
        }
    }


//    qDebug() << "minVal1 =" << minVal1 << "text: " << sText;
//    cv::imshow("win3", mask);
    cv::imshow("win5", recognizMask);
    cv::imshow("win4", dst);
    return ret;
}

bool CaptureWindow::containTextMat(cv::Rect aRect, cv::Point &aPoint, QString asTextSrch, QString asLang, int abApproxim)
{
    asTextSrch = asTextSrch.simplified();
    asTextSrch = asTextSrch.toLower();
    deleteCharExtra(asTextSrch);

    cv::Mat dst;
    win(aRect).copyTo(dst);
    cv::Mat mask;


    aPoint.x = (dst.size().width / 2);
    aPoint.y = (dst.size().height / 2);


    cv::Mat gray;
    cv::Mat recognizMask;
    cv::cvtColor(dst, gray, CV_BGR2GRAY);

    QString sText;
    int minVal1 = 70;
    bool ret = false;
    qDebug() << "asTextSrch =" << asTextSrch;
    while(minVal1 < 256 && !ret) {
//        cv::threshold(gray, mask, minNumber, maxNumber, cv::THRESH_BINARY);
        cv::threshold(gray, mask, minVal1, 153, cv::THRESH_BINARY);
    //    cv::adaptiveThreshold(gray, mask, 76, 153, cv::THRESH_BINARY);
        cv::inRange(mask, cv::Scalar(0, 0, 0), cv::Scalar(1, 1, 1), recognizMask);
    //    cv::inRange(mask, minScalar, maxScalar, recognizMask);

        int black, white;
        if(!blackLessWhite(recognizMask, black, white)) {
            cv::bitwise_not(recognizMask, recognizMask);
        }


        cv::imshow("win5", recognizMask);
        QApplication::processEvents();
//        QThread::msleep(1000);
        if(asLang == "ru") {
            myOCRRus->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
            myOCRRus->Recognize(nullptr);
            sText = myOCRRus->GetUTF8Text();
        } else {
            myOCREng->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
            myOCREng->Recognize(nullptr);
            sText = myOCREng->GetUTF8Text();
        }
        minVal1 += 15;
        qDebug() << "1>>>" << sText;
        sText = sText.simplified();
        sText = sText.toLower();
        deleteCharExtra(sText);
//        qDebug() << "2>>>" << sText;
        if(abApproxim == 0) {
            if(sText.contains(asTextSrch))
                ret = true;
        } else {
            if(comparisonStr(sText, asTextSrch) <= abApproxim)
                ret = true;
            if(sText.contains(asTextSrch))
                ret = true;
        }
    }

    if(sText.isEmpty())
        sText = "not recognized";
    qDebug() << "containTextMat minVal1 =" << minVal1 << "text: " << sText;
//    cv::imshow("win3", mask);
    cv::imshow("win5", recognizMask);
//    cv::imshow("win4", gray);
    return ret;
}

bool CaptureWindow::imageExpectedCloseAutoPilot(std::string asImageROI, double &coeff, int &ret, int anCount, int anStart, int anEnd)
{
    if(DEBUG2)
        qDebug() << ".";
    ret = 0;
    cv::Rect fieldSpace(calcRectFromPartOfIndex(anCount, anStart, anEnd));
    cv::Mat dst;
    win(fieldSpace).copyTo(dst);
    cv::Mat maskBlur;
    cv::Mat mask(dst.size(), CV_8U, 1);
    cv::Mat hsv;
    int targetMinVal1 = 250;
    int targetMaxVal3 = 200;
    bool check = false;
    double factor = 0.0;

    while(!check && targetMinVal1 >= 50 && targetMaxVal3 >= 40) {
        cv::inRange(dst, cv::Scalar(targetMinVal1, 0, 0), cv::Scalar(255, 255, targetMaxVal3), mask);
//        cv::inRange(dst, cv::Scalar(minNumber, 0, 0), cv::Scalar(255, 255, maxNumber), mask);
        cv::medianBlur(mask, maskBlur, 3);
        cv::Mat matAreaAutoPilot;
        cv::cvtColor(maskBlur, matAreaAutoPilot, cv::COLOR_GRAY2BGR);
        factor = 0;
        if(srchAreaOnceInMat(asImageROI, matAreaAutoPilot, factor)) {
            if(factor > coeff)
                check = true;
        }
        targetMaxVal3 -= 15;
        if(targetMaxVal3 < 60) {
            targetMaxVal3 = 200;
            targetMinVal1 -= 30;
        }
    }
    qDebug() << QString::number(factor, 'f', 2) << QString::number(coeff, 'f', 2) << targetMinVal1 << targetMaxVal3 << check;
    imshow("win4", maskBlur);
    coeff = factor;
    if(!check) {
        double factor2 = 0;
        QPoint _p;
        srchAreaOnceInRect("pic_detectIntercept", factor2, _p, 10, 17, 28);
        qDebug() << "factor2 =" << QString::number(factor2, 'f', 2);
        if(factor2 > 0.8) {
            ret = 1;
        }
    }
    return check;
}

CursorTarget *CaptureWindow::takeAimp()
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Rect fieldSpace(calcRectFromPartOfIndex(11, 25, 84));
    cv::Mat dst;
    win(fieldSpace).copyTo(dst);
    m_cursorTarget.active = false;
    m_cursorTarget.rectSrc = fieldSpace;
    m_cursorTarget.pointCenter = cv::Point(fieldSpace.width / 2, fieldSpace.height / 2 + 15);
    cv::Mat mask(dst.size(), CV_8U, 1);
    cv::Mat hsv;
//    cv::cvtColor(dst, hsv, CV_BGR2HSV);
//    int targetSatMax = 246;
//    int targetValMax = 253;
    int targetVal2 = 240;
    int targetVal3 = 240;
//    while(!m_cursorTarget.active && targetSatMax <= 255 && targetValMax <= 255) {
    while(!m_cursorTarget.active && targetVal2 >= 100 && targetVal3 >= 100) {
        //    cv::inRange(hsv, minScalar, maxScalar, mask);                         // hsv
//        cv::inRange(hsv, cv::Scalar(15, 43, 117), cv::Scalar(48, targetSatMax, targetValMax), mask);                         // hsv
//        cv::inRange(dst, minScalar, maxScalar, mask);                         // hsv
        cv::inRange(dst, cv::Scalar(0, targetVal2, targetVal3), cv::Scalar(255, 255, 255), mask);                         // hsv
        //    cv::Mat maskBlur;
        //    cv::blur(mask, maskBlur, cv::Size(3, 3));
            std::vector< std::vector< cv::Point> > contours2Src;
            std::vector< cv::Vec4i > hierarchy;
            cv::findContours(mask, contours2Src, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

            std::vector<std::vector< cv::Point > > contours2Prep;
            for(size_t j = 0; j < contours2Src.size(); j++) {
                double area = cv::contourArea(contours2Src[j]);
                if(area > 50 && area < 200)
                    contours2Prep.push_back(contours2Src[j]);
            }
//            double area = cv::contourArea(contours2Src[j]);
//            if(area > 80.1)
//                contours2Prep.push_back(contours2Src[j]);
//                qDebug() << "area =" << QString::number(area, 'f', 2);



            std::vector<cv::Rect> vecRects;
//            getRectsInContour(contours2Src, vecRects);
            getRectsInContour(contours2Prep, vecRects);

        #define TARGET_DELTA            10
        #define TARGET_HEIGHT_MIN       95
        #define TARGET_HEIGHT_MAX       146
        #define TARGET_WIDTH_MIN        95
        #define TARGET_WIDTH_MAX        218
            for(size_t i = 0; i < vecRects.size(); i++) {
                if(vecRects[i].height < TARGET_HEIGHT_MIN - TARGET_DELTA || vecRects[i].height > TARGET_HEIGHT_MAX + TARGET_DELTA
                        || vecRects[i].width < TARGET_WIDTH_MIN - TARGET_DELTA || vecRects[i].width > TARGET_WIDTH_MAX + TARGET_DELTA)
                    continue;
                if(m_cursorTarget.active) {
                    qDebug() << "Внимание Обнаружено несколько таргетов";
                }
                m_cursorTarget.active = true;
                m_cursorTarget.rectTarget = cv::Rect(vecRects[i].x, vecRects[i].y, vecRects[i].height, vecRects[i].height);
                m_cursorTarget.pointTarget = cv::Point(m_cursorTarget.rectTarget.x + m_cursorTarget.rectTarget.width / 2,
                                                       m_cursorTarget.rectTarget.y + m_cursorTarget.rectTarget.height / 2);
                cv::rectangle(dst, m_cursorTarget.rectTarget, cv::Scalar(0xFF, 0x0, 0x0));
            }
//            qDebug() << cv::contourArea()
            targetVal2 -= 15;
            if(targetVal2 <= 100) {
                targetVal2 = 240;
                targetVal3 -= 15;
            }
//            targetSatMax++;
//            if(targetSatMax == 255) {
//                targetValMax++;
//                targetSatMax = 246;
//            }
    }
//    qDebug() << "targetSatMax =" << targetSatMax << " targetValMax =" << targetValMax;
//    qDebug() << targetVal2 << targetVal3;
    cv::circle(dst, m_cursorTarget.pointCenter, 3, cv::Scalar(0, 255, 0));
//    cv::circle(dst, m_cursorTarget.pointTarget, 3, cv::Scalar(0, 255, 0));



//    imshow("win4", maskBlur);
    imshow("win2", mask);
    imshow("win3", dst);
    return &m_cursorTarget;
}

Primitives *CaptureWindow::getPrimitives(int aSide)
{
    if(DEBUG2)
        qDebug() << ".";
    primitives.found = false;
    cv::Mat dst;
    cv::Mat mask(dst.size(), CV_8U, 1);
    cv::Mat hsv;
    cv::Rect fieldSpace;
    if(aSide == 0) {
        fieldSpace = cv::Rect(calcRectFromPartOfIndex(11, 25, 84));
    } else if(aSide == 3) { // right
        fieldSpace = cv::Rect(calcRectFromPartOfIndex(11, 28, 87));
    } else if(aSide == 2) { // top
        fieldSpace = cv::Rect(calcRectFromPartOfIndex(11, 3, 62));
    } else if(aSide == 1) { // left
        fieldSpace = cv::Rect(calcRectFromPartOfIndex(11, 22, 81));
    }
    fieldSpace.height = fieldSpace.height - 10;
    enum {HOT_STAR} barrier = HOT_STAR;

    for(int k = 0; k < 1; k++) {
        if(k == barrier) {
            win(fieldSpace).copyTo(dst);
        //    m_cursorTarget.active = false;
        //    m_cursorTarget.rectSrc = fieldSpace;
        //    m_cursorTarget.pointCenter = cv::Point(fieldSpace.width / 2, fieldSpace.height / 2);
        //    cv::circle(dst, m_cursorTarget.pointCenter, 3, cv::Scalar(0, 255, 0));
        //    cv::cvtColor(dst, hsv, CV_BGR2HSV);
        //    cv::inRange(hsv, minScalar, maxScalar, mask);                         // hsv

            std::vector<cv::Rect> rects;
//            cv::inRange(dst, minScalar, cv::Scalar(30, 255, 255), mask);
            cv::inRange(dst, cv::Scalar(0, 0, 0), cv::Scalar(35, 255, 255), mask);
            cv::bitwise_not(mask, mask);
//            cv::inRange(dst, minScalar, maxScalar, mask);
            //            cv::Mat maskBlur;
            //            cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0, 0);
            cv::medianBlur(mask, mask, 7);
            std::vector< std::vector< cv::Point> > contours2;
            cv::findContours(mask, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
            if(contours2.empty()) {
                qDebug() << "Нет объектов";
                return &primitives;
            }
            cv::drawContours(dst, contours2, -1, cvBlue);
            std::vector<size_t> idx;
            std::vector<double> areas;
            areas.resize(contours2.size());
            idx.resize(contours2.size());
            for(size_t i = 0; i < contours2.size(); i++) {
                areas[i] = cv::contourArea(contours2[i]);
                idx[i] = i;

            }

            std::sort(idx.begin(), idx.end(), AreaCmp(areas));
            if(areas[idx.front()] > 25000) {
                cv::Rect rect = cv::boundingRect(contours2[idx.front()]);
//                    qDebug() << "area =" << QString::number(area, 'f', 2) << rect.width << rect.height;
                primitives.found = true;
                primitives.area = areas[idx.front()];
                primitives.contour = contours2[idx.front()];
                primitives.rect = cv::boundingRect(contours2[idx.front()]);
                primitives.screen = fieldSpace;

                cv::rectangle(dst, rect, cvRad);
            }
        }

    }



//    imshow("win4", maskBlur);
    imshow("win3", mask);
    imshow("win2", dst);
    return &primitives;
}

Compass *CaptureWindow::compass()
{
    static int count_error = 0;
    if(DEBUG2)
        qDebug() << ".";
#define     COMPAS_SIDE     90
    m_compas.active = false;
    cv::Rect compasRect;        // Основной квадрат компаса
    cv::Mat dst;
    cv::Rect fieldCompass(calcRectFromPartOfIndex(26, 476, 557));
    fieldCompass.y = fieldCompass.y - 15;
    fieldCompass.height = fieldCompass.height + 35;
    fieldCompass.x = fieldCompass.x - 55;
    fieldCompass.width = fieldCompass.width + 115;
    win(fieldCompass).copyTo(dst);


//    cv::Mat testMask;
//    int testVal = 240;
//    while(testVal <= 255) {
////        cv::inRange(dst, cv::Scalar(testVal, testVal, testVal), cv::Scalar(255, 255, 255), testMask);
//        cv::inRange(dst, cv::Scalar(minNumber, minNumber, minNumber), cv::Scalar(255, 255, 255), testMask);

//        cv::imshow("win3", testMask);
//        cv::imshow("win2", dst);
//        QApplication::processEvents();
//        QThread::msleep(300);
//        testVal += 10;
//    }
//    return &m_compas;




    //  ---------------------------       Поиск кругов       ----------------------

    cv::Mat bin(dst.size(), CV_8U, 1);
    cv::Mat img_blur;
#define HUE_FOR_COMAPS_CIRCLE   17
    int hue = HUE_FOR_COMAPS_CIRCLE;
    int minVal2 = 150;
    int minVal3 = 250;
    std::vector<cv::Vec3f> circles;
    std::vector<cv::Rect> compassRects;
//    qDebug() << "******* поиск кругов *********";

    while (minVal3 > 50) {
        circles.clear();
        compassRects.clear();
        bin.deallocate();
        img_blur.deallocate();
//        getMaskOfMat(dst, bin, cv::Scalar(hue, 150, 126), maxScalar);
//        getMaskOfMatHSV(dst, bin, cv::Scalar(hue, 127, 126), cv::Scalar(27, 250, 254));
        getMaskOfMatHSV(dst, bin, cv::Scalar(hue, minVal2, minVal3), cv::Scalar(27, 250, 254));
//        getMaskOfMat(dst, bin, cv::Scalar(hue, minNumber, maxNumber), maxScalar);
        std::vector< std::vector <cv::Point> > cont;
        std::vector<int > sort_idx;
        cv::blur(bin, img_blur, cv::Size(3, 3));


//        imshow("win5", img_blur);
//        QApplication::processEvents();
//        QThread::msleep(70);


//        cv::GaussianBlur(bin, img_blur, cv::Size(9, 9), 2, 2);
        cv::HoughCircles(img_blur, circles, cv::HOUGH_GRADIENT, 1, dst.rows / 1, 200, 15, 28, 30);
        if(circles.empty())  {
//            qDebug() << " hue =" << hue;
//            imshow("win5", dst);
//            imshow("win4", img_blur);
        } else {
            for(size_t i = 0; i < circles.size(); i++) {
                cv::Rect rectFound(cvRound(circles[i][0]) - COMPAS_SIDE/2, cvRound(circles[i][1]) - COMPAS_SIDE/2, COMPAS_SIDE, COMPAS_SIDE);
                if(rectFound.x <= 0 || rectFound.y <= 0
                        || rectFound.x + rectFound.width >= dst.size().width
                        || rectFound.y + rectFound.height >= dst.size().height) {
//                    qDebug() << "noise либо на границе либо по размерам не подходит";
                    continue;
                }
//                cv::circle(dst, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), cvRound(circles[i][2]), cvGreen, 2);
                compassRects.push_back(rectFound);
//                cv::rectangle(dst, rectFound, cv::Scalar(255, 0, 0), 2);
            }
            if(!compassRects.empty())
                break;
        }
        hue-=2;
        if(hue < 5) {
            minVal2 -= 25;
            hue = HUE_FOR_COMAPS_CIRCLE;
            if(minVal2 < 70) {
                minVal3 -= 25;
                minVal2 = 150;
            }
        }
    }
//    qDebug() << "hue = " << hue << " compassRects =" << compassRects.size();
    if(compassRects.empty()) {
        imshow("win3", dst);
        imshow("win5", img_blur);
        count_error++;
        qDebug() << "Круги компаса не найдены hue =" << hue << " minVal2 =" << minVal2 << " minVal3 =" << minVal3 <<  " count_error =" << count_error;
        return &m_compas;
    }
//    qDebug() << "hue =" << hue << " minVal2 =" << minVal2 ;

//    imshow("win3", dst);
//    imshow("win4", img_blur);
//    return &m_compas;



    //**********************       Выравнивание компаса      **************************
//    qDebug() << "******* Выравнивание компаса *********";
    compasRect = compassRects.front();
    cv::Point centerCompass;

    cv::Mat matPlus;
    cv::Mat maskPlus;
    /*
    cv::Point centerPlus(compasRect.x + compasRect.width / 2, compasRect.y + compasRect.height / 2);
    dst(cv::Rect(centerPlus.x - 15, centerPlus.y - 15, 30, 30)).copyTo(matPlus);
    int valfour = 250;
    int valMax1 = 25;
//    bool check_four = false;
    std::vector<std::vector<cv::Point> > plusContours;

//    qDebug() << "centerPlus=" << centerPlus.x << centerPlus.y;

    bool foundPlus = false;
    while(valMax1 <= 100 && !foundPlus) {
        plusContours.clear();
//        cv::inRange(matPlus, cv::Scalar(0, 0, valfour), cv::Scalar(35, 255, 255), maskPlus);
        cv::inRange(matPlus, cv::Scalar(0, 0, valfour), cv::Scalar(valMax1, 255, 255), maskPlus);
        cv::findContours(maskPlus, plusContours, cv::noArray(), cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
//        imshow("win4", matPlus);
//        imshow("win5", maskPlus);
//        QApplication::processEvents();
        if(plusContours.size() > 1) {
            cv::Point p1, p2;
//            cv::Rect rectInit = cv::boundingRect(plusContours[0]);
            cv::RotatedRect rotRectInit = cv::minAreaRect(plusContours[0]);
//            p1 = cv::Point(rectInit.x, rectInit.y);
            p1 = cv::Point(rotRectInit.center.x, rotRectInit.center.y);
//            bool bX = false;
//            bool bY = false;
            for(size_t i = 1; i < plusContours.size(); i++) {
                //                cv::Rect rect_ = cv::boundingRect(plusContours[i]);
                rotRectInit = cv::minAreaRect(plusContours[i]);
                cv::Point point_ = cv::Point(rotRectInit.center.x, rotRectInit.center.y);
                if(abs(p1.x - point_.x) > 4 && abs(p1.y - point_.y) > 4) {
                    cv::Point foundP1 = cv::Point(p1.x - 15, point_.y - 15);
                    cv::Point foundP2 = cv::Point(point_.x - 15, p1.y - 15);
                    if(sqrt(pow(foundP1.x, 2) + pow(foundP1.y, 2)) < sqrt(pow(foundP2.x, 2) + pow(foundP2.y, 2))) {
                        p2 = cv::Point(foundP1.x + 15, foundP1.y + 15);
                        centerCompass = foundP1;

                    } else {
                        p2 = cv::Point(foundP2.x + 15, foundP2.y + 15);
                        centerCompass = foundP2;
                    }
                    cv::circle(matPlus, p2, 2, cvBlue, 2);
//                    centerCompass = p2;
//                    imshow("win4", matPlus);
//                    imshow("win5", maskPlus);
//                    QApplication::processEvents();
                    foundPlus = true;
                    break;
                }
                //                QThread::msleep(50);

            }
//            for(size_t i = 0; i < plusContours.size(); i++) {
////                cv::Rect rect_ = cv::boundingRect(plusContours[i]);
//                rotRectInit = cv::minAreaRect(plusContours[i]);
//                cv::Point point_ = cv::Point(rotRectInit.center.x, rotRectInit.center.y);

//                if(abs(p1.x - point_.x) > 3) {
//                    if(p1.y > point_.y) {
//                        cv::Point tmp = p1;
//                        p1 = point_;
//                        point_ = tmp;
//                    }
//                    p2.y = p1.y > (matPlus.size().height - point_.y) ? p1.y : point_.y;
//                    bX = true;
//                }
//                if(abs(p1.y - point_.y) > 3) {
//                    if(p1.x > point_.x) {
//                        cv::Point tmp = p1;
//                        p1 = point_;
//                        point_ = tmp;
//                    }
//                    p2.x = p1.x > (matPlus.size().width - point_.x) ? p1.x : point_.x;
//                    bY = true;
//                }

//            }
//            if(bX && bY) {
////                qDebug() << "p=" << p2.x << p2.y;
//                centerCompass = cv::Point(p2.x - 15, p2.y - 15);
//                cv::circle(matPlus, p2, 2, cvBlue, 2);
//                break;
//            }
        }
        valfour-= 3;
        if(valfour  <= 3) {
            valfour = 255;
            valMax1 += 5;
        }
    }
    if(!foundPlus) {
        qDebug() << "Выравнивание компаса не удалось, выходим из просчёта, valfour =" << valfour << " valMax1 =" << valMax1;
        imshow("win4", matPlus);
        imshow("win5", maskPlus);
        return &m_compas;
    }
    imshow("win4", matPlus);
    imshow("win5", maskPlus);
    return &m_compas;

    */
    centerCompass.x = 0; centerCompass.y = 0;

    compasRect = cv::Rect(compasRect.x + centerCompass.x, compasRect.y + centerCompass.y, compasRect.width, compasRect.height);




    //**********************       Поиск точки      **************************


//    qDebug() << "******* Поиск точки *********";
    cv::Mat compasMat;
    cv::Mat compasMask;
    cv::Mat compasBlueMask;
    cv::Vec3f circl;
    circl[0] = compasRect.width / 2;
    circl[1] = compasRect.height / 2;
    circl[2] = 35;

    if(compasRect.x < 0 || compasRect.y < 0 || compasRect.x + compasRect.width > dst.size().width || compasRect.y + compasRect.height > dst.size().height) {
        qDebug() << "точка скорее всего не действительна";
//        imshow("win4", matPlus);
//        imshow("win5", maskPlus);
        return &m_compas;
    }


    dst(compasRect).copyTo(compasMat);

//    imshow("win3", compasMat);
//    imshow("win4", matPlus);
//    imshow("win5", maskPlus);
//    QApplication::processEvents();
//    return &m_compas;

//    cv::rectangle(dst, compasRect, cvBlue, 2);
    bool foundDot = false;
    std::vector<cv::Point> dotContour;
//    qDebug() << "******* Начало первой стадии *********";
    int valTest = 250;
    while(valTest > 95 && !foundDot) {
        cv::inRange(compasMat, cv::Scalar(valTest, valTest, valTest), cv::Scalar(255, 255, 255), compasMask);
        std::vector <std::vector <cv::Point> > compasContours;
        std::vector<size_t> idx;
        cv::bitwise_not(compasMask, compasMask);
        cv::medianBlur(compasMask, compasBlueMask, 3);
//        if(!getContoursIndexSort(compasMask, compasContours, idx, cv::RETR_LIST)) {
        if(!getContoursIndexSort(compasBlueMask, compasContours, idx, cv::RETR_LIST)) {

        }
//        cv::Mat tmpMat = compasMat;
//        cv::drawContours(tmpMat, compasContours, -1, cvGreen);
//        imshow("win2", tmpMat);
//        imshow("win3", compasMask);
        imshow("win3", compasBlueMask);
//        QThread::msleep(150);

        QApplication::processEvents();
//        imshow("win5", compasMask);
        foundDot = false;
        dotContour.clear();
        for(size_t i = 0; i < idx.size(); i++) {
//            imshow("win4", compasMat);
//            QApplication::processEvents();
//            QThread::msleep(1);
            cv::RotatedRect rotRect = cv::minAreaRect(compasContours[idx[i]]);
//            cv::Rect dotRect = cv::boundingRect(compasContours[idx[i]]);
            double area = cv::contourArea(compasContours[idx[i]]);
            if(area < 20 || area > 350 )
                continue;

            if(rotRect.center.x < (compasMat.size().width / 2 - 4) && rotRect.center.y > (compasMat.size().height / 2 + 4) && valTest > 200 )
                circl[2] = 31;
            else
                circl[2] = 37;


            if(isPointInsideElypse(circl, rotRect.center)) {
//                qDebug() << "area =" << area;

                if(rotRect.size.width < 11 && rotRect.size.height < 11 && (rotRect.size.width > 3 || rotRect.size.height > 3)) {
//                if(dotRect.width < 25 && dotRect.height < 25 && dotRect.width > 5 && dotRect.height > 5) {
//                    cv::drawContours(compasMat, compasContours, idx[i], cvBlue);
                    dotContour = compasContours[idx[i]];
                    foundDot = true;
                    qDebug() <<  valTest;

                    break;
                }
            }
        }

        valTest -= 5;
    }
//    if(!foundDot)
//        qDebug() << "not found valTest =" << valTest;
//    std::vector <std::vector <cv::Point> > tmpvec;
//    tmpvec.push_back(dotContour);
//    cv::drawContours(compasMat, tmpvec, -1, cvBlue);
//    imshow("win4", compasMat);
//    imshow("win5", compasBlueMask);
////    QThread::msleep(1500);
//    QApplication::processEvents();
//    return &m_compas;


/*

    int valtrid = 255;
    int valtridMinTrid = 255;
    while(valtridMinTrid > 100 && !foundDot) {
//        getMaskOfMat(compasMat, compasMask, minScalar, maxScalar);
//        getMaskOfMat(compasMat, compasMask, cv::Scalar( 0 , 0 , valDot), cv::Scalar(255 , 255 , 255));
//        cv::inRange(compasMat, minScalar, maxScalar, compasMask);
//        cv::inRange(compasMat, cv::Scalar(150, valtrid, minNumber), cv::Scalar(255, 255, 255), compasMask);
        cv::inRange(compasMat, cv::Scalar(200, valtrid, valtridMinTrid), cv::Scalar(255, 255, 255), compasMask);
        std::vector <std::vector <cv::Point> > compasContours;
        std::vector<size_t> idx;
        cv::bitwise_not(compasMask, compasMask);
        cv::medianBlur(compasMask, compasBlueMask, 3);
//        if(!getContoursIndexSort(compasMask, compasContours, idx, cv::RETR_LIST)) {
        if(!getContoursIndexSort(compasBlueMask, compasContours, idx, cv::RETR_LIST)) {

        }
//        cv::Mat tmpMat = compasMat;
//        cv::drawContours(tmpMat, compasContours, -1, cvGreen);
//        imshow("win2", tmpMat);
//        imshow("win3", compasMask);
        imshow("win3", compasBlueMask);
        QThread::msleep(100);

        QApplication::processEvents();
//        imshow("win5", compasMask);
        foundDot = false;
        dotContour.clear();
        for(size_t i = 0; i < idx.size(); i++) {
//            imshow("win4", compasMat);
            QApplication::processEvents();
//            QThread::msleep(1);
            cv::RotatedRect rotRect = cv::minAreaRect(compasContours[idx[i]]);
//            cv::Rect dotRect = cv::boundingRect(compasContours[idx[i]]);
            double area = cv::contourArea(compasContours[idx[i]]);
            if(area < 30 || area > 350 )
                continue;
            if(isPointInsideElypse(circl, rotRect.center)) {
//                qDebug() << "area =" << area;

                if(rotRect.size.width < 10 && rotRect.size.height < 11 && (rotRect.size.width > 3 || rotRect.size.height > 3)) {
//                if(dotRect.width < 25 && dotRect.height < 25 && dotRect.width > 5 && dotRect.height > 5) {
//                    cv::drawContours(compasMat, compasContours, idx[i], cvBlue);
                    dotContour = compasContours[idx[i]];
                    foundDot = true;

                    break;
                }
            }
        }

        valtrid -= 15;
        if(valtrid <= 100) {
            valtrid = 255;
            valtridMinTrid -= 10;
        }
    }
//    qDebug() << "valTrid =" << valtrid << "  valtridMinTrid =" << valtridMinTrid;
//    std::vector <std::vector <cv::Point> > tmpvec;
    tmpvec.push_back(dotContour);
    cv::drawContours(compasMat, tmpvec, -1, cvBlue);
    imshow("win4", compasMat);
    imshow("win5", compasBlueMask);
    QThread::msleep(1500);
    QApplication::processEvents();
    return &m_compas;

//    qDebug() << "******* Начало второй стадии *********";

    int satDot = 0;
    while(satDot < 100 && !foundDot) {
//        getMaskOfMat(compasMat, compasMask, minScalar, maxScalar);
        getMaskOfMatHSV(compasMat, compasMask, cv::Scalar( 0 , satDot , 0), cv::Scalar(21 , 255 , 255));
//        getMaskOfMat(compasMat, compasMask, cv::Scalar( 0 , minNumber , 0), maxScalar);
        std::vector <std::vector <cv::Point> > compasContours;
        std::vector<size_t> idx;
        cv::bitwise_not(compasMask, compasMask);
        cv::medianBlur(compasMask, compasBlueMask, 3);
        if(!getContoursIndexSort(compasBlueMask, compasContours, idx)) {

        }
//        cv::drawContours(compasMat, compasContours, -1, cvGreen);
//        imshow("win2", compasMat);
//        imshow("win5", compasBlueMask);
//        imshow("win3", compasMask);
//        QApplication::processEvents();
        foundDot = false;
        dotContour.clear();
        for(size_t i = 0; i < idx.size(); i++) {
            cv::RotatedRect rotRect = cv::minAreaRect(compasContours[idx[i]]);
            double area = cv::contourArea(compasContours[idx[i]]);
            if(isPointInsideElypse(circl, rotRect.center)) {
                cv::Rect dotRect = cv::boundingRect(compasContours[idx[i]]);
                if(area > 35 && area < 300 && dotRect.width < 25 && dotRect.height < 25) {
                    dotContour = compasContours[idx[i]];
                    foundDot = true;
                    break;
                }
            }
        }
        satDot += 5;
//        QThread::msleep(100);

    }

//    qDebug() << "******* Начало третьей стадии *********";
    int valDot = 250;
    while(valDot > 125 && !foundDot) {
//        getMaskOfMat(compasMat, compasMask, minScalar, maxScalar);
//        getMaskOfMat(compasMat, compasMask, cv::Scalar( 0 , 0 , minNumber), cv::Scalar(255 , 255 , 255));
        getMaskOfMatHSV(compasMat, compasMask, cv::Scalar( 0 , 0 , valDot), cv::Scalar(255 , 255 , 255));
        std::vector <std::vector <cv::Point> > compasContours;
        std::vector<size_t> idx;
//        cv::bitwise_not(compasMask, compasMask);
        cv::medianBlur(compasMask, compasBlueMask, 3);
        if(!getContoursIndexSort(compasBlueMask, compasContours, idx, cv::RETR_LIST)) {

        }
//        cv::drawContours(compasMat, compasContours, -1, cvGreen);
//        imshow("win2", compasMat);
//        imshow("win5", compasBlueMask);
//        imshow("win3", compasMask);
//        QApplication::processEvents();

        foundDot = false;
        dotContour.clear();
        for(size_t i = 0; i < idx.size(); i++) {
            cv::RotatedRect rotRect = cv::minAreaRect(compasContours[idx[i]]);
            double area = cv::contourArea(compasContours[idx[i]]);
            if(isPointInsideElypse(circl, rotRect.center)) {
                cv::Rect dotRect = cv::boundingRect(compasContours[idx[i]]);
                if(area > 35 && area < 300 && dotRect.width < 25 && dotRect.height < 25) {
                    dotContour = compasContours[idx[i]];
                    foundDot = true;
                    break;
                }
            }
        }
        valDot -= 5;
//        QThread::msleep(100);

    }
*/

    if(!foundDot) {
//        imshow("win2", dst);
        imshow("win4", compasMat);
//        imshow("win4", compasBlueMask);
        imshow("win5", compasMask);
//        qDebug() << "Контуры точки не найдены sat =" << satDot << " val =" << valDot << " valtrid =" << valtrid;
        qDebug() << "Контуры точки не найдены valTest =" << valTest;
        return &m_compas;
    }


    cv::RotatedRect dotRot = cv::minAreaRect(dotContour);
    cv::Rect dotRect = cv::boundingRect(dotContour);
    if(dotRect.width < 10) {
        dotRect.x = dotRect.x - 4;
        dotRect.width = dotRect.width +8;
    }
    if(dotRect.height < 10) {
        dotRect.y = dotRect.y - 4;
        dotRect.height = dotRect.height +8;
    }
    cv::Mat dotMat;
    compasMat(dotRect).copyTo(dotMat);
    cv::circle(compasMat, dotRot.center, 1, cvBlue, 2);         // рисуем точку на компасе
    m_compas.pDot = dotRot.center;
    m_compas.pCenter = cv::Point(compasRect.width/2, compasRect.height/2);

    cv::Mat dotMask;
//    getMaskOfMatHSV(dotMat, dotMask, cv::Scalar( 0 , 75 , 0), cv::Scalar(27 , 255 , 255));
//    getMaskOfMatHSV(dotMat, dotMask, cv::Scalar( 0 , 75 , 0), cv::Scalar(27 , 255 , 255));
    cv::inRange(dotMat, cv::Scalar(0, 0, 0), cv::Scalar(65, 255, 255), dotMask);
//    cv::inRange(dotMat, cv::Scalar(0, 0, 0), maxScalar, dotMask);

        cv::bitwise_not(dotMask, dotMask);
    //    dotMask = !dotMask;

//    getMaskOfMat(compasMat, dotMask, cv::Scalar(80, 0, 110), cv::Scalar(151, 186, 255));
    std::vector< std::vector <cv::Point> > dotContours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dotMask, dotContours, hierarchy, cv::RETR_CCOMP,  cv::CHAIN_APPROX_SIMPLE);
    if(dotContours.empty())  {
        qDebug() << "Контуры точки не найдены";
        imshow("win3", compasMask);
        imshow("win2", compasMat);
        imshow("win4", dotMat);
        imshow("win5", dotMask);
        return &m_compas;
    }
    std::vector<double> areas_dot(dotContours.size());
    std::vector<int> idx_dot(dotContours.size());
    m_compas.face = true;
    for(size_t i = 0; i < dotContours.size(); i++) {
        areas_dot[i] = cv::contourArea(dotContours[i], false);
        idx_dot[i] = i;
//        qDebug() << hierarchy[i][0] << hierarchy[i][1]  << hierarchy[i][2]  << hierarchy[i][3];
    }

    std::sort(idx_dot.begin(), idx_dot.end(), AreaCmp(areas_dot));
    if(hierarchy[idx_dot.front()][2] == -1)
        m_compas.face = true;
    else
        m_compas.face = false;
    m_compas.size = compasMat.size();
    m_compas.active = true;
//    qDebug() << m_compas.pDot.x << m_compas.pCenter.x;
//    qDebug() << "face" << m_compas.face;
//    qDebug() << hierarchy[idx_dot.front()][0] << hierarchy[idx_dot.front()][1]  << hierarchy[idx_dot.front()][2]  << hierarchy[idx_dot.front()][3];
//    cv::drawContours(dotMat, dotContours, hierarchy[idx_dot.front()][2], cvGreen, 1);
//    cv::circle(compasMat, dot.center, 2, cvGreen, 2);
//    imshow("win2", dst);
    cv::resize(dotMask, dotMask, dotMask.size() * 5);

//    imshow("win4", compasMat);
//    cv::circle(compasMat, cv::minAreaRect(dotContour).center, 2, cvRad, 2);
//    cv::circle(compasMat, dotRot.center, 2, cvBlue, 1);
    cv::resize(dotMat, dotMat, dotMat.size() * 5);
    imshow("win3", compasMask);
    imshow("win2", compasMat);
    imshow("win4", dotMat);
    imshow("win5", dotMask);
    return &m_compas;
}

void CaptureWindow::testTarget2()
{
    if(DEBUG2)
        qDebug() << ".";

    cv::Rect fieldSpace(calcRectFromPartOfIndex(11, 48, 72));
    cv::Mat dst;
    cv::Mat maskBlur;
    win(fieldSpace).copyTo(dst);
    cv::Point centerPoint = cv::Point(fieldSpace.width / 2, fieldSpace.height / 2);
    cv::circle(dst, centerPoint, 3, cvGreen);
    cv::Mat mask(dst.size(), CV_8U, 1);
    std::vector<cv::Vec3f> circles;
    int targetVal2 = 240;
    int targetVal3 = 240;
    while(!m_cursorTarget.active && targetVal2 >= 100 && targetVal3 >= 100) {
        cv::inRange(dst, cv::Scalar(0, targetVal2, targetVal3), cv::Scalar(255, 255, 255), mask);                         // hsv
        circles.clear();
        //    cv::blur(mask, maskBlur, cv::Size(3, 3));
        cv::GaussianBlur(mask, maskBlur, cv::Size(3, 3), 2, 2);

//        cv::HoughCircles(mask, circles, cv::HOUGH_GRADIENT, 1, dst.rows / 1, 200, 15, 28, 30);
        if(maxNumber == 0)
            maxNumber = 1;
        cv::HoughCircles(maskBlur, circles, cv::HOUGH_GRADIENT, 1, dst.rows / 200, 200, 10, 44, 50);


        targetVal2 -= 15;
        if(targetVal2 <= 100) {
            targetVal2 = 240;
            targetVal3 -= 25;
        }
    }
    if(circles.empty()) {
        qDebug() << "Нет кругов";
        return ;
    }
    std::vector<double> lengths;
    std::vector<size_t> idx;
    idx.resize(circles.size());
    lengths.resize(circles.size());
    for(size_t i = 0; i < circles.size() - 1; i++) {
        for(size_t j = i; j < circles.size(); j++) {

        }
//        cv::circle(dst, cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])), cvRound(circles[i][2]), cvGreen, 2);
        lengths[i] = ( sqrt( static_cast<double>(pow(circles[i+1][0] - circles[i][0], 2)) + static_cast<double>(pow(circles[i+1][1] - circles[i][1], 2) )) );
        idx[i] = i;
    }
    std::sort(idx.begin(), idx.end(), AreaCmp(lengths));
    for(size_t i = 0; i < circles.size(); i++) {
        qDebug() << "x =" << circles[idx[i]][0] << " y =" << circles[idx[i]][1] << lengths[idx[i]];
    }
    qDebug() << "targetVal2 =" << targetVal2 << " targetVal3 =" << targetVal3 << " circles.size =" << circles.size();
    cv::circle(dst, m_cursorTarget.pointTarget, 3, cv::Scalar(0, 255, 0));



    imshow("win2", maskBlur);
//    imshow("win2", mask);
    imshow("win3", dst);
}

void CaptureWindow::imageOverlay()
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Rect fieldSpace(calcRectFromPartOfIndex(11, 25, 84));
    cv::Mat dst;
    win(fieldSpace).copyTo(dst);
    m_cursorTarget.active = false;
    m_cursorTarget.rectSrc = fieldSpace;
    m_cursorTarget.pointCenter = cv::Point(fieldSpace.width / 2, fieldSpace.height / 2 + 15);
    cv::Mat mask(dst.size(), CV_8U, 1);
    cv::Mat hsv;
//    cv::cvtColor(dst, hsv, CV_BGR2HSV);
//    int targetSatMax = 246;
//    int targetValMax = 253;
    int targetVal2 = 240;
    int targetVal3 = 240;
//    while(!m_cursorTarget.active && targetSatMax <= 255 && targetValMax <= 255) {
    while(!m_cursorTarget.active && targetVal2 >= 100 && targetVal3 >= 100) {
        //    cv::inRange(hsv, minScalar, maxScalar, mask);                         // hsv
//        cv::inRange(hsv, cv::Scalar(15, 43, 117), cv::Scalar(48, targetSatMax, targetValMax), mask);                         // hsv
//        cv::inRange(dst, minScalar, maxScalar, mask);                         // hsv
        cv::inRange(dst, cv::Scalar(0, targetVal2, targetVal3), cv::Scalar(255, 255, 255), mask);                         // hsv
        //    cv::Mat maskBlur;
        //    cv::blur(mask, maskBlur, cv::Size(3, 3));
            std::vector< std::vector< cv::Point> > contours2Src;
            std::vector< cv::Vec4i > hierarchy;
            cv::findContours(mask, contours2Src, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

            std::vector<std::vector< cv::Point > > contours2Prep;
            for(size_t j = 0; j < contours2Src.size(); j++) {
                double area = cv::contourArea(contours2Src[j]);
                if(area > 50 && area < 200)
                    contours2Prep.push_back(contours2Src[j]);
            }
//            double area = cv::contourArea(contours2Src[j]);
//            if(area > 80.1)
//                contours2Prep.push_back(contours2Src[j]);
//                qDebug() << "area =" << QString::number(area, 'f', 2);



            std::vector<cv::Rect> vecRects;
//            getRectsInContour(contours2Src, vecRects);
            getRectsInContour(contours2Prep, vecRects);

        #define TARGET_DELTA            10
        #define TARGET_HEIGHT_MIN       95
        #define TARGET_HEIGHT_MAX       146
        #define TARGET_WIDTH_MIN        95
        #define TARGET_WIDTH_MAX        218
            for(size_t i = 0; i < vecRects.size(); i++) {
                if(vecRects[i].height < TARGET_HEIGHT_MIN - TARGET_DELTA || vecRects[i].height > TARGET_HEIGHT_MAX + TARGET_DELTA
                        || vecRects[i].width < TARGET_WIDTH_MIN - TARGET_DELTA || vecRects[i].width > TARGET_WIDTH_MAX + TARGET_DELTA)
                    continue;
                if(m_cursorTarget.active) {
                    qDebug() << "Внимание Обнаружено несколько таргетов";
                }
                m_cursorTarget.active = true;
                m_cursorTarget.rectTarget = cv::Rect(vecRects[i].x, vecRects[i].y, vecRects[i].height, vecRects[i].height);
                m_cursorTarget.pointTarget = cv::Point(m_cursorTarget.rectTarget.x + m_cursorTarget.rectTarget.width / 2,
                                                       m_cursorTarget.rectTarget.y + m_cursorTarget.rectTarget.height / 2);
                cv::rectangle(dst, m_cursorTarget.rectTarget, cv::Scalar(0xFF, 0x0, 0x0));
            }
//            qDebug() << cv::contourArea()
            targetVal2 -= 15;
            if(targetVal2 <= 100) {
                targetVal2 = 240;
                targetVal3 -= 15;
            }
//            targetSatMax++;
//            if(targetSatMax == 255) {
//                targetValMax++;
//                targetSatMax = 246;
//            }
    }
//    qDebug() << "targetSatMax =" << targetSatMax << " targetValMax =" << targetValMax;
//    qDebug() << targetVal2 << targetVal3;
    cv::circle(dst, m_cursorTarget.pointTarget, 3, cv::Scalar(0, 255, 0));



//    imshow("win4", maskBlur);
    imshow("win2", mask);

//    cv::addWeighted()
    cv::cvtColor(mask, mask, CV_GRAY2BGR);
    mask.copyTo(win(fieldSpace));
    imshow("win3", dst);

//    win(fieldSpace).copyTo(win, dst);
    //    dst.copyTo(win);
}

Distance *CaptureWindow::recognizDistance()
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Mat dst;
    cv::Mat mask;
    cv::Rect recognizRect;
    win(calcRectFromPartOfIndex(5, 12, 12)).copyTo(dst);
    cv::Mat hsv;
    cv::cvtColor(dst, hsv, CV_BGR2HSV);
    int targetSatMax = 246;
    bool rectFound = false;
    while(!m_cursorTarget.active && targetSatMax <= 255) {
            cv::inRange(hsv, cv::Scalar(15, 43, 117), cv::Scalar(48, targetSatMax, 253), mask);                         // hsv
            std::vector< std::vector< cv::Point> > contours2Src;
            std::vector< cv::Vec4i > hierarchy;
            cv::findContours(mask, contours2Src, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
            std::vector<cv::Rect> vecRects;
            getRectsInContour(contours2Src, vecRects);
        #define TARGET_DELTA            10
        #define TARGET_HEIGHT_MIN       95
        #define TARGET_HEIGHT_MAX       146
        #define TARGET_WIDTH_MIN        95
        #define TARGET_WIDTH_MAX        218
            for(size_t i = 0; i < vecRects.size(); i++) {
                if(vecRects[i].height < TARGET_HEIGHT_MIN - TARGET_DELTA || vecRects[i].height > TARGET_HEIGHT_MAX + TARGET_DELTA
                        || vecRects[i].width < TARGET_WIDTH_MIN - TARGET_DELTA || vecRects[i].width > TARGET_WIDTH_MAX + TARGET_DELTA)
                    continue;
                if(m_cursorTarget.active) {
                    qDebug() << "Внимание Обнаружено несколько таргетов";
                }
                recognizRect = cv::Rect(vecRects[i].x + vecRects[i].height - 3,
                                        vecRects[i].y + vecRects[i].height / 2, 60, 25);
                rectFound = true;
                cv::rectangle(dst, recognizRect, cv::Scalar(0xFF, 0x0, 0x0));
            }
            targetSatMax++;
    }
    if(!rectFound) {
//        //cv::imshow("win4", mask);
//        //cv::imshow("win3", dst);
        return &distance;
    }


////    win(cv::Rect(1018, 542, 130, 23)).copyTo(dst);
////    getMaskOfMat(dst, mask, cv::Scalar(  13 , 150 , 177  ), cv::Scalar(30 , 250 , 245 ));
//    getMaskOfMat(dst, mask, minScalar, maxScalar);

////    cv::Mat blurMask;
////    int k = minNumber;
////    if(k%2 == 0)
////        k++;
////    cv::medianBlur(mask, mask, 3);
////    cv::GaussianBlur(mask, mask, cv::Size(3, 3), 0, 0);
////    cv::blur(mask, mask, cv::Size(3, 3));
////    //cv::imshow("win5", blurMask);

////    myOCRRus
//    std::vector<cv::Vec4i> lines;
//    cv::Point p1, p2;
////    HoughLinesP(mask, lines, 1, CV_PI/180, 20, 50, 4);
////    cv::Canny(mask, mask, 50, 200, 3);
//    int threshold = 1;
//    while(lines.empty() && threshold < 50) {
//        HoughLinesP(mask, lines, 1, CV_PI/180, threshold, minNumber, 1);
//        threshold++;
//    }
//    if(lines.empty()) {
//        //cv::imshow("win4", mask);
//        //cv::imshow("win3", dst);
//        return;
//    }
//    qDebug() << "threshold =" << threshold;

//    for( size_t i = 0; i < lines.size(); i++ )
//    {
//        p1 = cv::Point(lines[i][0], lines[i][1]);
//        p2 = cv::Point(lines[i][2], lines[i][3]);
//        cv::line( dst, p1, p2, cvRad, 2);
//    }
    cv::rectangle(dst, recognizRect, cvBlue, 1);

    cv::Mat recognizMask;
    cv::Mat recognizMat;
    dst(recognizRect).copyTo(recognizMat);
//    getMaskOfMat(recognizMat, recognizMask, cv::Scalar(  14, 127 , 127 ), cv::Scalar(30 , 250 , 240 ));
    getMaskOfMatHSV(recognizMat, recognizMask, minScalar, maxScalar);


    cv::bitwise_not(recognizMask, recognizMask);
    timeElapse.restart();
    myOCRRusDigits->SetImage( (uchar*)recognizMask.data, recognizMask.size().width, recognizMask.size().height, recognizMask.channels(), recognizMask.step1());
    myOCRRusDigits->Recognize(nullptr);
    QString sRec = myOCRRusDigits->GetUTF8Text();
//    qDebug() << "elapse" << timeElapse.elapsed();
    if(sRec.isEmpty())
        sRec = "not recognized";
    sRec = sRec.simplified();
    sRec = sRec.toLower();
    deleteCharExtra(sRec);
    QString sKM = "км";
    QString srcRec = sRec;
    QString slog1, slog2;
    for(int i = sRec.size() - 1; i >= 0; i--) {
        if(sRec[i] == "э")
            sRec.replace(i, 1, "9");
        if(sRec[i] == "з")
            sRec.replace(i, 1, "3");
        if(sRec[i] == "д")
            sRec.replace(i, 1, "2");
        if(sRec[i] == "в")
            sRec.replace(i, 1, "8");
        if(sRec[i] == "о")
            sRec.replace(i, 1, "0");
        if(sRec[i] == "б")
            sRec.replace(i, 1, "6");
        if(sRec[i] == ",")
            sRec.replace(i, 1, ".");
        if(sRec[i] == "ы")
            sRec.replace(i, 1, "м");
        if(sRec[i] == "и")
            sRec.replace(i, 1, "м");

        if(slog1.contains(sKM)) {
            slog2.prepend(sRec[i]);
        } else {
            slog1.prepend(sRec[i]);
        }

    }
    slog2 = slog2.remove(QRegExp("[^0-9.]"));



    bool check = false;
    double dist = slog2.toDouble(&check);
    if(slog2.isEmpty()) {
        //cv::imshow("win5", recognizMask);
        //cv::imshow("win4", mask);
        //cv::imshow("win3", dst);
        return &distance;
    }
    if(check) {
//        qDebug() << "recognize:" << dist << slog1 << "\t" << srcRec;
        if(distance.distanceList.size() == 5)
            if(dist > distance.distance * 2.  || dist < distance.distance / 2.) {
                //cv::imshow("win5", recognizMask);
                //cv::imshow("win4", mask);
                //cv::imshow("win3", dst);
                return &distance;
            }
        slog2 = QString::number(dist, 'f', 0);
        distance.distanceList.append(slog2);
        if(distance.distanceList.size() == 6)
            distance.distanceList.removeAt(0);
        QList<QStringList> sList;
        std::vector<int> idx;
        std::vector<int> countMatch;
        countMatch.resize(distance.distanceList.size());
        idx.resize(distance.distanceList.size());
        for(int i = 0; i < distance.distanceList.size(); i++) {
            int count = 0;
            for(int k = i; k < distance.distanceList.size()-1; k++) {
                if(distance.distanceList[i] == distance.distanceList[k + 1])
                    count++;
            }
            countMatch[i] = count;
            idx[i] = i;
        }
        std::sort(idx.begin(), idx.end(), DigitCmp(countMatch));
        distance.distance = distance.distanceList[idx[0]].toDouble();
        qDebug() << "Дистанция " << distance.distance;

    }
//    else
//        qDebug() << sRec << slog2 << slog1;


    //cv::imshow("win5", recognizMask);
    //cv::imshow("win4", mask);
    //cv::imshow("win3", dst);
    return &distance;
}

void CaptureWindow::testColor()
{
    cv::Mat hsv;
    cv::Mat rectMat;
    cv::Mat mask;
    win(cv::Rect(100, 100, g_screen.width() - 500, g_screen.height() - 200)).copyTo(rectMat);
    cv::cvtColor(rectMat, hsv, CV_BGR2HSV );
    if(minScalar[0] >= 180)
        minScalar[0] = 179;
//    minScalar[0] *= 2;
    cv::inRange(hsv, minScalar, maxScalar, mask);


    cv::imshow("win2", hsv);
    cv::imshow("win3", mask);
}


bool CaptureWindow::checkContour(std::vector<cv::Point> &acvVecPointCont, cv::Mat &cvMatOut)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Point min_x;
    min_x.x = g_screen.width();
    cv::Point max_x;
    max_x.x = 0;
    cv::Point min_y;
    min_y.y = g_screen.height();
    cv::Point max_y;
    max_y.y = 0;
    for(size_t i = 0; i < acvVecPointCont.size(); i++) {
        if(acvVecPointCont[i].x < min_x.x)
            min_x = acvVecPointCont[i];
        if(acvVecPointCont[i].x > max_x.x)
            max_x = acvVecPointCont[i];
        if(acvVecPointCont[i].y < min_y.y)
            min_y = acvVecPointCont[i];
        if(acvVecPointCont[i].y > max_y.y)
            max_y = acvVecPointCont[i];
    }
    if(min_x.x < 30)
        return false;
    if(max_x.x > cvMatOut.size().width - 30)
        return false;
    int ndX = max_x.x - min_x.x;
    if(ndX < cvMatOut.size().width / 2)
        return false;
    int ndY = max_y.y - min_y.y;
    if(ndY == 0)
        return false;
    double dblRat = ndX / ndY;
    if(dblRat > 15) {
        std::vector < cv::Point> vecPoint;
        vecPoint.push_back(min_x);
        vecPoint.push_back(max_x);
//        m_veclinesMenu.push_back(vecPoint);

        return true;
    }
    return false;
}

bool CaptureWindow::findLeftPoint(std::vector<std::vector<cv::Point> > &acvVecPointCont, cv::Mat &cvMatOut, cv::Point &acvpRight, cv::Point &acvpLeft)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Point min_x;
    min_x.x = cvMatOut.size().width;
    cv::Point max_x;
    max_x.x = 0;
    cv::Point min_y;
    min_y.y = cvMatOut.size().height;
    std::vector<cv::Point> vecpcvRight;
    std::vector<cv::Point> vecpcvLeft;
    for(size_t j = 0; j < acvVecPointCont.size(); j++) {
        std::vector<cv::Point> vectmp = acvVecPointCont[j];
        for(size_t i = 0; i < vectmp.size(); i++) {
            if(vectmp[i].x < min_x.x)
                min_x = vectmp[i];
            if(vectmp[i].x > max_x.x)
                max_x = vectmp[i];

            if(vectmp[i].y < min_y.y)
                min_y = vectmp[i];
        }
    }

    acvpLeft.x = min_x.x;
    acvpLeft.y = min_y.y;
    acvpRight.x = max_x.x;
    acvpRight.y = min_y.y;
    return true;
}

bool CaptureWindow::checkApproximRect(const cv::Rect acRectSrs, const cv::Rect acRectCurCMP, int anDelta)
{
    if(DEBUG2)
        qDebug() << ".";
    int wnDelta = anDelta + 1;
    if(         acRectCurCMP.x          > acRectSrs.x - anDelta         && acRectCurCMP.x       < acRectSrs.x + anDelta
            &&  acRectCurCMP.y          > acRectSrs.y - anDelta         && acRectCurCMP.y       < acRectSrs.y + anDelta
            &&  acRectCurCMP.width      > acRectSrs.width - wnDelta     && acRectCurCMP.width   < acRectSrs.width + wnDelta
            &&  acRectCurCMP.height     > acRectSrs.height - wnDelta    && acRectCurCMP.height  < acRectSrs.height + wnDelta)
        return true;
    else
        return false;
}

void CaptureWindow::getRectsInContour(std::vector<std::vector<cv::Point> > &acvVecPointCont, std::vector<cv::Rect> &vecRects)
{
    if(DEBUG2)
        qDebug() << ".";
    vecRects.clear();
    for(size_t j = 0; j < acvVecPointCont.size(); j++) {
        std::vector<cv::Point> vectmp = acvVecPointCont[j];



        if(vectmp.size() < 3)
            continue;
        cv::Point min_x;
        min_x.x = g_screen.width();
        cv::Point max_x;
        max_x.x = 0;
        cv::Point min_y;
        min_y.y = g_screen.height();
        cv::Point max_y;
        max_y.y = 0;
        for(size_t i = 0; i < vectmp.size(); i++) {
            if(acvVecPointCont[j][i].x < min_x.x)
                min_x = acvVecPointCont[j][i];
            if(acvVecPointCont[j][i].x > max_x.x)
                max_x = acvVecPointCont[j][i];
            if(acvVecPointCont[j][i].y < min_y.y)
                min_y = acvVecPointCont[j][i];
            if(acvVecPointCont[j][i].y > max_y.y)
                max_y = acvVecPointCont[j][i];
        }
        cv::Rect rectTmp = cv::Rect(min_x.x, min_y.y, max_x.x - min_x.x, max_y.y - min_y.y);
        if(rectTmp.width < 55)
            continue;
        if(rectTmp.height < 28)
            continue;
        vecRects.push_back(rectTmp);
    }
}

void CaptureWindow::getRectsOfPatternMenu1(std::vector<std::vector<cv::Point> > &acvVecPointCont, std::vector<cv::Rect> &vecRects, std::vector<cv::Rect> &vecRectsShort)
{
    if(DEBUG2)
        qDebug() << ".";
    vecRectsShort.clear();
    vecRects.clear();
    for(size_t j = 0; j < acvVecPointCont.size(); j++) {
        std::vector<cv::Point> vectmp = acvVecPointCont[j];
        if(vectmp.size() < 3)
            continue;
        cv::Point min_x;
        min_x.x = g_screen.width();
        cv::Point max_x;
        max_x.x = 0;
        cv::Point min_y;
        min_y.y = g_screen.height();
        cv::Point max_y;
        max_y.y = 0;
        for(size_t i = 0; i < vectmp.size(); i++) {
            if(acvVecPointCont[j][i].x < min_x.x)
                min_x = acvVecPointCont[j][i];
            if(acvVecPointCont[j][i].x > max_x.x)
                max_x = acvVecPointCont[j][i];
            if(acvVecPointCont[j][i].y < min_y.y)
                min_y = acvVecPointCont[j][i];
            if(acvVecPointCont[j][i].y > max_y.y)
                max_y = acvVecPointCont[j][i];
        }
        cv::Rect rectTmp = cv::Rect(min_x.x, min_y.y, max_x.x - min_x.x, max_y.y - min_y.y);
        if(rectTmp.width < 500) {
            if(rectTmp.height < 30)
                continue;
            if(rectTmp.width > 260)
                continue;
            if(rectTmp.width < 200)
                continue;
            if(rectTmp.y > 400)
                continue;
            vecRectsShort.push_back(rectTmp);
        } else {
            if(rectTmp.height > 35)
                continue;
            if(rectTmp.y > 350)
                continue;
            if(rectTmp.y < 50)
                continue;
            vecRects.push_back(rectTmp);
        }
    }
}

void CaptureWindow::getRectsOfPatternMenu1SubNav(std::vector<std::vector<cv::Point> > &acvVecPointCont, std::vector<cv::Rect> &vecRects)
{
    if(DEBUG2)
        qDebug() << ".";
    vecRects.clear();
    for(size_t j = 0; j < acvVecPointCont.size(); j++) {
        std::vector<cv::Point> vectmp = acvVecPointCont[j];
        if(vectmp.size() < 3)
            continue;
        cv::Point min_x;
        min_x.x = g_screen.width();
        cv::Point max_x;
        max_x.x = 0;
        cv::Point min_y;
        min_y.y = g_screen.height();
        cv::Point max_y;
        max_y.y = 0;
        for(size_t i = 0; i < vectmp.size(); i++) {
            if(acvVecPointCont[j][i].x < min_x.x)
                min_x = acvVecPointCont[j][i];
            if(acvVecPointCont[j][i].x > max_x.x)
                max_x = acvVecPointCont[j][i];
            if(acvVecPointCont[j][i].y < min_y.y)
                min_y = acvVecPointCont[j][i];
            if(acvVecPointCont[j][i].y > max_y.y)
                max_y = acvVecPointCont[j][i];
        }
        cv::Rect rectTmp = cv::Rect(min_x.x, min_y.y, max_x.x - min_x.x, max_y.y - min_y.y);
        if(rectTmp.height < 40)
            continue;
        if(rectTmp.height > 80)
            continue;
        if(rectTmp.width > 170)
            continue;
        if(rectTmp.width < 70)
            continue;
        vecRects.push_back(rectTmp);
    }
}

bool CaptureWindow::getLeftAndUpInVecRects(const std::vector<cv::Rect> &vecRects, int &nXLeft, int &nYUp)
{
    if(DEBUG2)
        qDebug() << ".";
    nXLeft = g_screen.width();
    nYUp = g_screen.height();
    for(size_t i = 0; i < vecRects.size(); i++) {
        if(vecRects[i].x < nXLeft)
            nXLeft = vecRects[i].x;
        if(vecRects[i].y < nYUp)
            nYUp = vecRects[i].y;
    }

    if(nXLeft == g_screen.width())
        return false;
    if(nYUp == g_screen.height())
        return false;
    return true;
}

cv::Rect CaptureWindow::getRectInContour(std::vector<cv::Point> &acvVecPoint)
{
    if(DEBUG2)
        qDebug() << ".";
    if(acvVecPoint.size() < 3)
        return cv::Rect(0, 0, 0, 0);
    cv::Point min_x;
    min_x.x = g_screen.width();
    cv::Point max_x;
    max_x.x = 0;
    cv::Point min_y;
    min_y.y = g_screen.height();
    cv::Point max_y;
    max_y.y = 0;
    for(size_t i = 0; i < acvVecPoint.size(); i++) {
        if(acvVecPoint[i].x < min_x.x)
            min_x = acvVecPoint[i];
        if(acvVecPoint[i].x > max_x.x)
            max_x = acvVecPoint[i];
        if(acvVecPoint[i].y < min_y.y)
            min_y = acvVecPoint[i];
        if(acvVecPoint[i].y > max_y.y)
            max_y = acvVecPoint[i];
    }
    return cv::Rect(min_x.x, min_y.y, max_x.x - min_x.x, max_y.y - min_y.y);
}

cv::Mat CaptureWindow::makeBinHeadMenu(cv::Mat &aMatWord, cv::Rect aRectCursor)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Mat matCursor;
    aMatWord(aRectCursor).copyTo(matCursor);

    cv::Mat bin(matCursor.size(), CV_8U, 1);

    cv::inRange(matCursor, cv::Scalar(0, 106, 203), cv::Scalar(255, 255, 255), bin);

    //cv::imshow("win4", bin);
    //cv::imshow("win5", matCursor);
    return bin;
}

cv::Mat CaptureWindow::parsBinLineNavList(cv::Mat &aMatList, cv::Rect aRectCursor)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Mat matCursor;
    aMatList(aRectCursor).copyTo(matCursor);

    cv::Mat bin(matCursor.size(), CV_8U, 1);

    cv::inRange(matCursor, cv::Scalar(0, 106, 203), cv::Scalar(255, 255, 255), bin);
    cv::Mat tmpMat;
    cv::cvtColor(bin, tmpMat, cv::COLOR_GRAY2BGR);
    cv::Point point;
    int seekXDistance = matCursor.size().width - 100;
    int cutXLeft = 0;
    for(int i = 0; i < sNamePicNavList.size(); i++) {
        point = getPointAreaInMat(sNamePicNavList[i].toStdString(), tmpMat);
        if(point.x > 0) {
            cv::circle(matCursor, point, 1, cv::Scalar(0, 0, 255),2);
            int widthMat = mp_dataSet->at(sNamePicNavList[i].toStdString()).mat.size().width;
            if(point.x + (widthMat / 2) > cutXLeft)
                cutXLeft = point.x + (widthMat / 2);
        }
//        qDebug() << point.x << point.y;
    }
    int cutXRight = seekXDistance;
    int widthRightPic = 0;
    for(int i = 0; i < sNamePicNavListTar.size(); i++) {
        point = getPointAreaInMat(sNamePicNavListTar[i].toStdString(), tmpMat);
        if(point.x > 0) {
            cv::circle(matCursor, point, 1, cv::Scalar(0, 0, 255),2);
            widthRightPic = mp_dataSet->at(sNamePicNavListTar[i].toStdString()).mat.size().width;
            if(point.x - (widthRightPic / 2) < cutXRight)
                cutXRight = point.x - (widthRightPic / 2);
        }
    }

    cv::Mat cutMat;
    matCursor(cv::Rect(cutXLeft, 0, cutXRight - cutXLeft, matCursor.size().height)).copyTo(cutMat);
    cv::Mat binCut(matCursor.size(), CV_8U, 1);

    cv::inRange(cutMat, cv::Scalar(0, 106, 203), cv::Scalar(255, 255, 255), binCut);
    //cv::imshow("win4", binCut);
    //cv::imshow("win5", matCursor);
    return binCut;
//    return cv::Mat();
}


std::vector<cv::Rect> CaptureWindow::getRectsBigInContours(cv::Mat &binMat)
{
    if(DEBUG2)
        qDebug() << ".";
    std::vector< std::vector< cv::Point> > contours2Src;
    std::vector< std::vector< cv::Point> > contMoreLength;
    std::vector< cv::Vec4i > hierarchy;
    cv::findContours(binMat, contours2Src, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> vecRects;
    getRectsInContour(contours2Src, vecRects);

    return vecRects;
}

CursorPanel *CaptureWindow::subPanel1Nav()
{
    if(DEBUG2)
        qDebug() << ".";
    m_cursorPan.rectSubNavList.y = -1;
    m_cursorPan.activeSubNav = false;
    m_cursorPan.sSubNavName = "";


    cv::Mat dst;
    cv::Mat mask;

    cv::Rect rectToCopy = cv::Rect(400, 650, 900, 250);
    win(rectToCopy).copyTo(dst);
    cv::Mat hsv;


    cv::cvtColor( dst, hsv, CV_BGR2HSV );
//    cv::inRange(hsv, minScalar, maxScalar, mask);



    int minVal3 = 230;
    int minVal2 = 230;
    int minVal1 = 10;
    std::vector< std::vector< cv::Point> > vecRects;
    while(minVal1 < 15 && (vecRects.empty() || vecRects.size() > 2)) {
        std::vector< std::vector< cv::Point> > contours2;
        cv::inRange(hsv, cv::Scalar(minVal1, minVal2, minVal3), cv::Scalar(50, 255, 255), mask);
        cv::imshow("win3", mask);
        QApplication::processEvents();


        cv::findContours(mask, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//        cv::drawContours(rectMat, contours2, -1, cvGreen, 2);
        for(size_t i = 0; i < contours2.size(); i++) {
            cv::Rect push = cv::boundingRect(contours2[i]);
            double area = cv::contourArea(contours2[i]);
            if(area < 7500)
                continue;
            if(push.width > 250 || push.height > 200)
                continue;
            if(push.width < 70 || push.height < 40)
                continue;
            //            qDebug() << "area =" << area;




            cv::Mat recognizeMask;      // Искомая маска
            cv::Mat rotateMat;
            dst(cv::boundingRect(contours2[i])).copyTo(rotateMat);
            cv::Mat mask2;
            cv::Mat hsv2;
            cv::cvtColor(rotateMat, hsv2, CV_BGR2HSV);


            //            int minVal32 = 230;
            //            int minVal22 = 230;
            //            while(minVal22 > 70 && (vecRects.empty() || vecRects.size() > 2)) {

            inRange(hsv2, cv::Scalar(minVal1, minVal2, minVal3), cv::Scalar(50, 255, 255), mask2);                     // Получаем новую маску

            std::vector< std::vector< cv::Point> > contours22;
            std::vector<size_t> sortIdx(contours22.size());
            cv::findContours(mask2, contours22, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);  // из маски вытягиваем контуры
            if(getContoursIndexSort(mask2, contours22, sortIdx)) {
                cv::RotatedRect minRect = cv::minAreaRect(contours22[static_cast<size_t>(sortIdx[0])]);                     // Получаем угол на который нужно повернуть
                if(minRect.angle < -45)
                    minRect.angle += static_cast<float>(90.);
                cv::Mat transformMat;
                cv::Mat rMatToWarp = cv::getRotationMatrix2D(minRect.center, static_cast<double>(minRect.angle), 1);
                cv::warpAffine(rotateMat, transformMat, rMatToWarp, transformMat.size(), cv::INTER_CUBIC);          // Поворачиваем картинку
                cv::Mat recognizeMask2;
                cv::Mat recognizeHSV;
                cv::cvtColor(transformMat, recognizeHSV, CV_BGR2HSV);
                inRange(recognizeHSV, cv::Scalar(minVal1, minVal2, minVal3), cv::Scalar(50, 255, 255), recognizeMask2);                     // Получаем новую маску



                //                    cv::cvtColor(recognizeMask2, recognizeMask, cv::COLOR_GRAY2BGR);




                std::vector< std::vector< cv::Point> > cont_inside;
                std::vector<size_t> sortIdxInside(cont_inside.size());
                if(getContoursIndexSort(recognizeMask2, cont_inside, sortIdxInside)) {
                    cv::Mat aColorMat;
                    cv::Rect foundRect = cv::boundingRect(cont_inside[static_cast<size_t>(sortIdxInside[0])]);
                    transformMat(foundRect).copyTo(aColorMat);
                    cv::Mat recognizeHSVInside;
                    cv::cvtColor(aColorMat, recognizeHSVInside, CV_BGR2HSV);
                    inRange(recognizeHSVInside, cv::Scalar(minVal1, minVal2, minVal3), cv::Scalar(50, 255, 255), recognizeMask);                     // Получаем новую маску

                    cv::blur(recognizeMask, recognizeMask, cv::Size(3, 3));
//                    cv::medianBlur(recognizeMask, recognizeMask, 5);
//                    cv::imshow("win1", recognizeMask);
                    QApplication::processEvents();


                    cv::cvtColor(recognizeMask, recognizeMask, cv::COLOR_GRAY2BGR);
                    double coeff = 0.0;
                    if(srchAreaOnceInMat(subNamePicMenu1NavList[0].toStdString(), recognizeMask, coeff)) {
                        m_cursorPan.sSubNavName = "fix_target";
                        if(coeff > 0.985) {
                            qDebug() << m_cursorPan.sSubNavName << QString::number(coeff, 'f', 2);
                            m_cursorPan.activeSubNav = true;
                            vecRects.push_back(contours2[i]);
                            return &m_cursorPan;
                        }

                    }
                    coeff = 0.0;
                    if(srchAreaOnceInMat(subNamePicMenu1NavList[1].toStdString(), recognizeMask, coeff)) {
                        m_cursorPan.sSubNavName = "unfix_target";
                        if(coeff > 0.987) {
                            qDebug() << m_cursorPan.sSubNavName << QString::number(coeff, 'f', 2);
                            m_cursorPan.activeSubNav = true;
                            vecRects.push_back(contours2[i]);
                            return &m_cursorPan;
                        }
                    }
                    coeff = 0.0;
                    if(srchAreaOnceInMat(subNamePicMenu1NavList[3].toStdString(), recognizeMask, coeff)) {
                        m_cursorPan.sSubNavName = "enable_hypermode_helper";
                        if(coeff > 0.987) {
                            qDebug() << m_cursorPan.sSubNavName << QString::number(coeff, 'f', 2);
                            m_cursorPan.activeSubNav = true;
                            vecRects.push_back(contours2[i]);
                            return &m_cursorPan;
                        }
                    }
                }

            }




            //                minVal32 -= 15;
            //                if(minVal32 < 130) {
            //                    minVal32 = 230;
            //                    minVal22 -= 15;
            //                }
            //            }





            m_cursorPan.sSubNavName = "missed_the_mark";
//            qDebug() << m_cursorPan.sSubNavName;
            m_cursorPan.activeSubNav = true;
            //cv::imshow("win5", recognizeMask);
            //            break;


            continue;
//            vecRects.push_back(contours2[i]);
        }
        minVal3 -= 15;
        if(minVal3 < 100) {
            minVal3 = 250;
            minVal2 -= 15;
            if(minVal2 < 100) {
                minVal2 = 250;
                minVal1 += 2;
            }
        }
//        qDebug() << minVal1 << minVal2 << minVal3;
//        QThread::msleep(300);
    }

    imshow("win4", dst);
    return &m_cursorPan;
}

void CaptureWindow::transformSubMenu(cv::Mat &acvMat)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Point2f srcTri[4], dstQuad[4];
    srcTri[0].x = 0;
    srcTri[0].y = 0;
    srcTri[1].x = acvMat.size().width - 1;
    srcTri[1].y = 0;
    srcTri[2].x = 0;
    srcTri[2].y = acvMat.size().height - 1;
    srcTri[3].x = acvMat.size().width - 1;
    srcTri[3].y = acvMat.size().height - 1;


    dstQuad[0].x = 0;  //Top left
    dstQuad[0].y = 0;
    dstQuad[1].x = acvMat.size().width - 1;  //Top right
    dstQuad[1].y = -45;
    dstQuad[2].x = -65;  //Bottom left
    dstQuad[2].y = acvMat.size().height;
    dstQuad[3].x = acvMat.size().width - 30;  //Bottom right
    dstQuad[3].y = acvMat.size().height + 315;
    cv::Mat warp_matrix = cv::getPerspectiveTransform(srcTri, dstQuad);
    cv::warpPerspective(acvMat, acvMat, warp_matrix, acvMat.size());
}

//void CaptureWindow::determinCursorSubNavList(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst)
//{

//}

CursorPanel *CaptureWindow::menuDocking()
{
    if(DEBUG2)
        qDebug() << ".";

    m_cursorPan.activeMenuDocking = false;
    cv::Mat hsv;
    cv::Mat dst;
    win(cv::Rect(810, 805, 310, 130)).copyTo(dst);
    cv::cvtColor( dst, hsv, CV_BGR2HSV );
    cv::Mat mask;
//    cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);
    std::vector< std::vector< cv::Point> > vecRects;
    int valMin2 = 220;
    int valMin3 = 230;
    while(valMin2 > 130 && valMin3 > 150 && vecRects.empty()) {
        cv::inRange(hsv, cv::Scalar(10, valMin2, valMin3), cv::Scalar(50, 255, 255), mask);
//        cv::inRange(hsv, minScalar, maxScalar, mask);
    //    imshow("win3", mask);
    //    imshow("win2", dst);
    //    return &m_cursorPan;
        std::vector< std::vector< cv::Point> > contours2;
        vecRects.clear();
        cv::findContours(mask, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    //    cv::drawContours(dst, contours2, -1, cvGreen, 2);
        for(size_t i = 0; i < contours2.size(); i++) {
            cv::Rect rectPush = cv::boundingRect(contours2[i]);
            double area = cv::contourArea(contours2[i]);
            if(area > 8000 && area < 11000 && rectPush.width > 250 ) {
//                qDebug() << rectPush.width << area;
                cv::drawContours(dst, contours2, i, cvBlue);
                vecRects.push_back(contours2[i]);
            }
        }
        if(valMin2 < 150) {
            valMin3 -= 15;
            valMin2 = 220;
        }

        valMin2 -= 15;
    }
//    qDebug() << "valMin2 =" << valMin2 << " valMin3 =" << valMin3;
    if(vecRects.empty() || vecRects.size() > 2) {
        cv::imshow("win3", mask);
        cv::imshow("win2", dst);

        qDebug() << "Шаблон прямоугольника dock menu не найден";
        return &m_cursorPan;
    }


    cv::Rect rectToRecogniz(cv::boundingRect(vecRects.front()));
    rectToRecogniz = cv::Rect(rectToRecogniz.x + 2, rectToRecogniz.y + 2, rectToRecogniz.width - 4, rectToRecogniz.height -4);
    cv::Mat maskToRecognize;
    mask(rectToRecogniz).copyTo(maskToRecognize);

    myOCRRus->SetImage( (uchar*)maskToRecognize.data, maskToRecognize.size().width, maskToRecognize.size().height, maskToRecognize.channels(), maskToRecognize.step1());
    myOCRRus->Recognize(nullptr);
    m_cursorPan.sMenuDocking = myOCRRus->GetUTF8Text();
    if(m_cursorPan.sMenuDocking.isEmpty())
        m_cursorPan.sMenuDocking = "not recognized";
    m_cursorPan.sMenuDocking = m_cursorPan.sMenuDocking.simplified();
    m_cursorPan.sMenuDocking = m_cursorPan.sMenuDocking.toLower();
    deleteCharExtra(m_cursorPan.sMenuDocking);
    if(comparisonStr(m_cursorPan.sMenuDocking, "службыкосмопорта") <= 2) {
        m_cursorPan.sMenuDocking = "menu_docking_service";
        m_cursorPan.activeMenuDocking = true;
    } else if(comparisonStr(m_cursorPan.sMenuDocking, "наповерхность") <= 2) {
        m_cursorPan.sMenuDocking = "menu_docking_angar";
        m_cursorPan.activeMenuDocking = true;
    } else if(comparisonStr(m_cursorPan.sMenuDocking, "вангар") <= 2) {
        m_cursorPan.sMenuDocking = "menu_docking_angar";
        m_cursorPan.activeMenuDocking = true;
    } else if(comparisonStr(m_cursorPan.sMenuDocking, "автозапуск") <= 2) {
        m_cursorPan.sMenuDocking = "menu_docking_autostart";
        m_cursorPan.activeMenuDocking = true;
    }
//    qDebug() << m_cursorPan.sMenuDocking;
    imshow("win3", mask);
    imshow("win2", dst);
    return &m_cursorPan;
}

cv::Rect CaptureWindow::getRect(QString sName)
{
    cv::Rect rect = mp_dataSet->at(sName.toStdString()).rect;
    return rect;
//    return QRect(rect.x, rect.y, rect.width, rect.height);
}

QPoint CaptureWindow::getPoint(QString sName)
{
    cv::Rect rect = mp_dataSet->at(sName.toStdString()).rect;
    return QPoint(rect.x + rect.width / 2, rect.y + rect.height / 2);
}

void CaptureWindow::enableResizeImage()
{
    if(DEBUG2)
        qDebug() << ".";
//    cv::namedWindow("win1", cv::WND_PROP_FULLSCREEN);
    resizeImage = resizeImage ? false : true;
    if(resizeImage) {
        cv::destroyWindow("win1");
//        cv::namedWindow("win1", cv::WND_PROP_AUTOSIZE);
//        cv::setWindowProperty("win1", cv::WND_PROP_AUTOSIZE, cv::WINDOW_GUI_NORMAL);
    } else {
        cv::destroyWindow("win1");
        cv::namedWindow("win1", cv::WND_PROP_FULLSCREEN);
        cv::setWindowProperty("win1", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
        cv::setMouseCallback("win1", my_mouse_callback, this);
        cv::moveWindow("win1", 0, -STANDART_FULLHD_HEIGHT);
    }
}

void CaptureWindow::enableSaveVideo()
{
    QString fName;
    saveVideo = saveVideo ? false : true;
    if(saveVideo) {
        fName = QString("%0\\%1.avi").arg(PATH_VIDEO_LOG).arg(QDateTime::currentDateTime().toString("dd-mmss"));
        writeVideo.open(fName.toStdString(), 0, 0, cv::Size(g_screen.width(), g_screen.height()), true);              // Open input
        if(!writeVideo.isOpened()) {
            qDebug()  << "couldn't open video: " << fName;
            saveVideo = false;
            return;
        }
        timeElapseForVideoSave.restart();
        timeElapseForVideoSave2.restart();
        qDebug() << "create video file " << fName;

    } else {
        writeVideo.release();
    }
    qDebug() << "save video is" << saveVideo;
}

void CaptureWindow::freeze()
{
    m_flowFrame = m_flowFrame ? false : true;
    win.copyTo(m_srcWin);
}

bool CaptureWindow::blackLessWhite(cv::Mat &aBinMat, int &anWhite, int &anBlack)       // Только для бинарных матриц, anWhite anBlack возврат результата если нужно
{
    if(DEBUG2)
        qDebug() << ".";
    anWhite = 0;
    anBlack = 0;
    for(int i = 0; i < aBinMat.rows; i++) {
        for(int j = 0; j < aBinMat.cols; j++) {
            if(aBinMat.at<uchar>(i,j) == 255)
                anWhite++;
            else if(aBinMat.at<uchar>(i,j) == 0)
                anBlack++;
        }
    }
    return anBlack < anWhite;
}

void CaptureWindow::setSide(int aSide)
{
    m_side = aSide;
}

void CaptureWindow::getPrepMatsForMenu(cv::Mat &aColorMat, cv::Mat &aMaskMat)
{
    if(DEBUG2)
        qDebug() << ".";
    win(cv::Rect(100, 100, g_screen.width() - 500, g_screen.height() - 200)).copyTo(aColorMat);
    cv::Mat hsv;
    cv::cvtColor(aColorMat, hsv, CV_BGR2HSV );
    cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), aMaskMat);
}

void CaptureWindow::getMaskOfMatHSV(cv::Mat &aColorMat, cv::Mat &aMaskMat, cv::Scalar aMinScalar, cv::Scalar aMaxScalar)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Mat hsvTmp;
    cv::cvtColor(aColorMat, hsvTmp, CV_BGR2HSV);
    cv::inRange(hsvTmp, aMinScalar, aMaxScalar, aMaskMat);
}

bool CaptureWindow::getMatsOfContour(cv::Mat &aColorMat, cv::Mat &aMaskMat, std::vector<cv::Point> contour)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::Mat rotateMat;
    aColorMat(cv::boundingRect(contour)).copyTo(rotateMat);
    cv::Mat mask;
    cv::Mat hsv;
    cv::cvtColor(rotateMat, hsv, CV_BGR2HSV);
    inRange(hsv, cv::Scalar(12, 190, 190), cv::Scalar(50, 255, 255), mask);                     // Получаем новую маску
    std::vector< std::vector< cv::Point> > contours2;
    cv::findContours(mask, contours2, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);  // из маски вытягиваем контуры
    std::vector<size_t> sortIdx(contours2.size());
    if(!getContoursIndexSort(mask, contours2, sortIdx)) {
        qDebug() << "Подконтуры не найдены в фунции getMaskOfContour";
        //cv::imshow("win4", rotateMat);
        //cv::imshow("win5", mask);
        return false;
    }
    cv::RotatedRect minRect = cv::minAreaRect(contours2[static_cast<size_t>(sortIdx[0])]);                     // Получаем угол на который нужно повернуть
    if(minRect.angle < -45)
        minRect.angle += static_cast<float>(90.);
    cv::Mat transformMat;
    cv::Mat rMatToWarp = cv::getRotationMatrix2D(minRect.center, static_cast<double>(minRect.angle), 1);
    cv::warpAffine(rotateMat, transformMat, rMatToWarp, transformMat.size(), cv::INTER_CUBIC);          // Поворачиваем картинку
    cv::Mat recognizeMask;
    cv::Mat recognizeHSV;
    cv::cvtColor(transformMat, recognizeHSV, CV_BGR2HSV);
    inRange(recognizeHSV, cv::Scalar(12, 190, 190), cv::Scalar(50, 255, 255), recognizeMask);                     // Получаем новую маску

    std::vector< std::vector< cv::Point> > cont_inside;
    std::vector<size_t> sortIdxInside(cont_inside.size());
    if(!getContoursIndexSort(recognizeMask, cont_inside, sortIdxInside)) {
        qDebug() << "Подконтуры не найдены в фунции getMaskOfContour во внутреннем прямоугольнике";
        //cv::imshow("win4", transformMat);
        //cv::imshow("win5", recognizeMask);
        return false;
    }
//    cv::rectangle(transformMat, cv::boundingRect(cont_inside[static_cast<size_t>(sortIdxInside[0])]), cvRad, 2);
    transformMat(cv::boundingRect(cont_inside[static_cast<size_t>(sortIdxInside[0])])).copyTo(aColorMat);
    cv::Mat recognizeHSVInside;
    cv::cvtColor(aColorMat, recognizeHSVInside, CV_BGR2HSV);
    inRange(recognizeHSVInside, cv::Scalar(12, 190, 190), cv::Scalar(50, 255, 255), aMaskMat);                     // Получаем новую маску
//    //cv::imshow("win3", transformMat);
//    //cv::imshow("win4", aColorMat);
//    //cv::imshow("win5", aMaskMat);
    return true;
}

bool CaptureWindow::getContoursIndexSort(cv::Mat &aMaskMat, std::vector<std::vector<cv::Point> > &contours2, std::vector<size_t> &idx, cv::RetrievalModes aAlgoType)
{
    if(DEBUG2)
        qDebug() << ".";
    cv::findContours(aMaskMat, contours2, cv::noArray(), aAlgoType, cv::CHAIN_APPROX_SIMPLE);  // из маски вытягиваем контуры
    if(contours2.empty())
        return false;
    idx.resize(contours2.size());
    std::vector<double> areas(contours2.size());
    for(size_t i = 0; i < contours2.size(); i++) {                               // Записываем площади контуров в вектор
        idx[i] = static_cast<int>(i);
        areas[i] = contourArea(contours2[i], false);                             // Не учитываем порядок вершин
    }
    std::sort( idx.begin(), idx.end(), AreaCmp(areas ));                        // Сортируем
    return true;
}

cv::Rect CaptureWindow::calcRectFromPartOfIndex(int anCount, int aiStart, int aiEnd)
{
//    if(DEBUG2)
//        qDebug() << ".";
    int partWidth = g_screen.width() / anCount;
    int partHeight = g_screen.height() / anCount;
//    qDebug() << USING_WIDTH - partWidth * anCount << " " << USING_HEIGHT - partHeight * anCount;
    int x1 = 1, y1 = 1, x2 = 0, y2 = 0;
    int i_start = 0;
    int ix_index = 0;
    int iy_index = 0;
    while(i_start <= aiEnd) {
        if(ix_index == anCount ) {
           iy_index++;
           ix_index = 0;
        }
        if(i_start == aiStart) {
            x1 = ix_index * partWidth;
            y1 = iy_index * partHeight;
        }
        if(i_start == aiEnd) {
            x2 = ix_index * partWidth;
            y2 = iy_index * partHeight;
        }
        ix_index++;
        i_start++;
    }
    x2 += partWidth;
    y2 += partHeight;
    cv::Rect cvRect = cv::Rect(x1, y1, x2 - x1, y2 - y1);
    return cvRect;
}
