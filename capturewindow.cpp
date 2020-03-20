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
    minNumber(0),
    maxNumber(255)
{
    mp_dataSet = ap_dataSet;


//    cv::namedWindow("win1", cv::WND_PROP_FULLSCREEN);
    cv::namedWindow("win1", cv::WND_PROP_AUTOSIZE);
    cv::moveWindow("win1", 0, -STANDART_FULLHD_HEIGHT);

//    cv::setWindowProperty("win1", cv::WND_PROP_FULLSCREEN, cv::WND_PROP_AUTOSIZE);
    cv::setMouseCallback("win1", my_mouse_callback, this);
//    cv::namedWindow("win2", cv::WND_PROP_AUTOSIZE );
//    cv::namedWindow("win3", cv::WND_PROP_AUTOSIZE );
//    cv::moveWindow("win2", 0, -STANDART_FULLHD_HEIGHT);
//    cv::moveWindow("win3", 0, -STANDART_FULLHD_HEIGHT/2);
//    cv::moveWindow("win4", m_screen.width() / 2, -STANDART_FULLHD_HEIGHT/2);
//    cv::moveWindow("win5", m_screen.width() / 2, -STANDART_FULLHD_HEIGHT/2);


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

}

CaptureWindow::~CaptureWindow()
{
//    qDebug() << "begin Capture exit in destructor";
    win.release();
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

//    panel1Body();
//    panelBodyCont();
//    panel1Header();
//    compass();
//    takeAimp();
//    subPanel1Nav();
//    menuDocking();
//    recognizDistance();

    cv::resize(win, win, win.size() / 2);
    imshow("win1", win);
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

bool CaptureWindow::srchAreaOnceInMat(std::string asImageROI, cv::Mat acvMat, double factor)
{
//    cv::Mat _matXRoi;
    cv::Point _point;
    cv::Mat _roiMat;
//    win(acvMat).copyTo(_matXRoi);
    _roiMat = mp_dataSet->at(asImageROI).mat;
    if(findPointRoi(_roiMat, acvMat, _point, factor)) {
        return true;
    } else
        return false;
}

cv::Point CaptureWindow::getPointAreaInMat(std::string asImageROI, cv::Mat acvMat, double factor)
{
    cv::Point _point = cv::Point(-1, -1);
    cv::Mat _roiMat;

    _roiMat = mp_dataSet->at(asImageROI).mat;
    qDebug() << QString::fromStdString(asImageROI);
    printMat(_roiMat);

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
        qDebug() << "error rows pattern > src";
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

bool CaptureWindow::srchAreaOnceInRect(std::string as_imageROI, cv::Rect acvRect, double coeff)
{
    cv::Mat _matXRoi;
    cv::Point _point;
    cv::Mat _roiMat;
    win(acvRect).copyTo(_matXRoi);

    _roiMat = mp_dataSet->at(as_imageROI).mat;


    if(findPointRoi(_roiMat, _matXRoi, _point, coeff)) {
        return true;
    } else
        return false;

}



void CaptureWindow::callBackMouse(int event, int x, int y, int flags)
{
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

bool CaptureWindow::srchAreaOnceInRect(std::string as_rectWhichInLook, std::string as_imageROI)
{
    cv::Mat _matXRoi;
    cv::Rect _rect = mp_dataSet->at(as_rectWhichInLook).rect;
    cv::Point _point;
    cv::Mat _roiMat;
    win(_rect).copyTo(_matXRoi);

    _roiMat = mp_dataSet->at(as_imageROI).mat;

    if(findPointRoi(_roiMat, _matXRoi, _point, MIN_COEFF_FOR_FIND)) {
        return true;
    } else
        return false;
}

cv::Point CaptureWindow::getPointAfterLookAreaOnceInRect(std::string as_rectWhichInLook, std::string as_imageROI)
{
    cv::Mat _matXRoi;
    cv::Rect _rect = mp_dataSet->at(as_rectWhichInLook).rect;
    cv::Point _point;
    cv::Mat _roiMat;
    win(_rect).copyTo(_matXRoi);

    _roiMat = mp_dataSet->at(as_imageROI).mat;

    if(findPointRoi(_roiMat, _matXRoi, _point, MIN_COEFF_FOR_FIND)) {
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

cv::Point CaptureWindow::getPointAfterLookAreaInRect(std::string asImageROI, cv::Rect acvRect)
{
    cv::Mat _matXRoi;
    cv::Point _point;
    cv::Mat _roiMat;
    win(acvRect).copyTo(_matXRoi);

    _roiMat = mp_dataSet->at(asImageROI).mat;

    if(findPointRoi(_roiMat, _matXRoi, _point, MIN_COEFF_FOR_FIND)) {
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

CursorPanel *CaptureWindow::panel1Body()
{
    m_cursorPan.activeBody = false;

    if(comparisonStr(m_cursorPan.sHeaderName, "навигация") <= 2) {
        return panelBodyNav();
    } else if(comparisonStr(m_cursorPan.sHeaderName, "контакты") <= 2 ) {
        return panelBodyCont();
    }
    return &m_cursorPan;
}

CursorPanel *CaptureWindow::panelBodyNav()
{
    qDebug() << "CursorPanel *CaptureWindow::panelBodyNav()";
    cv::Mat hsv;
    cv::Mat rectMat;
    win(cv::Rect(100, 100, m_screen.width() - 500, m_screen.height() - 200)).copyTo(rectMat);
    cv::cvtColor( rectMat, hsv, CV_BGR2HSV );
    cv::Mat mask;
    cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(mask, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector< std::vector< cv::Point> > vecRects;
    for(size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if(area < 1500)
            continue;
        if(area > 2500 && area < 7500 ) {
            cv::RotatedRect minRect = cv::minAreaRect(contours[i]);
            int widthLeftPanel = minRect.size.width > minRect.size.height ? minRect.size.width : minRect.size.height;
            if(widthLeftPanel > 90 && widthLeftPanel < 130) {
                //            putText(rectMat, QString::number(minRect[i].angle, 'f', 2).toStdString(), minRect[i].center, 1, 1, cvBlue, 2);
                vecRects.push_back(contours[i]);
            }
        }
        if(area > 20000 && area < 30000 ) {

//            cv::drawContours( rectMat, contours, static_cast<int>(i), cvBlue );
//            qDebug() << QString::number(area, 'f', 3);
            //            putText(rectMat, QString::number(minRect[i].angle, 'f', 2).toStdString(), minRect[i].center, 1, 1, cvBlue, 2);
            vecRects.push_back(contours[i]);
        }
//        qDebug() << QString::number(area, 'f', 3);
    }
    if(vecRects.size() == 1) {
        cv::drawContours( rectMat, vecRects, -1, cvBlue );
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

                        if(srchAreaOnceInMat("menuNavSetFilter", rColorRecognize, 0.95)) {
                            m_cursorPan.sBodyName = "push_filter";
                        } else if(srchAreaOnceInMat("menuNavResetFilter", rColorRecognize, 0.95)) {
                            m_cursorPan.sBodyName = "close_push_filter";
                        } else if(srchAreaOnceInMat("menuNavMapGalaxy", rColorRecognize, 0.95)) {
                            m_cursorPan.sBodyName = "push_galaxy_map";
                        } else if(srchAreaOnceInMat("menuNavMapSys", rColorRecognize, 0.95)) {
                            m_cursorPan.sBodyName = "push_system_map";
                        }
//                        //cv::imshow("win4", binMatToRecognize);
                        m_cursorPan.activeBody = true;

                    }
                }
            }
        }


//        ////cv::imshow("win5", rectMat);

//        qDebug() << "Левоя панель навигации" ;

    } else if(vecRects.size() == 2) {
        for(size_t i = 0; i < vecRects.size(); i++) {
            cv::RotatedRect minRect = cv::minAreaRect(vecRects[i]);
            int widthRightPanel = minRect.size.width > minRect.size.height ? minRect.size.width : minRect.size.height;

            if(widthRightPanel > 700) {
                cv::Mat rotateMat;
                int hightLine = minRect.size.width < minRect.size.height ? minRect.size.width : minRect.size.height;
                cv::Rect rectToCut(boundingRect(vecRects[i]));
                rectToCut.y -= hightLine / 2;
                rectToCut.height += hightLine / 2;
                rectMat(rectToCut).copyTo(rotateMat);
                if(minRect.angle < -45)
                    minRect.angle += 90.;
                cv::Mat rMatToWarp = cv::getRotationMatrix2D(minRect.center, minRect.angle, 1);
                cv::warpAffine(rotateMat, rotateMat, rMatToWarp, rotateMat.size(), cv::INTER_CUBIC);

                cv::Mat bin;
                cv::Mat rotateMatHSV;
                cv::cvtColor(rotateMat, rotateMatHSV, CV_BGR2HSV);
//                inRange(rotateMatHSV, minScalar, cv::Scalar(50, 255, 255), bin);
                inRange(rotateMatHSV, cv::Scalar(12, 190, 190), cv::Scalar(50, 255, 255), bin);
                std::vector< std::vector< cv::Point> > cont;
                cv::findContours(bin, cont, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                for(size_t i = 0; i < cont.size(); i++) {
                    double area = contourArea(cont[i]);
                    if(area > 19000 && area < 30000 ) {
//                        qDebug() << area;
//                        cv::drawContours( rotateMat, cont, static_cast<int>(i), cvBlue );
                        cv::Rect rectRecognize = cv::boundingRect(cont[i]);
                        cv::Mat binRecognize;

                        bin(cv::Rect(rectRecognize.x + 5, rectRecognize.y + 1, rectRecognize.width - 10, rectRecognize.height - 2)).copyTo(binRecognize);
                                    cv::Mat colorMapToSrchPattern;
                                    cv::cvtColor(binRecognize, colorMapToSrchPattern, cv::COLOR_GRAY2BGR);
                                    cv::Point point;
                                    int seekXDistance = colorMapToSrchPattern.size().width - 300;
                                    int cutXLeft = 0;
                                    for(int i = 0; i < sNamePicNavList.size(); i++) {
                                        printMat(colorMapToSrchPattern);
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


                        myOCREng->SetImage( (uchar*)binCut.data, binCut.size().width, binCut.size().height, binCut.channels(), binCut.step1());
                        myOCREng->Recognize(nullptr);
                        m_cursorPan.sBodyName = myOCREng->GetUTF8Text();
                        if(m_cursorPan.sBodyName.isEmpty())
                            m_cursorPan.sBodyName = "not recognized";
                        m_cursorPan.sBodyName = m_cursorPan.sBodyName.simplified();
                        m_cursorPan.sBodyName = m_cursorPan.sBodyName.toLower();
                        deleteCharExtra(m_cursorPan.sBodyName);
                        m_cursorPan.activeBody = true;

                        qDebug() << m_cursorPan.sBodyName;
                        ////cv::imshow("win4", binCut);
                        //cv::imshow("win5", binRecognize);
                    }
                }

            }
        }


    } else {
//        qDebug() << "Панели навигации не определены";
    }
//    //cv::imshow("win3", mask);
//    //cv::imshow("win2", rectMat);
    return &m_cursorPan;
}

CursorPanel *CaptureWindow::panelBodyCont()
{
    cv::Mat hsv;
    cv::Mat rectMat;
    win(cv::Rect(100, 100, m_screen.width() - 500, m_screen.height() - 200)).copyTo(rectMat);
    cv::cvtColor( rectMat, hsv, CV_BGR2HSV );
    cv::Mat mask;
    cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(mask, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector< std::vector< cv::Point> > vecRects;
    for(size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if(area < 1500)
            continue;
        if(area > 16000 && area < 30000 ) {
            vecRects.push_back(contours[i]);
        }
    }
    if(vecRects.size() == 1) {
        // Вставка на обрезку прокрутки
        if(cv::boundingRect(vecRects[0]).height > 400) {
            cv::Rect tmp = cv::boundingRect(vecRects[0]);
            tmp.width -= 50;
            mask(tmp).copyTo(mask);
            rectMat(tmp).copyTo(rectMat);
            contours.clear();
            cv::findContours(mask, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            vecRects.clear();
            for(size_t i = 0; i < contours.size(); i++) {
                double area = cv::contourArea(contours[i]);
                if(area < 1500)
                    continue;
                if(area > 16000 && area < 25000 ) {
                    vecRects.push_back(contours[i]);
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
                        bin(cv::Rect(rectRecognize.x + 5, rectRecognize.y + 1, rectRecognize.width - 10, rectRecognize.height - 2)).copyTo(binRecognize);


                        myOCREng->SetImage( (uchar*)binRecognize.data, binRecognize.size().width, binRecognize.size().height, binRecognize.channels(), binRecognize.step1());
                        myOCREng->Recognize(nullptr);
                        m_cursorPan.sBodyName = myOCREng->GetUTF8Text();
                        if(m_cursorPan.sBodyName.isEmpty())
                            m_cursorPan.sBodyName = "not recognized";
                        m_cursorPan.sBodyName = m_cursorPan.sBodyName.simplified();
                        m_cursorPan.sBodyName = m_cursorPan.sBodyName.toLower();
                        deleteCharExtra(m_cursorPan.sBodyName);
                        m_cursorPan.activeBody = true;

                        //cv::imshow("win4", binRecognize);
                    }
                }

            } else if(rectToCut.width < 500 && rectToCut.width > 300) {
                m_cursorPan.sBodyName = "req_docking";
                m_cursorPan.activeBody = true;

            }
        }
    }  else {
//        qDebug() << "Панели навигации не определены";
    }
    qDebug() << m_cursorPan.sBodyName;
    //cv::imshow("win3", mask);
    //cv::imshow("win2", rectMat);
    return &m_cursorPan;
}

CursorPanel *CaptureWindow::panel1Header()
{
    m_cursorPan.activeHeader = false;
    cv::Mat hsv;
    cv::Mat rectMat;
    win(cv::Rect(100, 100, m_screen.width() - 500, m_screen.height() - 200)).copyTo(rectMat);
    cv::cvtColor( rectMat, hsv, CV_BGR2HSV );
    cv::Mat mask;
    cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);

    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(mask, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//    cv::drawContours(rectMat, contours, -1, cvGreen, 2);
    std::vector< std::vector< cv::Point> > vecRects;
    for(size_t i = 0; i < contours.size(); i++) {
        cv::Rect push = cv::boundingRect(contours[i]);
        if(push.width < 190 || push.width > 310)
            continue;
        double area = cv::contourArea(contours[i]);
        if(area > 5000 && area < 15000) {
//            qDebug() << push.width << area;
            vecRects.push_back(contours[i]);
        }
    }
    if(vecRects.empty() || vecRects.size() > 2) {
            //cv::imshow("win4", mask);
            //cv::imshow("win2", rectMat);

//        qDebug() << "Шаблон прямоугольника header menu не найден";
        return &m_cursorPan;
    }


    cv::Mat rotateMat;
    cv::RotatedRect rotRect = cv::minAreaRect(vecRects[0]);
    cv::Rect rectToCut(boundingRect(vecRects[0]));
    int hightLine = rotRect.size.width < rotRect.size.height ? rotRect.size.width : rotRect.size.height;
    rectToCut.y -= hightLine / 2;
    rectToCut.height += hightLine / 2;
    rectMat(rectToCut).copyTo(rotateMat);
    if(rotRect.angle < -45)
        rotRect.angle += 90.;
    cv::Mat rMatToWarp = cv::getRotationMatrix2D(rotRect.center, rotRect.angle, 1);
    cv::warpAffine(rotateMat, rotateMat, rMatToWarp, rotateMat.size(), cv::INTER_CUBIC);

//    //cv::imshow("win4", mask);
//    //cv::imshow("win2", rotateMat);
//    return &m_cursorPan;


    cv::Mat bin;
    cv::Mat rotateHSV;
    cv::cvtColor( rotateMat, rotateHSV, CV_BGR2HSV );

    inRange(rotateHSV, cv::Scalar(7, 190, 190), cv::Scalar(50, 255, 255), bin);
    std::vector<std::vector<cv::Point> > cont;

    cv::Mat matToRcoginze;
    findContours(bin, cont, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if(cont.empty()) {
//        qDebug() << "Шаблон прямоугольника header menu не найден, в суб прямоугольнике";
        return &m_cursorPan;
    }
//    cv::drawContours(rectMat, cont, -1, cvGreen, 2);
    for(size_t i = 0; i < cont.size(); i++) {
        double area = cv::contourArea(cont[i]);

        if(area > 5000 && area < 17000) {

//            cv::drawContours(rotateMat, cont, static_cast<int>(i), cvGreen, 2);
            cv::Rect rectRecognize = cv::boundingRect(cont[i]);
            bin(cv::Rect(rectRecognize.x + 5, rectRecognize.y + 1, rectRecognize.width - 10, rectRecognize.height - 2)).copyTo(matToRcoginze);
//            myOCRRus->SetVariable("user_defined_dpi", "300");
//            tesseract::TessBaseAPI tess;
//                tess.Init(NULL, "eng", tesseract::OEM_DEFAULT);
//                tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
//                tess.SetVariable("tessedit_char_whitelist", "QWERTYUIOPASDFGHJKLZXCVBNM");
            myOCRRus->SetImage( (uchar*)bin.data, bin.size().width, bin.size().height, bin.channels(), bin.step1()); // !!! Почему бин а не matToRcoginze
//            myOCRRus->SetSourceResolution();

//            myOCRRus->setT
            myOCRRus->Recognize(nullptr);
            m_cursorPan.sHeaderName = myOCRRus->GetUTF8Text();
            if(m_cursorPan.sHeaderName.isEmpty())
                m_cursorPan.sHeaderName = "not recognized";
            m_cursorPan.sHeaderName = m_cursorPan.sHeaderName.simplified();
            m_cursorPan.sHeaderName = m_cursorPan.sHeaderName.toLower();
            deleteCharExtra(m_cursorPan.sHeaderName);
            if(comparisonStr(m_cursorPan.sHeaderName, "службыкосмопорта") <= 2)
                return &m_cursorPan;
            if(comparisonStr(m_cursorPan.sHeaderName, "наповерхность") <= 2)
                return &m_cursorPan;
            if(comparisonStr(m_cursorPan.sHeaderName, "вангар") <= 2)
                return &m_cursorPan;
            if(comparisonStr(m_cursorPan.sHeaderName, "автозапуск") <= 2)
                return &m_cursorPan;
            m_cursorPan.activeHeader = true;

            qDebug() << m_cursorPan.sHeaderName ;
//            //cv::imshow("win3", matToRcoginze);
        }
    }
//    //cv::imshow("win4", bin);
//    //cv::imshow("win2", rectMat);
    return &m_cursorPan;
}

std::vector<cv::Point> CaptureWindow::findPoints(std::vector< cv::Point>  contours, cv::Mat &dst) {
    std::vector<cv::Point> vec;
    std::vector<cv::Point> min_x;
    min_x.push_back(cv::Point(STANDART_HD_WIDTH, STANDART_HD_HEIGHT));
    std::vector<cv::Point> max_x;
    max_x.push_back(cv::Point(0, 0));
    std::vector<cv::Point> min_y;
    min_y.push_back(cv::Point(STANDART_HD_WIDTH, STANDART_HD_HEIGHT));
    std::vector<cv::Point> max_y;
    max_y.push_back(cv::Point(0, 0));
    for(size_t i = 0; i < contours.size(); i++) {
        if(contours[i].x < min_x.at(0).x) {
            min_x.clear();
            min_x.push_back(contours[i]);
        } else if(contours[i].x == min_x.at(0).x) {
            min_x.push_back(contours[i]);
        }
        if(contours[i].x > max_x.at(0).x) {
            max_x.clear();
            max_x.push_back(contours[i]);
        } else if(contours[i].x == max_x.at(0).x){
            max_x.push_back(contours[i]);
        }
        if(contours[i].y < min_y.at(0).y) {
            min_y.clear();
            min_y.push_back(contours[i]);
        } else if(contours[i].y == min_y.at(0).y)
        {
            min_y.push_back(contours[i]);
        }
        if(contours[i].y > max_y.at(0).y) {
            max_y.clear();
            max_y.push_back(contours[i]);
        } else if(contours[i].y == max_y.at(0).y){
            max_y.push_back(contours[i]);
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
    std::vector< std::vector< cv::Point> > contoursSrc;
    std::vector< cv::Vec4i > hierarchy;
    cv::findContours(binPrep, contoursSrc, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect> vecRects;
    std::vector<cv::Rect> vecRectsShort;
    getRectsOfPatternMenu1(contoursSrc, vecRects, vecRectsShort);
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
    std::vector< std::vector< cv::Point> > contoursSrc;
    std::vector< cv::Vec4i > hierarchy;
    cv::findContours(binPrep, contoursSrc, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect> vecRects;
    getRectsOfPatternMenu1SubNav(contoursSrc, vecRects);
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
        int partWidth = m_screen.width() / m_nCountCell;
        int partHeight = m_screen.height() / m_nCountCell;
    //    qDebug() << USING_WIDTH - partWidth * anCount << " " << USING_HEIGHT - partHeight * anCount;
        int x1 = 1, y1 = 1, x2 = 0, y2 = 0;
        while(x1 < m_screen.width()) {
            cv::line(win, cv::Point(x1, 0), cv::Point(x1, m_screen.height()), cvScalar(0x0, 0xFF, 0x0));
            x1 += partWidth;
        }

        while(y1 < m_screen.height()) {
            cv::line(win, cv::Point(0, y1), cv::Point(m_screen.width(), y1), cvScalar(0x0, 0xFF, 0x0));
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
            cv::putText(win, QString::number(i_start).toStdString(), p1,  cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0x0, 0xFF, 0x0), 4);

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

bool CaptureWindow::findPointRoi(cv::Mat &t_mat, cv::Mat &t_whereFind, cv::Point &t_point, double t_factor)
{
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
            qDebug() << "error rows pattern > src";
            return false;
        }
        cv::matchTemplate(t_whereFind, t_mat, result[i], const_vec[i]);
        _f.method = const_vec[i];
        cv::minMaxLoc(result[i], &_f.m1, &_f.m2, &_f.p1, &_f.p2);
        vec.push_back(_f);
    }
    for(auto tmp: vec) {
        if(tmp.m2 < 0.9)
            qDebug() << tmp.m2;
    }
    cv::Point match = findMatchPoint(vec, t_factor);
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

cv::Point CaptureWindow::findMatchPoint(std::vector<VarForTemplateMatch> _vec, double t_factor)
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
    _vec.erase(std::remove_if(_vec.begin(), _vec.end(), [_vec, t_factor] (VarForTemplateMatch aItem) -> bool {
                   return aItem.m2 < t_factor;
               }), _vec.end());
//    qDebug() << QString::number(_vec[0].m1, 'f', 2) << QString::number(_vec[0].m2, 'f', 2);
    if(_vec.size() == 2) {
        return _vec[0].m2 > _vec[1].m2 ? _vec[0].p2 : _vec[0].p2;
    } else if(_vec.size() == 1 && _vec[0].m2 >  t_factor) {
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

CursorTarget *CaptureWindow::takeAimp()
{

    cv::Rect fieldSpace(calcRectFromPartOfIndex(11, 25, 84));
    cv::Mat dst;
    win(fieldSpace).copyTo(dst);
    m_cursorTarget.active = false;
    m_cursorTarget.rectSrc = fieldSpace;
    m_cursorTarget.pointCenter = cv::Point(fieldSpace.width / 2, fieldSpace.height / 2);
    cv::circle(dst, m_cursorTarget.pointCenter, 3, cv::Scalar(0, 255, 0));
    cv::Mat mask(dst.size(), CV_8U, 1);
    cv::Mat hsv;
    cv::cvtColor(dst, hsv, CV_BGR2HSV);
    int targetSatMax = 246;
    while(!m_cursorTarget.active && targetSatMax <= 255) {
        //    cv::inRange(hsv, minScalar, maxScalar, mask);                         // hsv
            cv::inRange(hsv, cv::Scalar(15, 43, 117), cv::Scalar(48, targetSatMax, 253), mask);                         // hsv
        //    cv::inRange(dst, cv::Scalar(0, 106, 179), cv::Scalar(22, 255, 255), mask);                    // bgr
        //    cv::Mat maskBlur;
        //    cv::blur(mask, maskBlur, cv::Size(3, 3));
            std::vector< std::vector< cv::Point> > contoursSrc;
            std::vector< cv::Vec4i > hierarchy;
            cv::findContours(mask, contoursSrc, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
            std::vector<cv::Rect> vecRects;
            getRectsInContour(contoursSrc, vecRects);
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
            targetSatMax++;
    }

    cv::circle(dst, m_cursorTarget.pointTarget, 3, cv::Scalar(0, 255, 0));



//    imshow("win4", maskBlur);
    imshow("win2", mask);
    imshow("win3", dst);
    return &m_cursorTarget;
}

Compass *CaptureWindow::compass()
{
#define     COMPAS_SIDE     90
    m_compas.active = false;
    cv::Rect compasRect;        // Основной квадрат компаса
    cv::Mat dst;
    cv::Rect fieldCompass(calcRectFromPartOfIndex(27, 467, 660));
    win(fieldCompass).copyTo(dst);



    //  ---------------------------       Поиск кругов       ----------------------
    cv::Mat bin(dst.size(), CV_8U, 1);
    cv::Mat img_blur;
    int hue = 15;
    std::vector<cv::Vec3f> circles;
    std::vector<cv::Rect> compassRects;
    while (hue > 9) {
        circles.clear();    compassRects.clear();
        bin.deallocate();
        img_blur.deallocate();
        getMaskOfMat(dst, bin, cv::Scalar(hue, 150, 126), cv::Scalar(27, 243, 254));
        std::vector< std::vector <cv::Point> > cont;
        std::vector<int > sort_idx;
        cv::blur(bin, img_blur, cv::Size(3, 3));
        cv::HoughCircles(img_blur, circles, cv::HOUGH_GRADIENT, 1, dst.rows / 1, 200, 15, 25, 35);
        if(circles.empty())  {
//            qDebug() << " hue =" << hue;
//            imshow("win5", dst);
//            imshow("win4", img_blur);
            hue--;
        } else
            break;
    }
    if(circles.empty()) {
        qDebug() << "Круги компаса не найдены hue =" << hue;
        return &m_compas;
    }

//    imshow("win3", dst);
//    imshow("win4", bin);
//    imshow("win5", img_blur);
//    return &m_compas;

            // Убираем лишние круги
    for(size_t i = 0; i < circles.size(); i++) {
        cv::Rect rectFound(cvRound(circles[i][0]) - COMPAS_SIDE/2, cvRound(circles[i][1]) - COMPAS_SIDE/2, COMPAS_SIDE, COMPAS_SIDE);
        if(rectFound.x < 0 || rectFound.y < 0
                || rectFound.x + rectFound.width > dst.size().width
                || rectFound.y + rectFound.height > dst.size().height) {
            qDebug() << "noise";
            continue;
        }
        compassRects.push_back(rectFound);
//        cv::rectangle(dst, rectFound, cv::Scalar(255, 0, 0), 2);
    }


    //  ---------------------------       Поиск по паттерну       ----------------------
    std::vector<cv::Rect> vecRects;
    std::vector<size_t> fact_idx;
    std::vector<double> factors;
    for(size_t i = 0; i < compassRects.size(); i++) {
        cv::Mat compasMatSrc;
        cv::Mat oreolCompasMask;
        cv::Mat oreolCompasMaskGray;
        dst(compassRects[0]).copyTo(compasMatSrc);
        getMaskOfMat(compasMatSrc, oreolCompasMask, cv::Scalar(15, 148, 126), cv::Scalar(27, 243, 254));
        cv::cvtColor(oreolCompasMask, oreolCompasMaskGray, cv::COLOR_GRAY2BGR);
        double factor = 0.1;
        cv::Point srchPoint;
        srchPoint = getPointAndFactorOfSPattern(oreolCompasMaskGray, "compasOreol", factor);
        fact_idx.push_back(i);
        factors.push_back(factor);
        vecRects.push_back(cv::Rect(compassRects[i].x + srchPoint.x - COMPAS_SIDE/2, compassRects[i].y + srchPoint.y - COMPAS_SIDE/2, COMPAS_SIDE, COMPAS_SIDE));
    }
    if(vecRects.empty()) {
        cv::drawContours(dst, vecRects, -1, cvGreen, 1);
        qDebug() << "Паттерн компаса в найденом круге не найдены";
//        imshow("win5", img_blur);
        imshow("win4", bin);
        imshow("win2", dst);
        return &m_compas;
    }

    std::sort( fact_idx.begin(), fact_idx.end(), FactorCmp(factors ));                        // Сортируем
    compasRect = vecRects[fact_idx[0]];
    if(compasRect.x < 0 || compasRect.y < 0
            || compasRect.x + compasRect.width > dst.size().width
            || compasRect.y + compasRect.height > dst.size().height) {
        qDebug() << "Паттерн компаса в найденом круге не найдены";
        imshow("win5", img_blur);
        imshow("win4", bin);
        imshow("win2", dst);
        return &m_compas;
    }
//    qDebug() << factors[fact_idx[0]];



    //**********************       Поиск точки      **************************
    cv::Mat compasMat;
    cv::Mat compasMask;
    cv::Mat compasBlueMask;
    cv::Vec3f circl;
    circl[0] = compasRect.width / 2;
    circl[1] = compasRect.height / 2;
    circl[2] = 30;

    dst(compasRect).copyTo(compasMat);
    cv::rectangle(dst, compasRect, cvBlue, 2);
    bool foundDot = false;
    int satDot = 0;
    std::vector<cv::Point> dotContour;
    while(satDot < 100 && !foundDot) {
//        getMaskOfMat(compasMat, compasMask, minScalar, maxScalar);
        getMaskOfMat(compasMat, compasMask, cv::Scalar( 0 , satDot , 0), cv::Scalar(21 , 255 , 255));
        std::vector <std::vector <cv::Point> > compasContours;
        std::vector<size_t> idx;
        cv::bitwise_not(compasMask, compasMask);
        cv::medianBlur(compasMask, compasBlueMask, 3);
        if(!getContoursIndexSort(compasBlueMask, compasContours, idx)) {

        }
//        cv::drawContours(compasMat, compasContours, -1, cvGreen);
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
    }
    int valDot = 250;
    while(valDot > 100 && !foundDot) {
//        getMaskOfMat(compasMat, compasMask, minScalar, maxScalar);
        getMaskOfMat(compasMat, compasMask, cv::Scalar( 0 , 0 , valDot), cv::Scalar(255 , 255 , 255));
        std::vector <std::vector <cv::Point> > compasContours;
        std::vector<size_t> idx;
//        cv::bitwise_not(compasMask, compasMask);
        cv::medianBlur(compasMask, compasBlueMask, 3);
        if(!getContoursIndexSort(compasBlueMask, compasContours, idx, cv::RETR_LIST)) {

        }
//        cv::drawContours(compasMat, compasContours, -1, cvGreen);
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
    }
    if(!foundDot) {
        imshow("win2", dst);
        imshow("win3", compasMat);
        imshow("win4", compasBlueMask);
        imshow("win5", compasMask);
        qDebug() << "Контуры точки не найдены sat =" << satDot << " val =" << valDot;
        return &m_compas;
    }
//    cv::circle(compasMat, cv::minAreaRect(dotContour).center, 2, cvRad, 2);


    cv::RotatedRect dotRot = cv::minAreaRect(dotContour);
    cv::Rect dotRect = cv::boundingRect(dotContour);
    cv::Mat dotMat;
    compasMat(dotRect).copyTo(dotMat);
    cv::circle(compasMat, dotRot.center, 2, cvBlue, 2);
    m_compas.pDot = dotRot.center;
    m_compas.pCenter = cv::Point(compasRect.width/2, compasRect.height/2);

    cv::Mat dotMask;
    getMaskOfMat(dotMat, dotMask, cv::Scalar( 0 , 75 , 0), cv::Scalar(27 , 255 , 255));
    //    getMaskOfMat(dotMat, dotMask, minScalar, maxScalar);

        cv::bitwise_not(dotMask, dotMask);
    //    dotMask = !dotMask;

//    getMaskOfMat(compasMat, dotMask, cv::Scalar(80, 0, 110), cv::Scalar(151, 186, 255));
    std::vector< std::vector <cv::Point> > dotContours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dotMask, dotContours, hierarchy, cv::RETR_CCOMP,  cv::CHAIN_APPROX_SIMPLE);
    if(dotContours.empty())  {
        qDebug() << "Контуры точки не найдены";
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
    m_compas.active = true;
//    qDebug() << "face" << m_compas.face;
//    qDebug() << hierarchy[idx_dot.front()][0] << hierarchy[idx_dot.front()][1]  << hierarchy[idx_dot.front()][2]  << hierarchy[idx_dot.front()][3];
    cv::drawContours(dotMat, dotContours, hierarchy[idx_dot.front()][2], cvGreen, 1);
//    cv::circle(compasMat, dot.center, 2, cvGreen, 2);
    imshow("win2", dst);
    cv::resize(dotMask, dotMask, dotMask.size() * 5);

    imshow("win3", compasMat);
    cv::resize(dotMat, dotMat, dotMat.size() * 5);
    imshow("win4", dotMat);
    imshow("win5", dotMask);
    return &m_compas;
}

Distance *CaptureWindow::recognizDistance()
{
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
            std::vector< std::vector< cv::Point> > contoursSrc;
            std::vector< cv::Vec4i > hierarchy;
            cv::findContours(mask, contoursSrc, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
            std::vector<cv::Rect> vecRects;
            getRectsInContour(contoursSrc, vecRects);
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
    getMaskOfMat(recognizMat, recognizMask, minScalar, maxScalar);


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


bool CaptureWindow::checkContour(std::vector<cv::Point> &acvVecPointCont, cv::Mat &cvMatOut)
{
    cv::Point min_x;
    min_x.x = m_screen.width();
    cv::Point max_x;
    max_x.x = 0;
    cv::Point min_y;
    min_y.y = m_screen.height();
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
    vecRects.clear();
    for(size_t j = 0; j < acvVecPointCont.size(); j++) {
        std::vector<cv::Point> vectmp = acvVecPointCont[j];
        if(vectmp.size() < 3)
            continue;
        cv::Point min_x;
        min_x.x = m_screen.width();
        cv::Point max_x;
        max_x.x = 0;
        cv::Point min_y;
        min_y.y = m_screen.height();
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
    vecRectsShort.clear();
    vecRects.clear();
    for(size_t j = 0; j < acvVecPointCont.size(); j++) {
        std::vector<cv::Point> vectmp = acvVecPointCont[j];
        if(vectmp.size() < 3)
            continue;
        cv::Point min_x;
        min_x.x = m_screen.width();
        cv::Point max_x;
        max_x.x = 0;
        cv::Point min_y;
        min_y.y = m_screen.height();
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
    vecRects.clear();
    for(size_t j = 0; j < acvVecPointCont.size(); j++) {
        std::vector<cv::Point> vectmp = acvVecPointCont[j];
        if(vectmp.size() < 3)
            continue;
        cv::Point min_x;
        min_x.x = m_screen.width();
        cv::Point max_x;
        max_x.x = 0;
        cv::Point min_y;
        min_y.y = m_screen.height();
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
    nXLeft = m_screen.width();
    nYUp = m_screen.height();
    for(size_t i = 0; i < vecRects.size(); i++) {
        if(vecRects[i].x < nXLeft)
            nXLeft = vecRects[i].x;
        if(vecRects[i].y < nYUp)
            nYUp = vecRects[i].y;
    }

    if(nXLeft == m_screen.width())
        return false;
    if(nYUp == m_screen.height())
        return false;
    return true;
}

cv::Rect CaptureWindow::getRectInContour(std::vector<cv::Point> &acvVecPoint)
{
    if(acvVecPoint.size() < 3)
        return cv::Rect(0, 0, 0, 0);
    cv::Point min_x;
    min_x.x = m_screen.width();
    cv::Point max_x;
    max_x.x = 0;
    cv::Point min_y;
    min_y.y = m_screen.height();
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
    std::vector< std::vector< cv::Point> > contoursSrc;
    std::vector< std::vector< cv::Point> > contMoreLength;
    std::vector< cv::Vec4i > hierarchy;
    cv::findContours(binMat, contoursSrc, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> vecRects;
    getRectsInContour(contoursSrc, vecRects);

    return vecRects;
}

CursorPanel *CaptureWindow::subPanel1Nav()
{
    m_cursorPan.rectSubNavList.y = -1;
    m_cursorPan.activeSubNav = false;
    cv::Mat dst;
    cv::Mat mask;
    getPrepMatsForMenu(dst, mask);
    std::vector <std::vector <cv::Point> > contours;
    std::vector<size_t> idx;
    if(comparisonStr(m_cursorPan.sHeaderName, "навигация") <= 2) {
        if(getContoursIndexSort(mask, contours, idx)) {
            for(size_t i = 0; i < 1; i++) {

                cv::Rect cmpRect = cv::boundingRect(contours[static_cast<size_t>(idx[i])]);
                if(cmpRect.width > 250 || cmpRect.height > 200)
                    continue;
                if(cmpRect.width < 70 || cmpRect.height < 40)
                    continue;

                cv::Mat recognizeMask;      // Искомая маска
                if(getMatsOfContour(dst, recognizeMask, contours[static_cast<size_t>(idx[i])])) {
                    cv::cvtColor(recognizeMask, recognizeMask, cv::COLOR_GRAY2BGR);
                    if(srchAreaOnceInMat(subNamePicMenu1NavList[0].toStdString(), recognizeMask, 0.97)) {
                        m_cursorPan.sSubNavName = "fix_target";
                    } else if(srchAreaOnceInMat(subNamePicMenu1NavList[1].toStdString(), recognizeMask, 0.985)) {
                        m_cursorPan.sSubNavName = "unfix_target";
                    } else if(srchAreaOnceInMat(subNamePicMenu1NavList[2].toStdString(), recognizeMask, 0.985)) {
                        m_cursorPan.sSubNavName = "enable_hypermode";
                    } else if(srchAreaOnceInMat(subNamePicMenu1NavList[3].toStdString(), recognizeMask, 0.985)) {
                        m_cursorPan.sSubNavName = "enable_hypermode_he1lper";
                    } else {
                        m_cursorPan.sSubNavName = "missed_the_mark";
                    }
                    qDebug() << m_cursorPan.sSubNavName;
                    m_cursorPan.activeSubNav = true;
                    //cv::imshow("win5", recognizeMask);
                    break;
                } else {
                    qDebug() << "contour not found";
                    imshow("win5", mask);

                }


            }
        } else {
            qDebug() << "contour not found";
            imshow("win5", mask);

        }
    } else {
        qDebug() << "Последнее включенное меню было" << m_cursorPan.sHeaderName;
    }


    imshow("win4", dst);

    return &m_cursorPan;
}

void CaptureWindow::transformSubMenu(cv::Mat &acvMat)
{
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

    m_cursorPan.activeMenuDocking = false;
    cv::Mat hsv;
    cv::Mat dst;
    win(cv::Rect(810, 805, 310, 130)).copyTo(dst);
    cv::cvtColor( dst, hsv, CV_BGR2HSV );
    cv::Mat mask;
    cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), mask);

//    imshow("win3", mask);
//    imshow("win2", dst);
//    return &m_cursorPan;



    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(mask, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//    cv::drawContours(dst, contours, -1, cvGreen, 2);
    std::vector< std::vector< cv::Point> > vecRects;
    for(size_t i = 0; i < contours.size(); i++) {
        cv::Rect rectPush = cv::boundingRect(contours[i]);
        double area = cv::contourArea(contours[i]);
        if(area > 5000 && area < 15000 && rectPush.width > 250 ) {
//            qDebug() << rectPush.width << area;
            cv::drawContours(dst, contours, i, cvBlue);
            vecRects.push_back(contours[i]);
        }
    }
    if(vecRects.empty() || vecRects.size() > 2) {
            //cv::imshow("win3", mask);
            //cv::imshow("win2", dst);

//        qDebug() << "Шаблон прямоугольника dock menu не найден";
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

bool CaptureWindow::blackLessWhite(cv::Mat &aBinMat, int &anWhite, int &anBlack)       // Только для бинарных матриц
{
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

void CaptureWindow::getPrepMatsForMenu(cv::Mat &aColorMat, cv::Mat &aMaskMat)
{
    win(cv::Rect(100, 100, m_screen.width() - 500, m_screen.height() - 200)).copyTo(aColorMat);
    cv::Mat hsv;
    cv::cvtColor(aColorMat, hsv, CV_BGR2HSV );
    cv::inRange(hsv, cv::Scalar(10, 210, 230), cv::Scalar(50, 255, 255), aMaskMat);
}

void CaptureWindow::getMaskOfMat(cv::Mat &aColorMat, cv::Mat &aMaskMat, cv::Scalar aMinScalar, cv::Scalar aMaxScalar)
{
    cv::Mat hsvTmp;
    cv::cvtColor(aColorMat, hsvTmp, CV_BGR2HSV);
    cv::inRange(hsvTmp, aMinScalar, aMaxScalar, aMaskMat);
}

bool CaptureWindow::getMatsOfContour(cv::Mat &aColorMat, cv::Mat &aMaskMat, std::vector<cv::Point> contour)
{
    cv::Mat rotateMat;
    aColorMat(cv::boundingRect(contour)).copyTo(rotateMat);
    cv::Mat mask;
    cv::Mat hsv;
    cv::cvtColor(rotateMat, hsv, CV_BGR2HSV);
    inRange(hsv, cv::Scalar(12, 190, 190), cv::Scalar(50, 255, 255), mask);                     // Получаем новую маску
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(mask, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);  // из маски вытягиваем контуры
    std::vector<size_t> sortIdx(contours.size());
    if(!getContoursIndexSort(mask, contours, sortIdx)) {
        qDebug() << "Подконтуры не найдены в фунции getMaskOfContour";
        //cv::imshow("win4", rotateMat);
        //cv::imshow("win5", mask);
        return false;
    }
    cv::RotatedRect minRect = cv::minAreaRect(contours[static_cast<size_t>(sortIdx[0])]);                     // Получаем угол на который нужно повернуть
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

bool CaptureWindow::getContoursIndexSort(cv::Mat &aMaskMat, std::vector<std::vector<cv::Point> > &contours, std::vector<size_t> &idx, cv::RetrievalModes aAlgoType)
{
    cv::findContours(aMaskMat, contours, cv::noArray(), aAlgoType, cv::CHAIN_APPROX_SIMPLE);  // из маски вытягиваем контуры
    if(contours.empty())
        return false;
    idx.resize(contours.size());
    std::vector<double> areas(contours.size());
    for(size_t i = 0; i < contours.size(); i++) {                               // Записываем площади контуров в вектор
        idx[i] = static_cast<int>(i);
        areas[i] = contourArea(contours[i], false);                             // Не учитываем порядок вершин
    }
    std::sort( idx.begin(), idx.end(), AreaCmp(areas ));                        // Сортируем
    return true;
}

cv::Rect CaptureWindow::calcRectFromPartOfIndex(int anCount, int aiStart, int aiEnd)
{
    int partWidth = m_screen.width() / anCount;
    int partHeight = m_screen.height() / anCount;
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
