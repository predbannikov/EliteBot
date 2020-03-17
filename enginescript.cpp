#include "enginescript.h"


EngineScript::EngineScript(QObject *parent) : QObject(parent)
{

    mp_ioData = new IOData;
    mp_dataSet = mp_ioData->assignpDataSet();

    m_pointOffsetScreen = QPoint(STANDART_FULLHD_WIDTH, -STANDART_FULLHD_HEIGHT);
    capture = new CaptureWindow(mp_dataSet, STANDART_FULLHD_WIDTH + 1, -STANDART_FULLHD_HEIGHT, USING_WIDTH, USING_HEIGHT, this);
    m_pControl = new AIControl;

    if(mp_ioData->prepWorkPath()) {
        std::cout << "load success" << std::endl;
        ginfo = new GuiInfo(mp_ioData);
//        connect(ginfo, &GuiInfo::sendScriptPerform, this, &EngineScript::performScript);

        connect(ginfo, &GuiInfo::signalGetMatRoi, capture, &CaptureWindow::slotCheckRoiMat);
        connect(ginfo, &GuiInfo::signalGetRectRoi, capture, &CaptureWindow::slotCheckRoiRect);
        connect(ginfo, &GuiInfo::signalDrawMatchRect, capture, &CaptureWindow::slotDrawMatchRect);
        connect(ginfo, &GuiInfo::signalDrawMesh, capture, &CaptureWindow::slotSetDrawLine);
        connect(ginfo, &GuiInfo::signalSetLoop, capture, &CaptureWindow::slotSetLoop);
        connect(ginfo, &GuiInfo::signalSendMinScalar, capture, &CaptureWindow::setMinScalar);
        connect(ginfo, &GuiInfo::signalSendMaxScalar, capture, &CaptureWindow::setMaxScalar);
        connect(ginfo, &GuiInfo::signalSendMinNumber, capture, &CaptureWindow::setMinNumber);
        connect(ginfo, &GuiInfo::signalSendMidNumber, capture, &CaptureWindow::setMidNumber);
        connect(ginfo, &GuiInfo::signalSendMaxNumber, capture, &CaptureWindow::setMaxNumber);
//        connect(capture, &CaptureWindow::sendKey, ginfo, &GuiInfo::slotReadKey);
//        connect(ginfo, &GuiInfo::signalSendMaxContourForLength, capture, &CaptureWindow::slotSetMaxContourForLength);



        connect(capture, &CaptureWindow::openGUI, this, &EngineScript::openGUI);
        connect(capture, &CaptureWindow::exitCapture, this, &EngineScript::exitEngine);


        // for debug
        connect(capture, &CaptureWindow::signalSaveImageForDebug, this, &EngineScript::slotSaveImage);
    } else  {
        std::cout << "load not seccess" << std::endl;
        cycle = false;
    }


    sock = new QTcpSocket;
//    sock->connectToHost("192.168.16.128", 10101);
    sock->connectToHost("127.0.0.1", 10101);


    timeElapsedForFunc.start();

    ginfo->openDialog();
    ginfo->move(1280, 550);
    ginfo->sendAllNumbData();
}

void EngineScript::engine()
{
    m_pControl->state = RESTOR_GAME;
//    m_pControl->state = DEBUG_STATE;
    while(cycle) {
        switch (m_pControl->state) {
        case DEBUG_STATE:
            break;
        case RESTOR_GAME: {
            QJsonObject _jMsgToSend;
            _jMsgToSend["target"] = "mouse";
            _jMsgToSend["method"] = "move_click";
            _jMsgToSend["code"] = "BTN_LEFT";
            _jMsgToSend["x"] = 2100;
            _jMsgToSend["y"] = -USING_HEIGHT + 150;
            sendDataToSlave(QJsonDocument(_jMsgToSend).toJson());           // Активировать окно справа сверху
            _jMsgToSend = QJsonObject();
            QThread::msleep(50);
            m_pControl->state = AICONTROL;
            break;
        }
        case AICONTROL:
            m_pControl->smallRing();
            break;
        case PUSH_KEY:
            push_key();
            m_pControl->state = AICONTROL;
            break;
        case PRESS_KEY:
            press_key();
            m_pControl->state = AICONTROL;
            break;
        case RELEASE_KEY:
            press_key();
            m_pControl->state = AICONTROL;
            break;
        case SEARCH_IMAGE_CONTINUOUS:
        {
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {
                if(srchAreaOnceInPart(m_pControl->searchImage, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd, m_pControl->coeff)) {
                    m_pControl->check = true;
                    m_pControl->state = AICONTROL;
                }
            } else {
                m_pControl->state = AICONTROL;
            }
            break;
        }
        case WHILE_IMAGE_CONTINUOUS:
        {
            static int accum = 0;
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {
                if(!srchAreaOnceInPart(m_pControl->searchImage, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd, m_pControl->coeff)) {
                    if(accum == 10) {
                        accum = 0;
                        m_pControl->check = true;
                        m_pControl->state = AICONTROL;
                    }
                    qDebug() << "image hide" << accum;
                    accum++;
                } else {
                    accum = 0;
                }
            } else {
                accum = 0;
                m_pControl->check = false;
                m_pControl->state = AICONTROL;
            }
            break;
        }
        case WHICH_IMAGE_MORE_SIMILAR:
            m_pControl->searchImage = getImageMoreSimilarInRect(m_pControl->listForSearch, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd);
            m_pControl->state = AICONTROL;
            break;
        case CLICK_POINT_IMAGE_AFTER_LOOK:
        {
            cv::Point cvPoint = getPointAfterLookAreaInRect(m_pControl->searchImage, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd);
            mouse_move_click(cvPoint);
            m_pControl->state = AICONTROL;
            break;
        }
        case CLICK_TO_POINT:
        {
            cv::Rect rectTmp = mp_dataSet->at(m_pControl->searchImage.toStdString()).rect;
            cv::Point cvPoint(rectTmp.x + (rectTmp.width / 2), rectTmp.y + (rectTmp.height / 2));
            mouse_move_click(cvPoint);
            m_pControl->state = AICONTROL;
            break;
        }
        case TYPING_TEXT:
            typingText();
            m_pControl->state = AICONTROL;
            break;
        case TRANS_PANEL1:
        {
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {
                CursorPanel *pan = capture->panel1Header();
                if(pan->activeHeader) {
                    QThread::msleep(500);
                    m_pControl->cursorPanel = pan;
                    m_pControl->check = true;
                    m_pControl->state = AICONTROL;
                }
            } else {
                m_pControl->check = false;
                m_pControl->state = AICONTROL;
            }
            break;
        }
        case TRANS_BODY_CURSOR:
        {
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {
                CursorPanel *pan = capture->panel1Body();
                if(pan->activeBody) {
                    m_pControl->cursorPanel = pan;
                    m_pControl->check = true;
                    m_pControl->state = AICONTROL;
                }
            } else {
                m_pControl->check = false;
                m_pControl->state = AICONTROL;
            }
        }
            break;
        case TRANS_SUB_CURSOR:
        {
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {
                CursorPanel *pan = capture->subPanel1Nav();
                if(pan->activeSubNav) {
                    m_pControl->cursorPanel = pan;
                    m_pControl->check = true;
                    m_pControl->state = AICONTROL;
                }
            } else {
                m_pControl->check = false;
                m_pControl->state = AICONTROL;
            }
            break;
        }
        case TRANS_MENU_DOCKING:
        {
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {
                CursorPanel *pan = capture->menuDocking();
                if(pan->activeMenuDocking) {
                    m_pControl->cursorPanel = pan;
                    m_pControl->check = true;
                    m_pControl->state = AICONTROL;
                }
            } else {
                m_pControl->check = false;
                m_pControl->state = AICONTROL;
            }
            break;
        }
        case WAIT_MSEC:                                                 // Функция ожидания
        {
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {

            } else {
                m_pControl->check = true;
                m_pControl->state = AICONTROL;
            }
            break;
        }
        case APPROACH:                                                 // Функция ожидания
        {
            static bool resetDistance = false;
            static bool approach = false;
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {
                if(!approach) {
                    press_key("w");
                    approach = true;
                }
                if(elaps > 6000) {
                    release_key("w");
                    approach = true;
                    for(int i = 0; i < 5; i++) {
                        push_key("x");
                        QThread::msleep(1300);
                        approach = false;
                        resetDistance = false;
                        m_pControl->check = true;
                        m_pControl->state = AICONTROL;
                    }
                    qDebug() << "Выход из enginescripte APPROACH";
                }
//                Distance *distance = capture->recognizDistance();
//                if(!resetDistance) {
//                    distance->distanceList.clear();
//                    distance->distance = 0;
//                    resetDistance = true;
//                } else {
//                    if(!approach) {
//                        press_key("w");
//                        approach = true;
//                    }
//                    if(distance->distance < std::numeric_limits<double>::epsilon () &&
//                       distance->distance > -std::numeric_limits<double>::epsilon () ) {
//                        qDebug() << "distance zero" << distance->distanceList;
//                        break;
//                    }

//                    if(distance->distance <= m_pControl->distance) {
//                        release_key("w");
//                        for(int i = 0; i < 5; i++) {
//                            push_key("x");
//                            QThread::msleep(1300);
//                        }
//                        approach = false;
//                        resetDistance = false;
//                        m_pControl->check = true;
//                        m_pControl->state = AICONTROL;
//                    }
//                }

            } else {
                for(int i = 0; i < 5; i++) {
                    push_key("x");
                    QThread::msleep(1300);
                }
                approach = false;
                resetDistance = false;
                m_pControl->check = false;
                m_pControl->state = AICONTROL;
            }
            break;
        }
        case SET_FRAME_FREQ:
            capture->nWaitKey = m_pControl->frameFreq;
            m_pControl->state = AICONTROL;
            break;
        case COMPASS: {
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {
                Compass *compass = capture->compass();
                if(!compass->active)
                    break;
                int dx = compass->pCenter.x - compass->pDot.x;
                int dy = compass->pCenter.y - compass->pDot.y;
                static int powerX;
                static int seekY;
                static bool pickup = false;
                static bool pickdownX = false;
                static bool pickdownY = false ;
                static bool rotate = false ;
                const int approxim = 8;            // Расстояние до центра до которого компас реагирует
                // Вычисление угла между двумя векторами
                cv::Point a(compass->pCenter.x - compass->pDot.x, compass->pCenter.y - compass->pDot.y);
                cv::Point b(0, 1);
#define SPEED_COMPAS_Y      250
#define SPEED_COMPAS_ROT    250
                double radius = sqrt(pow(dx,2) + pow(dy, 2));

                double atan_a = atan2(a.x * b.y - b.x * a.y, a.x * b.x + a.y * b.y);
                double grad = atan_a * 180. / M_PI;
                qDebug() << "grad=" << grad << " radius=" << radius;
                //  Выравнивание по ротации
                if(!rotate) {
                    qDebug() << "radius =" << radius;
                        if(approxim <= radius) {
                            if((grad > 10 || grad < -10)) {         // выровнять по градусам
                                if(dx == 0)
                                    dx = 1;
                                powerX = SPEED_COMPAS_ROT * (dx / abs(dx));        // знак полярности
                                powerX *= -1;
                                qDebug() << "rotate dx =" << dx << powerX;
                                move_mouse_rel(powerX, 0);
                            }
                        } else {
                            powerX = 0;
                            pickdownX = true;
                            qDebug() << "x rotate complate";
                        }
                    rotate = true;
                }
                if(grad < 20 && grad > -20)
                    if(!pickdownX) {
                        powerX *= -1;
                        move_mouse_rel(powerX, 0);
                        pickdownX = true;
                        qDebug() << "x rotate complate" << dx << powerX;
                    }
                //  Выравнивание по оси Y
                if(!pickup && pickdownX) {
                    if(approxim <= abs(dy) || !compass->face) {
                        seekY = -SPEED_COMPAS_Y;
                    } else {
                        if(dy < -3) {
                            seekY = SPEED_COMPAS_Y;
                        } else {
                            seekY = 0;
                            pickdownY = true;
                            qDebug() << "y complate" ;
                        }
                    }
                    qDebug() << "move Y" << dy << seekY << " face=" << compass->face;
                    move_mouse_rel(0, seekY);
                    pickup = true;
                }  
                if(compass->face && pickdownX) {
                    if(dy > -3 && dy < approxim  ) {
                        if(!pickdownY) {
                            qDebug() << "Делаем обратную силу";
                            seekY *= -1;
                            move_mouse_rel(0, seekY);
                            pickdownY = true;
                            qDebug() << "y complate" << dy << seekY << " face=" << compass->face;
                        }
                    }
                }
                if(pickdownX && pickdownY)
                {
                    qDebug() << "success compass" ;
                    pickdownX = false;
                    pickdownY = false;
                    pickup = false;
                    rotate = false;
//                    m_pControl->check = true;
                    m_pControl->state = AICONTROL;
                }
            } else {
                m_pControl->check = false;
                m_pControl->state = AICONTROL;
            }
            break;
        }
        case AIMING:
        {
            qint64 elaps = m_pControl->timeElapsed.elapsed();
            if(elaps < m_pControl->timeWaitMsec) {
                CursorTarget *tar = capture->takeAimp();

                int dx = tar->pointCenter.x - tar->pointTarget.x;
                int dy = tar->pointCenter.y - tar->pointTarget.y;
                static int powerX;
                static int seekY;
                static int seekX;

                static bool pickup = false;
                static bool pickdownX = false;
                static bool pickdownY = false;
                static bool speed0 = false;
                static bool speed1 = false;
                static bool rotate = false;
                static int count_error = 0;
//                qDebug() << dx;
//                double radius = sqrt(pow(dx,2) + pow(dy, 2));
                if(!tar->active) {
                    qDebug() << "target not active err=" << count_error;
                    if(count_error == 3) {
                        qDebug() << "target RESET err=" << count_error;
                        count_error = 0;
                        push_key(" ");
                        QThread::msleep(30);
                        pickup = false;
                        speed1 = false;
                        speed0 = false;
                        pickdownX = false;
                        rotate = false;
                        pickdownY = false;
                        m_pControl->check = false;
                        m_pControl->target = true;
                        m_pControl->state = AICONTROL;
                    }
                    if(timeElapsedForFunc.elapsed() > 500 ) {
                        count_error++;
                        timeElapsedForFunc.restart();
                    }
                    break;


                }
                count_error = 0;
                double radius = sqrt(pow(dx,2) + pow(dy, 2));
/*                // Вычисление угла между двумя векторами
                cv::Point a(tar->pointCenter.x - tar->pointTarget.x, tar->pointCenter.y - tar->pointTarget.y);
                cv::Point b(0, 1);
                double atan_a = atan2(a.x * b.y - b.x * a.y, a.x * b.x + a.y * b.y);
                double grad = atan_a * 180. / M_PI;
                qDebug() << "grad=" << grad << " radius=" << radius;


#define ROTATE_SPEED_0  180
#define ROTATE_SPEED_2  100
#define ROTATE_SPEED_3  100
#define ROTATE_SPEED_2_SEEK  45
#define ROTATE_SPEED_3_SEEK  90
#define ROTATE_SPEDD_DELTA  7
                //  Выравнивание по ротации
                if(!rotate) {
                    qDebug() << "radius =" << radius;
                        if(abs(dx) > 5) {
                            if((grad > 3 || grad < -3)) {         // выровнять по градусам
                                if(dx == 0)
                                    dx = 1;
                                powerX = ROTATE_SPEED_0 * (dx / abs(dx));        // знак полярности
                                powerX *= -1;
                                qDebug() << "rotate dx =" << dx << powerX;
//                                if(abs(grad) > 30) {
//                                    powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_2);
//                                    speed0 = true;
//                                }
//                                if(abs(grad) > 90) {
//                                    powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_3);
//                                    speed1 = true;
//                                }
                                move_mouse_rel(powerX , 0);
                            }
                        } else {
                            powerX = 0;
                            pickdownX = true;
                            qDebug() << "x rotate complate" << dx;
                        }
                        rotate = true;
                } else {
                    if(powerX != 0 && !pickdownX) {
                        if(abs(grad) > ROTATE_SPEED_2_SEEK + ROTATE_SPEDD_DELTA && !speed0 ) {
                            powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_2);
                            speed0 = true;
                            move_mouse_rel((powerX / abs(powerX)) * ROTATE_SPEED_2, 0);
                            qDebug() << "UP rotate speed" << powerX;
                        }
                        if(abs(grad) > ROTATE_SPEED_3_SEEK + ROTATE_SPEDD_DELTA && !speed1) {
                            powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_3);
                            speed1 = true;
                            move_mouse_rel((powerX / abs(powerX)) * ROTATE_SPEED_3, 0);
                            qDebug() << "UP rotate speed" << powerX;
                        }

                        if(abs(grad) < ROTATE_SPEED_2_SEEK - ROTATE_SPEDD_DELTA && speed0 ) {
                            powerX = (powerX / abs(powerX)) * (abs(powerX) - ROTATE_SPEED_2 );
                            move_mouse_rel(-((powerX / abs(powerX)) * ROTATE_SPEED_2), 0);
                            speed0 = false;
                            qDebug() << "DOWN rotate speed" << powerX;
                        }
                        if(abs(grad) < ROTATE_SPEED_3_SEEK - ROTATE_SPEDD_DELTA && speed1 ) {
                            powerX = (powerX / abs(powerX)) * (abs(powerX) - ROTATE_SPEED_3);
                            move_mouse_rel(-((powerX / abs(powerX)) * ROTATE_SPEED_3), 0);
                            speed1 = false;
                            qDebug() << "DOWN rotate speed" << powerX;
                        }
                    }

                    if(abs(grad) < 3) {
                        if(!pickdownX) {
                            //                        move_mouse_rel(-powerX, 0);
                            push_key(" ");
                            pickdownX = true;
                            qDebug() << "x rotate complate" << dx << powerX;
                        }
                    }

*/

                    if(!pickup) {
                        if(dy == 0)
                            dy = 1;
                        if(dx == 0)
                            dx = 1;

                        seekY = dy / abs(dy) * 150;
                        seekY *= -1;
                        seekX = dx / abs(dx);
                        qDebug() << "START Y X move" << dy << seekY;
                        move_mouse_rel(0, seekY);
                        pickup = true;
                        if(dx > 0) {
                            press_key("a");
                            QThread::msleep(30);
                            qDebug() << "press_key(a)";
                        } else if(dx < 0) {
                            press_key("d");
                            QThread::msleep(30);
                            qDebug() << "press_key(d)";
                        }

                    }
                    qDebug() << dx;
                    if(seekX > 0 && !pickdownX) {
                        if(dx < 25)
                        {
                            release_key("a");
                            QThread::msleep(30);
                            pickdownX = true;
                            qDebug() << "release_key(a)";
                        }
                    } else if(seekX < 0 && !pickdownX) {
                        if(dx > -25)
                        {
                            release_key("d");
                            pickdownX = true;
                            QThread::msleep(30);
                            qDebug() << "release_key(d)";
                        }
                    }


                    if(seekY > 0 ) {
                        if(dy > 0)
                            if(!pickdownY) {
                                move_mouse_rel(0, -seekY);
                                pickdownY = true;
                            }
                    }
                    else if(seekY <= 0 ){
                        if(dy < 0)
                            if(!pickdownY) {
                                move_mouse_rel(0, -seekY);
                                pickdownY = true;


                            }
                    }


                if(pickdownX && pickdownY)
                {
                    qDebug() << "success aiming" ;
                    pickdownX = false;
                    pickdownY = false;
                    pickup = false;
//                    speed0 = false;
//                    speed1 = false;
//                    rotate = false;
                    m_pControl->check = true;
                    m_pControl->radius = radius;
                    m_pControl->state = AICONTROL;
                }


            } else {
                qDebug() << "exit" ;
                m_pControl->check = false;
                m_pControl->state = AICONTROL;
            }
            break;
        }

        }
        capture->update();
    }
}

void EngineScript::setScript(QJsonObject _script)
{
    script = _script;
}

bool EngineScript::srchAreaOnceInRect(QString as_ImageROI, QString as_rectInWhichLook)
{
    if(mp_dataSet->find(as_rectInWhichLook.toStdString()) != mp_dataSet->end() &&
            mp_dataSet->find(as_ImageROI.toStdString()) != mp_dataSet->end())
        return capture->srchAreaOnceInRect(as_rectInWhichLook.toStdString(), as_rectInWhichLook.toStdString());
    return false;
}

cv::Point EngineScript::getPointAfterLookAreaOnceInRect(QString as_ImageROI, QString as_rectInWhichLook)
{
    if(mp_dataSet->find(as_rectInWhichLook.toStdString()) != mp_dataSet->end() &&
            mp_dataSet->find(as_ImageROI.toStdString()) != mp_dataSet->end())
        return capture->getPointAfterLookAreaOnceInRect(as_rectInWhichLook.toStdString(), as_rectInWhichLook.toStdString());
    return cv::Point();
}

cv::Point EngineScript::getPointAfterLookAreaInRect(QString asImageROI, int anCount, int anStart, int anEnd)
{
    cv::Rect cvRect = calcRectFromPartOfIndex(anCount, anStart, anEnd);
    if(mp_dataSet->find(asImageROI.toStdString()) != mp_dataSet->end())
        return capture->getPointAfterLookAreaInRect(asImageROI.toStdString(), cvRect);
    return cv::Point();
}

//bool EngineScript::srchAreaOnceInPart(QString as_imageROI, int anXCount, int anYCount, int anXStart, int anYStart, int anXEnd, int anYEnd)
//{
//    cv::Rect cvRect = calcRectFromPart(anXCount, anYCount, anXStart, anYStart, anXEnd, anYEnd);
//    if(mp_dataSet->find(as_imageROI.toStdString()) != mp_dataSet->end())
//        return capture->srchAreaOnceInRect(as_imageROI.toStdString(), cvRect);
//    return false;
//}

bool EngineScript::srchAreaOnceInPart(QString as_imageROI, int anCount, int anStart, int anEnd, double coeff)
{
    cv::Rect cvRect = calcRectFromPartOfIndex(anCount, anStart, anEnd);
    if(mp_dataSet->find(as_imageROI.toStdString()) != mp_dataSet->end())
        return capture->srchAreaOnceInRect(as_imageROI.toStdString(), cvRect, coeff);
    return false;
}

cv::Rect EngineScript::calcRectFromPart(int anXCount, int anYCount, int anXStart, int anYStart, int anXEnd, int anYEnd)
{
    int partWidth = USING_WIDTH / anXCount;
    int partHeight = USING_HEIGHT / anYCount;

    if((partWidth + 1) * anXEnd > USING_WIDTH) {
        qDebug() << "border right behind limits";
        return cv::Rect();
    }
    if((partHeight + 1) * anYEnd > USING_HEIGHT) {
        qDebug() << "border bottom behind limits";
        return cv::Rect();
    }

    return cv::Rect((anXStart+1) * partWidth, (anYStart + 1) * partHeight, (partWidth + 1) * anXEnd, (partHeight + 1) * anYEnd);

}

cv::Rect EngineScript::calcRectFromPartOfIndex(int anCount, int aiStart, int aiEnd)
{
    int partWidth = USING_WIDTH / anCount;
    int partHeight = USING_HEIGHT / anCount;
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

QString EngineScript::getImageMoreSimilarInRect(QStringList alistsStations, int anCount, int anStart, int anEnd)
{
    cv::Rect cvRect = calcRectFromPartOfIndex(anCount, anStart, anEnd);
    struct {
        QString name;
        double coeff = 0;
    } pair;
    for(QString asStation: alistsStations) {
        double tmp_coeff = capture->getCoeffImageInRect(asStation.toStdString(), cvRect);
        if(tmp_coeff > pair.coeff) {
            pair.name = asStation;
            pair.coeff = tmp_coeff;
        }
        qDebug() << asStation << "coeff =" << tmp_coeff;
    }
    qDebug() << "coeff =" << pair.coeff;
    return pair.name;
}

void EngineScript::push_key()
{
//    static int i = 0;
//    i++;
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "push_key";
    jMsg["code"] = m_pControl->push_key;
//    if(i == 2)
//        qDebug() << "stop";
    sendDataToSlave(QJsonDocument(jMsg).toJson());
}

void EngineScript::push_key(QString aChar)
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "push_key";
    jMsg["code"] = aChar;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
}

void EngineScript::press_key(QString aChar)
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "press_key";
    jMsg["code"] = aChar;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
}

void EngineScript::release_key(QString aChar)
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "release_key";
    jMsg["code"] = aChar;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
}

void EngineScript::press_key()
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "press_key";
    jMsg["code"] = m_pControl->press_key;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
}

void EngineScript::release_key()
{
    QJsonObject jMsg;
    jMsg["target"] = "keyboard";
    jMsg["method"] = "release_key";
    jMsg["code"] = m_pControl->release_key;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
}

void EngineScript::typingText()
{
    for(int i = 0; i < m_pControl->typingText.size(); i++) {
        QString aChar = m_pControl->typingText.at(i);
        QJsonObject jMsg;
        jMsg["target"] = "keyboard";
        jMsg["method"] = "push_key";
        jMsg["code"] = aChar;
        sendDataToSlave(QJsonDocument(jMsg).toJson());
        QThread::msleep(35);
    }
//    QJsonObject jMsg;
//    jMsg["target"] = "keyboard";
//    jMsg["method"] = "push_f";
//    jMsg["f_code"] = "ENTER";
//    QThread::msleep(50);
    //    sendDataToSlave(QJsonDocument(jMsg).toJson());
}

void EngineScript::move_mouse_rel(int x, int y)
{
    QJsonObject jMsg;
    jMsg["target"] = "mouse";
    jMsg["method"] = "move";
    jMsg["move_type"] = "REL";
    jMsg["x"] = x ;
    jMsg["y"] = y;
    sendDataToSlave(QJsonDocument(jMsg).toJson());
}

void EngineScript::mouse_move_click(cv::Point cvPoint)
{

    QJsonObject jMsg;
    jMsg["target"] = "mouse";
    jMsg["method"] = "move_click";
    jMsg["code"] = "BTN_LEFT";
    jMsg["x"] = cvPoint.x + m_pointOffsetScreen.x();
    jMsg["y"] = cvPoint.y + m_pointOffsetScreen.y();
    sendDataToSlave(QJsonDocument(jMsg).toJson());
}



void EngineScript::sendDataToSlave(QByteArray a_data)
{
    sock->write(a_data);
    sock->waitForBytesWritten();
}

bool EngineScript::menuDocking()
{
    return capture->menuDocking()->activeMenuDocking;
}

//void EngineScript::nextScript()
//{
//    timeElapsed.restart();
//    m_index = m_listScript[m_index]["index"].toInt() + 1;
//    //    m_rectMap.erase("rectMatch");

//}

//void EngineScript::setScript(QString as_scriptName)
//{
////    qDebug() << "switch script:" << as_scriptName;
//    QJsonArray *m_arrayScript = mp_ioData->assignpScript(as_scriptName);
//    setListScript(*m_arrayScript);
//    m_index = 0;
//}

//void EngineScript::setListScript()
//{
//    m_listScript.clear();
//    QJsonArray *arrayScript = mp_ioData->assignpScript();
//    for(QJsonValue jValue: *arrayScript) {
//        m_listScript.insert(jValue.toObject()["index"].toInt(), jValue.toObject());
//    }
//}

//void EngineScript::setListScript(QJsonArray t_jArray)
//{
//    m_listScript.clear();
//    for(QJsonValue jValue: t_jArray) {
//        m_listScript.insert(jValue.toObject()["index"].toInt(), jValue.toObject());
//    }
//}

//void EngineScript::setListScript(QJsonObject t_jObj)
//{
//    m_listScript.clear();
//    m_listScript.insert(t_jObj["index"].toInt(), t_jObj);
//}

void EngineScript::update()
{


    engine();
//    setListScript();
//    m_index = 0;
//    while(cycle) {
//        QJsonObject jObj = m_listScript[m_index];
//        if(jObj["action"].toString() == "srchAreaOnceInRect") {
//            if(srchAreaOnceInRect(jObj["roi"].toString(), jObj["xrect"].toString())) {
//                qDebug() << "hit the target";
////                setScript(jObj["next_script"].toString());
//            } else {
////                qDebug() << "missed the mark";
////                nextScript();
//            }
//        } else if(jObj["action"].toString() == "srchAreaInRectAndCheck") {
//            cv::Point _p = getPointAfterLookAreaOnceInRect(jObj["roi"].toString(), jObj["xrect"].toString());
//                qDebug() << "hit the target";
////                setScript(jObj["next_script"].toString());
//                //                    _jMsgToSend["target"] = "mouse";
//                //                    _jMsgToSend["method"] = "move_click";
//                //                    _jMsgToSend["code"] = "BTN_LEFT";
//                //                    _jMsgToSend["x"] = _point.x + m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat.cols / 2;
//                //                    _jMsgToSend["y"] = _point.y + m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat.rows / 2;

//        } else if(jObj["action"].toString() == "switchScript") {
//            setScript(jObj["next_script"].toString());
//        } else if(jObj["action"].toString() == "sendKeyAndWaitRectArea") {
//            if(_jMsgToSend.isEmpty()) {
//                _jMsgToSend["target"] = "keyboard";
//                _jMsgToSend["method"] = "push_key";
//                _jMsgToSend["code"] = jObj["code"];
//                sendDataToSlave(QJsonDocument(_jMsgToSend).toJson());
//                timeElapsedForFunc.start();
//            } else {

//                cv::Rect cvRect = calcRectFromPartOfIndex(10, 12, 87);
//                capture->addDrawRect(cvRect);
//                if(timeElapsedForFunc.elapsed() > jObj["waitMsec"].toInt()) {
//                    setScript(jObj["next_script"].toString());
//                    qDebug() << "hit the target";
//                    _jMsgToSend = QJsonObject();
//                }
//            }
//        }
//        capture->update();
//    }
}

void EngineScript::openGUI()
{
//    if(capture->checkRectName("selectROI"))
//        ginfo->openDialog(capture->getNamedRect("selectROI"), capture->getSelectMatROI());
//    else
        ginfo->openDialog();
}

void EngineScript::exitEngine()
{
    qDebug() << "slot exit enginer";
    cycle = false;
    qApp->quit();
}

void EngineScript::slotSaveImage(cv::Mat acvMat, QString asName)
{
    mp_ioData->saveImageForDebug(acvMat, asName);
}

//void EngineScript::performScript(QJsonArray t_jArray)
//{
//    m_index = 0;
//    setListScript(t_jArray);
//}

//void EngineScript::parsePushKey(QChar ac_key)
//{

//}
