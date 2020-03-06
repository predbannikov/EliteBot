#include "capturewindow.h"
#include "algorithm"
#include "qmath.h"

#define     MIN_COEFF_FOR_FIND              0.99
#define     MIN_COEFF_FOR_FIND_POINT        0.7

#define CURSOR_DELTA     6


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
    maxScalar(cv::Scalar(255, 255, 255)),
    maxContours(50)
{
    mp_dataSet = ap_dataSet;

    cv::namedWindow("win1", cv::WND_PROP_FULLSCREEN);
    cv::moveWindow("win1", 0, -STANDART_FULLHD_HEIGHT);
    cv::setWindowProperty("win1", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    cv::setMouseCallback("win1", my_mouse_callback, this);
    cv::namedWindow("win2", cv::WND_PROP_AUTOSIZE );
    cv::namedWindow("win3", cv::WND_PROP_AUTOSIZE );
    cv::moveWindow("win2", 0, 0);
    cv::moveWindow("win3", USING_WIDTH / 2, 0);
    cv::moveWindow("win4", USING_WIDTH / 2, 520);
    cv::moveWindow("win5", USING_WIDTH / 2, 590);



//    storage = cvCreateMemStorage(0);



//    cv::Mat _mat = cv::imread("C:\\Users\\user\\AppData\\Local\\autoclicker\\elite\\Gabriel Enterprise.png");
//    cv::Mat _mat2 = cv::imread("C:\\Users\\user\\AppData\\Local\\autoclicker\\elite\\menu1_contact.png");
//    cv::imshow("win2tmp", _mat);
    myOCREng = new tesseract::TessBaseAPI();
    myOCRRus = new tesseract::TessBaseAPI();
//    printf("Leptonica version: %s\n", getLeptonicaVersion());


    if (myOCREng->Init("F:\\projects\\work\\elite\\tesseract-ocr\\tessdata", "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
    }
    if (myOCRRus->Init("F:\\projects\\work\\elite\\tesseract-ocr\\tessdata", "rus")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
    }


//    qDebug() << myOCREng->GetUTF8Text() << myOCRRus->GetUTF8Text();


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



//    tests();
//    panel1();


    imshow("win1", win);
    int ch = cv::waitKey(100);
    char c = static_cast<char> (ch);
    if(ch != 255) {
        if(c == 27)
            emit exitCapture();
        else if(c == 's') {                                                 // заморозить кадр
            m_flowFrame = m_flowFrame ? false : true;
            win.copyTo(m_srcWin);
        } else if(c == 'g') {
            emit openGUI();
        }
    }
}

cv::Mat *CaptureWindow::slotCheckRoiMat()
{
    cv::Mat *_mat = nullptr;
    if(state.roi)
        _mat = &matroi;
    return _mat;
}

cv::Rect *CaptureWindow::slotCheckRoiRect()
{
    cv::Rect *_rect = nullptr;
    if(state.roi)
        _rect = &m_rectMap["selectROI"];
    return _rect;
}

void CaptureWindow::slotSetDrawLine(bool aCheck, int anCount)
{
    m_nCountCell = anCount;
    m_bDrawLine = aCheck;
}

void CaptureWindow::setMinScalar(cv::Scalar acvMinScalar)
{
    qDebug() << "Min scalar" << acvMinScalar.val[0]  << acvMinScalar.val[1]  << acvMinScalar.val[2];
    minScalar = acvMinScalar;
}

void CaptureWindow::setMaxScalar(cv::Scalar acvMaxScalar)
{
    qDebug() << "Max scalar" << acvMaxScalar.val[0]  << acvMaxScalar.val[1]  << acvMaxScalar.val[2];
    maxScalar = acvMaxScalar;
}

void CaptureWindow::slotSetMaxContourForLength(int anMaxContour)
{
    maxContours = anMaxContour;
}

void CaptureWindow::slotDrawMatchRect(Qt::CheckState t_state)
{
    draw_match_rect = static_cast<bool>(t_state);
}

void CaptureWindow::slotSetLoop(Qt::CheckState t_state)
{
    loop = static_cast<bool>(t_state);
}

CaptureWindow::~CaptureWindow()
{
    win.release();
    cv::destroyAllWindows();
//    cvReleaseMemStorage(&storage);
//    myOCREng->Clear();
//    myOCREng->End();
//    myOCRRus->Clear();
//    myOCRRus->End();
    qDebug() << "exit in destructor CaptureWindow";
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

bool CaptureWindow::srchAreaOnceInMat(std::string as_imageROI, cv::Mat acvMat)
{
//    cv::Mat _matXRoi;
    cv::Point _point;
    cv::Mat _roiMat;
//    win(acvMat).copyTo(_matXRoi);
    _roiMat = mp_dataSet->at(as_imageROI).mat;
    if(findPointRoi(_roiMat, acvMat, _point, MIN_COEFF_FOR_FIND)) {
        return true;
    } else
        return false;
}

cv::Point CaptureWindow::getPointAreaInMat(std::string asImageROI, cv::Mat acvMat)
{
//    cv::Mat _matXRoi;
//    cv::Rect _rect; // = mp_dataSet->at(as_rectWhichInLook).rect;

    cv::Point _point = cv::Point(-1, -1);
    cv::Mat _roiMat;
//    win(_rect).copyTo(_matXRoi);

    _roiMat = mp_dataSet->at(asImageROI).mat;

    if(findPointRoi(_roiMat, acvMat, _point, MIN_COEFF_FOR_FIND)) {

        _point.x = _point.x + _roiMat.size().width / 2;
        _point.y = _point.y + _roiMat.size().height / 2;
    }
    return _point; // cv::Point(_point.x + m_nXOffset, _point.y + m_nYOffset);
}

bool CaptureWindow::srchAreaOnceInRect(std::string as_imageROI, cv::Rect acvRect)
{
    cv::Mat _matXRoi;
    cv::Point _point;
    cv::Mat _roiMat;
    win(acvRect).copyTo(_matXRoi);

    _roiMat = mp_dataSet->at(as_imageROI).mat;


    if(findPointRoi(_roiMat, _matXRoi, _point, MIN_COEFF_FOR_FIND)) {
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

CursorPanel *CaptureWindow::panel1()
{
    cv::Mat tmp(win.size().height, win.size().width, win.depth(), win.channels());
    cv::resize(win, tmp, tmp.size(), 0, 0, cv::INTER_LINEAR);
    transformMenu(tmp);
    cv::Mat dst;
    cv::Mat bin(dst.size(), CV_8U, 1);
    tmp(cv::Rect(170, 150, tmp.size().width - 700, tmp.size().height - 250)).copyTo(dst);
    dst(cv::Rect(3, 100, 1020, 685)).copyTo(dst);

    cv::inRange(dst, cv::Scalar(0, 93, 200), cv::Scalar(100, 255, 255), bin);

    std::vector< std::vector< cv::Point> > contoursSrc;
    std::vector< std::vector< cv::Point> > contMoreLength;
    std::vector< cv::Vec4i > hierarchy;
    cv::findContours(bin, contoursSrc, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> vecRects;
    getRectsInContour(contoursSrc, vecRects);

    for(size_t i = 0; i < vecRects.size(); i++) {
        cv::rectangle(dst, vecRects[i], cv::Scalar(0xFF, 0x0, 0x0));
    }
    determinCursorHeader(vecRects, dst);
    determinCursorBody(vecRects, dst);


        myOCREng->Clear();
    //    myOCREng->End();
        myOCRRus->Clear();
    //    myOCRRus->End();

    imshow("win2", bin);
    imshow("win3", dst);
    return &m_cursorPan;
}

void CaptureWindow::transformMenu(cv::Mat &acvMat)
{
//    int centerX = USING_WIDTH / 2;
//    int centerY = USING_HEIGHT / 2;
//    double angle = -3.47;
//    cv::Mat rot_mat = cv::getRotationMatrix2D(cv::Point(centerX, centerY), angle, 1);
//    cv::warpAffine(acvMat, acvMat, rot_mat, acvMat.size(), cv::INTER_CUBIC);

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
        int partWidth = USING_WIDTH / m_nCountCell;
        int partHeight = USING_HEIGHT / m_nCountCell;
    //    qDebug() << USING_WIDTH - partWidth * anCount << " " << USING_HEIGHT - partHeight * anCount;
        int x1 = 1, y1 = 1, x2 = 0, y2 = 0;
        while(x1 < USING_WIDTH) {
            cv::line(win, cv::Point(x1, 0), cv::Point(x1, USING_HEIGHT), cvScalar(0x0, 0xFF, 0x0));
            x1 += partWidth;
        }

        while(y1 < USING_HEIGHT) {
            cv::line(win, cv::Point(0, y1), cv::Point(USING_WIDTH, y1), cvScalar(0x0, 0xFF, 0x0));
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

void CaptureWindow::setCaptureImage(QJsonObject jobj)
{
    Q_UNUSED(jobj)
}

void CaptureWindow::findImage(cv::Mat _mat)
{
    Q_UNUSED(_mat)
//    const int count_method_ = 6;
//    if(findImageIndex == 0) {
////        std::cout << "read image" << std::endl;
////        for(int i = 0; i < count_match_methods; i++) {
////            cv::namedWindow("find" + std::to_string(i));
////            cv::moveWindow("find" + std::to_string(i), i*WIDTH_WINDOW, HEIGHT_WINDOW);
////        }
//        temp = cv::imread("template.png");
//    }
//    findImageIndex++;
//    if(findImageIndex == 50)
//        findImageIndex = 1;
//    if(findImageIndex != 1)
//        return;
//    else
//        findImageIndex++;
////    cv::Size size = win.size() - temp.size();
////    size = size + cv::Size(1,1);

////    double d_min[count_match_methods];
////    double d_max[count_match_methods];
//    cv::Mat result[count_match_methods];
////    cv::Point p_min[count_match_methods];
////    cv::Point p_max[count_match_methods];
////    printInfoMat(temp);
//    std::vector<VarForTemplateMatch> vec;
////    count_match_methods = 6;
//    std::vector<int> const_vec;
//    const_vec.push_back(CV_TM_CCORR_NORMED);
//    const_vec.push_back(CV_TM_CCOEFF_NORMED);


////    std::cout << "active=" << m_dataSet->begin()->second.active << std::endl;
//    for(size_t i = 0; i != const_vec.size(); ++i) {
////        d_min[i] = 0.0;
////        d_max[i] = 0.0;
//        VarForTemplateMatch _f;
//                // CV_TM_CCOEFF_NORMED CV_TM_CCORR_NORMED
//        cv::matchTemplate(win, temp, result[i], const_vec[i]);
////        printInfoMat(result[i]);

//        _f.method = const_vec[i];
//        cv::minMaxLoc(result[i], &_f.m1, &_f.m2, &_f.p1, &_f.p2);

////        cv::minMaxLoc(result[i], &d_min[i], &d_max[i], &p_min[i], &p_max[i]);
////        cv::normalize(result[i],result[i],1,0,CV_MINMAX);
////        imshow("find" + std::to_string(i), result[i]);


////        std::cout << "i=" << i
////                  << " min=" << std::setw(11) << std::setprecision(3 )<< d_min[i]
////                     << " max=" << std::setw(11) << std::setprecision(3) << d_max[i]
////                        << std::setw(17) << " p_min=" << p_min[i].x<< ":" << p_min[i].y
////                        << std::setw(17) << " p_max=" << p_max[i].x << ":" << p_max[i].y << std::endl;
////        std::cout << "i=" << i
////                        << " min=" << std::setw(11) << std::setprecision(3 )<< _f.m1
////                        << " max=" << std::setw(11) << std::setprecision(3) << _f.m2
////                        << std::setw(17) << " p_min=" << _f.p1.x<< ":" << _f.p1.y
////                        << std::setw(17) << " p_max=" << _f.p2.x << ":" << _f.p2.y << std::endl;

//        vec.push_back(_f);

//    }
//    cv::Point match = findMatchPoint(vec);
//    if(match != cv::Point(0,0)) {
//        cv::Rect rectMatch(match.x , match.y, temp.size().width, temp.size().height);
//        if(m_rectMap.find("rectMatch") == m_rectMap.end())
//            m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//        else
//            m_rectMap["rectMatch"] = rectMatch;
//    } else {
//        m_rectMap.erase("rectMatch");
//    }




}

bool CaptureWindow::findPointRoi(cv::Mat &t_mat, cv::Mat &t_whereFind, cv::Point &t_point, double t_factor)
{
    cv::Mat result[count_match_methods];
    std::vector<VarForTemplateMatch> vec;
    std::vector<int> const_vec;
    const_vec.push_back(CV_TM_CCORR_NORMED);
    const_vec.push_back(CV_TM_CCOEFF_NORMED);
    for(size_t i = 0; i != const_vec.size(); ++i) {
        VarForTemplateMatch _f;
        cv::matchTemplate(t_whereFind, t_mat, result[i], const_vec[i]);
        _f.method = const_vec[i];
        cv::minMaxLoc(result[i], &_f.m1, &_f.m2, &_f.p1, &_f.p2);
        vec.push_back(_f);
    }
    cv::Point match = findMatchPoint(vec, t_factor);
    t_point = match;
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
    _vec.erase(std::remove_if(_vec.begin(), _vec.end(), [_vec] (VarForTemplateMatch aItem) -> bool {
                   return aItem.m2 < MIN_COEFF_FOR_FIND_POINT;
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

bool CaptureWindow::checkContour(std::vector<cv::Point> &acvVecPointCont, cv::Mat &cvMatOut)
{
    cv::Point min_x;
    min_x.x = USING_WIDTH;
    cv::Point max_x;
    max_x.x = 0;
    cv::Point min_y;
    min_y.y = USING_HEIGHT;
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
    for(size_t j = 0; j < acvVecPointCont.size(); j++) {
        std::vector<cv::Point> vectmp = acvVecPointCont[j];
        if(vectmp.size() < 3)
            continue;
        cv::Point min_x;
        min_x.x = USING_WIDTH;
        cv::Point max_x;
        max_x.x = 0;
        cv::Point min_y;
        min_y.y = USING_HEIGHT;
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

cv::Rect CaptureWindow::getRectInContour(std::vector<cv::Point> &acvVecPoint)
{
    if(acvVecPoint.size() < 3)
        return cv::Rect(0, 0, 0, 0);
    cv::Point min_x;
    min_x.x = USING_WIDTH;
    cv::Point max_x;
    max_x.x = 0;
    cv::Point min_y;
    min_y.y = USING_HEIGHT;
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

    cv::inRange(matCursor, minScalar, maxScalar, bin);

//    std::vector< std::vector< cv::Point> > contoursSrc;
//    std::vector< std::vector< cv::Point> > contMoreLength;
//    std::vector< cv::Vec4i > hierarchy;
//    cv::findContours(bin, contoursSrc, cv::noArray(), cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
//    cv::Rect bigRect = cv::Rect(USING_WIDTH, USING_HEIGHT, 0, 0);
//    cv::Point min_x;
//    min_x.x = USING_WIDTH;
//    cv::Point max_x;
//    max_x.x = 0;
//    cv::Point min_y;
//    min_y.y = USING_HEIGHT;
//    cv::Point max_y;
//    max_y.y = 0;
//    for( size_t i = 0; i < contoursSrc.size(); i++ ) {
//        cv::Rect contRect = getRectInContour(contoursSrc[i]);
//        if(checkApproximRect(cv::Rect(0, 0, aRectCursor.width, aRectCursor.height), contRect, 2)) {
//            continue;
//        } else {
////            if(contRect.x < bigRect.x)
////                bigRect.x = contRect.x;
////            if(contRect.width > bigRect.width)
////                bigRect.width = contRect.width;
////            if(contRect.y < bigRect.y)
////                bigRect.y = contRect.y;
////            if(contRect.height > bigRect.height)
////                bigRect.height = contRect.height;

//                if(contRect.x < min_x.x)
//                    min_x.x = contRect.x;
//                if(contRect.x + contRect.width > max_x.x)
//                    max_x.x = contRect.x + contRect.width;
//                if(contRect.y < min_y.y)
//                    min_y.y = contRect.y;
//                if(contRect.y + contRect.height > max_y.y)
//                    max_y.y = contRect.y + contRect.height;
//        }
//    }

//    cv::Rect tmpRect = cv::Rect(min_x.x -1 , min_y.y - 1, max_x.x - min_x.x + 2, max_y.y - min_y.y + 2);
//    tmpRect.x - 1 < 0 ? tmpRect.x = 0: tmpRect.x - 1;
//    tmpRect.y - 1 < 0 ? tmpRect.y = 0: tmpRect.y - 1;
//    cv::rectangle(matCursor, tmpRect, cv::Scalar(0, 255, 0), 2, 8);
//    cv::drawContours(matCursor, contoursSrc, -1, cv::Scalar(0, 255, 0));

//    cv::Mat cur;
//    matCursor(tmpRect).copyTo(cur);
//    cv::Mat bin2(matCursor.size(), CV_8U, 1);
//    cv::inRange(cur, minScalar, maxScalar, bin2);
//    cv::Mat res(bin2.size(), CV_8U, 3);
//    bin2(cv::Rect(0, 0, bin2.size().width, bin2.size().height)).copyTo(res);
//    cv::imshow("win4", bin);
//    cv::imshow("win5", matCursor);
    return bin;
}

cv::Mat CaptureWindow::parsBinLineNavList(cv::Mat &aMatList, cv::Rect aRectCursor)
{
    cv::Mat matCursor;
    aMatList(aRectCursor).copyTo(matCursor);

    cv::Mat bin(matCursor.size(), CV_8U, 1);

    cv::inRange(matCursor, minScalar, maxScalar, bin);
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

    cv::inRange(cutMat, minScalar, maxScalar, binCut);
    cv::imshow("win5", binCut);
    return binCut;
//    return cv::Mat();
}

void CaptureWindow::determinCursorHeader(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst)
{
    m_cursorPan.rectHeader.y = USING_HEIGHT;
    m_cursorPan.headerActive = false;

#define CURSOR_DELTA_HEADER     5
#define CURSOR_Y_HEADER         5
#define CURSOR_WIDTH_HEADER     38

    for(std::vector<cv::Rect>::iterator it = vecRects.begin(); it != vecRects.end(); it++) {
        if(it->y >              CURSOR_Y_HEADER - CURSOR_DELTA_HEADER
                && it->y <      CURSOR_Y_HEADER + CURSOR_DELTA_HEADER
                && it->height > CURSOR_WIDTH_HEADER - CURSOR_DELTA_HEADER
                && it->height < CURSOR_WIDTH_HEADER + CURSOR_DELTA_HEADER)
        {
                m_cursorPan.rectHeader = cv::Rect(it->x + 1, it->y + 1, it->width - 2, it->height - 2);
                m_cursorPan.headerActive = true;
                vecRects.erase(it);
                cv::Mat binColor = makeBinHeadMenu(aMatDst, m_cursorPan.rectHeader);
                myOCRRus->SetImage( (uchar*)binColor.data, binColor.size().width, binColor.size().height, binColor.channels(), binColor.step1());
                myOCRRus->Recognize(nullptr);
                m_cursorPan.sHeaderName = myOCRRus->GetUTF8Text();
                if(m_cursorPan.sHeaderName.isEmpty())
                    m_cursorPan.sHeaderName = "not recognized";
                m_cursorPan.sHeaderName = m_cursorPan.sHeaderName.simplified();
                m_cursorPan.sHeaderName = m_cursorPan.sHeaderName.toLower();
//                qDebug() << "RUS:" << m_cursorPan.sHeaderName;
                break;
        }
    }
}

void CaptureWindow::determinCursorBody(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst)
{
    m_cursorPan.bodyActive = false;


#define CURSOR_X_LEFT_NAV       17
#define CURSOR_WIDTH_LEFT_NAV   89


#define CURSOR_X_MIDDLE_NAV     128
#define CURSOR_WIDTH_MIDDLE_NAV 98
#define CURSOR_X_RIGHT_NAV      231
#define CURSOR_WIDTH_RIGHT_NAV  764

#define CURSOR_Y_LEFT_NAV_FILT  210
#define CURSOR_Y_LEFT_NAV_XFILT 277
#define CURSOR_Y_LEFT_NAV_G_MAP 490
#define CURSOR_Y_LEFT_NAV_S_MAP 610

    for(std::vector<cv::Rect>::iterator it = vecRects.begin(); it != vecRects.end(); it++) {
        if(m_cursorPan.headerActive) {
//            qDebug() << "Body:" << it->x << it->y << it->width << it->height;
//            qDebug() << sNameMenu1Head[0] << sNameMenu1Head[1] << sNameMenu1Head[2];
            if(comparisonStr(m_cursorPan.sHeaderName, sNameMenu1Head[0]) <= 1) {          // Меню навигация
                if(         it->x > CURSOR_X_LEFT_NAV - CURSOR_DELTA
                        &&  it->x < CURSOR_X_LEFT_NAV + CURSOR_DELTA
                        &&  it->width > CURSOR_WIDTH_LEFT_NAV - CURSOR_DELTA
                        &&  it->width < CURSOR_WIDTH_LEFT_NAV + CURSOR_DELTA) {
                    // Курсор в панели навигации слева
                    if(it->y > CURSOR_Y_LEFT_NAV_FILT - CURSOR_DELTA
                            &&  it->y < CURSOR_Y_LEFT_NAV_FILT + CURSOR_DELTA) {
                        m_cursorPan.sBodyName = "push_filter";
                    } else if(it->y > CURSOR_Y_LEFT_NAV_XFILT - CURSOR_DELTA
                              &&  it->y < CURSOR_Y_LEFT_NAV_XFILT + CURSOR_DELTA) {
                        m_cursorPan.sBodyName = "close_push_filter";
                    } else if(it->y > CURSOR_Y_LEFT_NAV_G_MAP - CURSOR_DELTA
                              &&  it->y < CURSOR_Y_LEFT_NAV_G_MAP + CURSOR_DELTA) {
                        m_cursorPan.sBodyName = "push_galaxy_map";
                    } else if(it->y > CURSOR_Y_LEFT_NAV_S_MAP - CURSOR_DELTA
                              &&  it->y < CURSOR_Y_LEFT_NAV_S_MAP + CURSOR_DELTA) {
                        m_cursorPan.sBodyName = "push_system_map";
                    }
                    m_cursorPan.rectBody = *it;
                    m_cursorPan.bodyActive = true;
                } else if(it->x > CURSOR_X_MIDDLE_NAV - CURSOR_DELTA
                          &&  it->x < CURSOR_X_MIDDLE_NAV + CURSOR_DELTA
                          &&  it->width > CURSOR_WIDTH_MIDDLE_NAV - CURSOR_DELTA
                          &&  it->width < CURSOR_WIDTH_MIDDLE_NAV + CURSOR_DELTA) {
                    // Курсор в панели навигации посредине
//                    qDebug() << "middle cursor";
                } else if(it->x > CURSOR_X_RIGHT_NAV - CURSOR_DELTA
                          &&  it->x < CURSOR_X_RIGHT_NAV + CURSOR_DELTA
                          &&  it->width > CURSOR_WIDTH_RIGHT_NAV - CURSOR_DELTA
                          &&  it->width < CURSOR_WIDTH_RIGHT_NAV + CURSOR_DELTA) {
                    // Курсор в панели навигации справа
                    m_cursorPan.rectBody = *it;
                    m_cursorPan.bodyActive = true;
                    cv::Mat cutMat = parsBinLineNavList(aMatDst, m_cursorPan.rectBody);
//                    cv::Mat binColor = makeBinHeadMenu(cutMat, m_cursorPan.rectBody);
//                    emit signalSaveImageForDebug(cutMat);
                    myOCREng->SetImage( (uchar*)cutMat.data, cutMat.size().width, cutMat.size().height, cutMat.channels(), cutMat.step1());
                    myOCREng->Recognize(nullptr);
                    m_cursorPan.sBodyName = myOCREng->GetUTF8Text();
                    if(m_cursorPan.sBodyName.isEmpty())
                        m_cursorPan.sBodyName = "not recognized";
                    m_cursorPan.sBodyName = m_cursorPan.sBodyName.simplified();
                    m_cursorPan.sBodyName = m_cursorPan.sBodyName.toLower();

                }
//                qDebug() << "ENG:" << m_cursorPan.sBodyName;

            } else if(comparisonStr(m_cursorPan.sHeaderName, sNameMenu1Head[1]) <= 1) {        // меню транзакции

            } else if(comparisonStr(m_cursorPan.sHeaderName, sNameMenu1Head[2]) <= 1) {        // меню контакты
#define CURSOR_X_LEFT_CONT  6
#define CURSOR_X_RIGHT_CONT 573
//                qDebug() << "Body:" << it->x << it->y << it->width << it->height;
                if(it->x > CURSOR_X_LEFT_CONT - CURSOR_DELTA
                        &&  it->x < CURSOR_X_LEFT_CONT + CURSOR_DELTA){
                    // Курсор слева в панели Контакты
                    qDebug() << "left";
                    m_cursorPan.rectBody = *it;
                    m_cursorPan.bodyActive = true;
                    cv::Mat cutMat = parsBinLineNavList(aMatDst, m_cursorPan.rectBody);
                    myOCREng->SetImage( (uchar*)cutMat.data, cutMat.size().width, cutMat.size().height, cutMat.channels(), cutMat.step1());
                    myOCREng->Recognize(nullptr);
                    m_cursorPan.sBodyName = myOCREng->GetUTF8Text();
                    if(m_cursorPan.sBodyName.isEmpty())
                        m_cursorPan.sBodyName = "not recognized";
                    m_cursorPan.sBodyName = m_cursorPan.sBodyName.simplified();
                    m_cursorPan.sBodyName = m_cursorPan.sBodyName.toLower();

                }else if(it->x > CURSOR_X_RIGHT_CONT - CURSOR_DELTA
                            &&  it->x < CURSOR_X_RIGHT_CONT + CURSOR_DELTA){
                    // Курсор справа в панели Контакты
                    m_cursorPan.rectBody = *it;
                    m_cursorPan.bodyActive = true;
                    m_cursorPan.sBodyName = "req_docking";

                    qDebug() << "right";
                }
            }
        }

    }
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
    cv::Mat tmp;
    win.copyTo(tmp);
    transformMenu(tmp);
    cv::Mat dst;
    tmp(cv::Rect(345, 300, tmp.size().width - 1170, tmp.size().height - 465)).copyTo(dst);

    cv::Mat bin(dst.size(), CV_8U, 1);
    cv::inRange(dst, cv::Scalar(0, 93, 200), cv::Scalar(100, 255, 255), bin);

    std::vector< std::vector< cv::Point> > contoursSrc;
    std::vector< std::vector< cv::Point> > contMoreLength;
    std::vector< cv::Vec4i > hierarchy;
    cv::findContours(bin, contoursSrc, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> vecRects;
    getRectsInContour(contoursSrc, vecRects);

    for(size_t i = 0; i < vecRects.size(); i++) {
        cv::rectangle(dst, vecRects[i], cv::Scalar(0xFF, 0x0, 0x0));
    }
    determinCursorSubNavList(vecRects, bin);


    imshow("win2", bin);
    imshow("win3", dst);
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

void CaptureWindow::determinCursorSubNavList(std::vector<cv::Rect> &vecRects, cv::Mat &aMatDst)
{
    m_cursorPan.rectSubNavList.y = -1;
    m_cursorPan.subNavList = false;

#define CURSOR_Y_NAV_SUB_LIST           498
#define CURSOR_Y_NAV_SUB_LIST_BOTTOM    567

    for(std::vector<cv::Rect>::iterator it = vecRects.begin(); it != vecRects.end(); it++) {
//        qDebug() << "Body:" << it->x << it->y << it->width << it->height;
        if(     it->y > CURSOR_Y_NAV_SUB_LIST - CURSOR_DELTA     &&
                it->y < CURSOR_Y_NAV_SUB_LIST + CURSOR_DELTA)
        {
            // Значит курсор в положении кнопок управления, верхнее положение
                m_cursorPan.subNavList = true;
                m_cursorPan.rectSubNavList = cv::Rect(it->x + 1, it->y + 1, it->width - 2, it->height - 2);
                cv::Mat curMat;
//                cv::Rect cutRect = cv::Rect(30, 10, m_cursorPan.rectSubNavList.width - 60, m_cursorPan.rectSubNavList.height - 20);
                aMatDst(m_cursorPan.rectSubNavList).copyTo(curMat);
                cv::cvtColor(curMat, curMat, cv::COLOR_GRAY2BGR);
                if(srchAreaOnceInMat(subNamePicMenu1NavList[0].toStdString(), curMat)) {
                    m_cursorPan.sSubNavName = "fix_target";
                    qDebug() << "Menu fix";
                } else if(srchAreaOnceInMat(subNamePicMenu1NavList[1].toStdString(), curMat)) {
                    m_cursorPan.sSubNavName = "unfix_target";
                    qDebug() << "Menu unfix";
                } else if(srchAreaOnceInMat(subNamePicMenu1NavList[2].toStdString(), curMat)) {
                    m_cursorPan.sSubNavName = "enable_hypermode";
                    qDebug() << "Menu enabled giper mode";
                } else
                    qDebug() << "missed the mark";
                cv::imshow("win4", curMat);
                break;
        } else if(it->y > CURSOR_Y_NAV_SUB_LIST_BOTTOM - CURSOR_DELTA     &&
                  it->y < CURSOR_Y_NAV_SUB_LIST_BOTTOM + CURSOR_DELTA) {
            qDebug() << "Menu previous";
        }
    }
}

CursorPanel *CaptureWindow::menuDocking()
{
    cv::Mat tmp;
    cv::Mat dst;
    win(cv::Rect(820, 775, 300, 160)).copyTo(dst);
    cv::Mat bin(dst.size(), CV_8U, 1);
    cv::inRange(dst, cv::Scalar(0, 100, 100), cv::Scalar(255, 255, 255), bin);
    std::vector<cv::Rect> vecRects = getRectsBigInContours(bin);
    for(size_t i = 0; i < vecRects.size(); i++) {
        cv::rectangle(dst, vecRects[i], cv::Scalar(0xFF, 0x0, 0x0));
    }


    m_cursorPan.rectMenuDocking.y = -1;
    m_cursorPan.activeMenuDocking = false;

#define CURSOR_Y_MENU_DOCKING_AUTOSTART     124
#define CURSOR_Y_MENU_DOCKING_ANGAR         77
#define CURSOR_Y_MENU_DOCKING_SERVICE       34
#define CURSOR_DOCKING_WIDTH                299
#define CURSOR_DOCKING_HEIGHT               34


    for(std::vector<cv::Rect>::iterator it = vecRects.begin(); it != vecRects.end(); it++) {
//        qDebug() << "Dock:" << it->x << it->y << it->width << it->height;
        if(     it->width > CURSOR_DOCKING_WIDTH - CURSOR_DELTA     &&
                it->width < CURSOR_DOCKING_WIDTH + CURSOR_DELTA     &&
                it->height > CURSOR_DOCKING_HEIGHT - CURSOR_DELTA   &&
                it->height < CURSOR_DOCKING_HEIGHT + CURSOR_DELTA)
        {
            // Значит курсор в положении автозапуск
            if(     it->y > CURSOR_Y_MENU_DOCKING_AUTOSTART - CURSOR_DELTA &&
                    it->y < CURSOR_Y_MENU_DOCKING_AUTOSTART + CURSOR_DELTA  ){
                m_cursorPan.activeMenuDocking = true;
                m_cursorPan.sNameMenuDocking = "menu_docking_autostart";

            } else if(     it->y > CURSOR_Y_MENU_DOCKING_ANGAR - CURSOR_DELTA &&
                           it->y < CURSOR_Y_MENU_DOCKING_ANGAR + CURSOR_DELTA  ) {
                m_cursorPan.activeMenuDocking = true;
                m_cursorPan.sNameMenuDocking = "menu_docking_angar";

            } else if(     it->y > CURSOR_Y_MENU_DOCKING_SERVICE - CURSOR_DELTA &&
                           it->y < CURSOR_Y_MENU_DOCKING_SERVICE + CURSOR_DELTA  ) {
                m_cursorPan.activeMenuDocking = true;
                m_cursorPan.sNameMenuDocking = "menu_docking_service";
            }
            m_cursorPan.rectSubNavList = cv::Rect(it->x + 1, it->y + 1, it->width - 2, it->height - 2);
            qDebug() << m_cursorPan.sNameMenuDocking;
            break;
        }
    }


    imshow("win2", bin);
    imshow("win3", dst);
    return &m_cursorPan;
}
