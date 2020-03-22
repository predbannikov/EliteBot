#include "enginescript.h"


QRect  m_screen;


EngineScript::EngineScript(IOData *apIOData, QObject *parent) : QObject(parent)
{

    m_pIOData = apIOData;
    mp_dataSet = m_pIOData->assignpDataSet();

    initDisplay();



    m_pointOffsetScreen = QPoint( m_screen.x(), m_screen.y());
    capture = new CaptureWindow(mp_dataSet, m_screen.x(), m_screen.y(), m_screen.width(), m_screen.height(), this);
    m_pControl = new AIControl;

    init();

    // for debug
    connect(capture, &CaptureWindow::signalSaveImageForDebug, this, &EngineScript::slotSaveImage);
    cycle = true;


//    sock = new QTcpSocket;
////    sock->connectToHost("192.168.16.128", 10101);
//    sock->connectToHost("127.0.0.1", 10101);


    timeElapsedForFunc.start();
    emit signalOpenGui();
}

EngineScript::~EngineScript()
{
    capture->deleteLater();
    qDebug() << "end EngineScript exit";
    delete capture;
}

void EngineScript::readCommand(QStringList &aslistCommand)
{
    aslistCommand = m_pControl->readCommand();
}

void EngineScript::engine()
{
//    update();
    //    m_pControl->state = RESTOR_GAME;
//    m_pControl->state = DEBUG_STATE;
//    while(cycle) {
//        switch (m_pControl->state) {
//        case DEBUG_STATE:
//            break;
//        case RESTOR_GAME: {
//            mouse_move_click(cv::Point(m_screen.width()/2, m_screen.height()/2));
//            QThread::msleep(50);
//            mouse_move_click(cv::Point(m_screen.width()/2, m_screen.height()/2));
//            m_pControl->state = AICONTROL;
//            break;
//        }
//        case AICONTROL:
//            m_pControl->smallRing();
//            break;
//        case PUSH_KEY:
//            push_key();
//            m_pControl->state = AICONTROL;
//            break;
//        case PRESS_KEY:
//            press_key();
//            m_pControl->state = AICONTROL;
//            break;
//        case RELEASE_KEY:
//            release_key();
//            m_pControl->state = AICONTROL;
//            break;
//        case SEARCH_IMAGE_CONTINUOUS:
//        {
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {
//                if(srchAreaOnceInPart(m_pControl->searchImage, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd, m_pControl->coeff)) {
//                    m_pControl->check = true;
//                    m_pControl->state = AICONTROL;
//                }
//            } else {
//                m_pControl->state = AICONTROL;
//            }
//            break;
//        }
//        case WHILE_IMAGE_CONTINUOUS:
//        {
//            static qint64 lastElapse = 0;
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {
//                if(!srchAreaOnceInPart(m_pControl->searchImage, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd, m_pControl->coeff)) {
//                    if(m_pControl->timeElapsed.elapsed() - lastElapse > m_pControl->whileWaitMsec) {
//                        m_pControl->check = true;
//                        m_pControl->state = AICONTROL;
//                    }
//                    qDebug() << "pattern image hide" << lastElapse;
//                } else {
//                    lastElapse = m_pControl->timeElapsed.elapsed();
//                }
//            } else {
//                lastElapse = 0;
//                m_pControl->check = false;
//                m_pControl->state = AICONTROL;
//            }
//            break;
//        }
//        case WHICH_IMAGE_MORE_SIMILAR:
//            m_pControl->searchImage = getImageMoreSimilarInRect(m_pControl->listForSearch, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd);
//            m_pControl->state = AICONTROL;
//            break;
//        case CLICK_POINT_IMAGE_AFTER_LOOK:
//        {
//            cv::Point cvPoint = getPointAfterLookAreaInRect(m_pControl->searchImage, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd);
//            mouse_move_click(cvPoint);
//            m_pControl->state = AICONTROL;
//            break;
//        }
//        case CLICK_TO_POINT:
//        {
//            cv::Rect rectTmp = mp_dataSet->at(m_pControl->searchImage.toStdString()).rect;
//            cv::Point cvPoint(rectTmp.x + (rectTmp.width / 2), rectTmp.y + (rectTmp.height / 2));
//            mouse_move_click(cvPoint);
//            m_pControl->state = AICONTROL;
//            break;
//        }
//        case TYPING_TEXT:
//            typingText();
//            m_pControl->state = AICONTROL;
//            break;
//        case TRANS_PANEL1:                                                  // *
//        {
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {
//                CursorPanel *pan = capture->panel1Header();
//                if(pan->activeHeader) {
//                    QThread::msleep(500);
//                    m_pControl->cursorPanel = pan;
//                    m_pControl->check = true;
//                    m_pControl->state = AICONTROL;
//                }
//            } else {
//                m_pControl->check = false;
//                m_pControl->state = AICONTROL;
//            }
//            break;
//        }
//        case TRANS_BODY_CURSOR:                                              // *
//        {
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {
//                CursorPanel *pan = capture->panel1Body();
//                if(pan->activeBody) {
//                    m_pControl->cursorPanel = pan;
//                    m_pControl->check = true;
//                    m_pControl->state = AICONTROL;
//                }
//            } else {
//                m_pControl->check = false;
//                m_pControl->state = AICONTROL;
//            }
//        }
//            break;
//        case TRANS_SUB_CURSOR:
//        {
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {
//                CursorPanel *pan = capture->subPanel1Nav();
//                if(pan->activeSubNav) {
//                    m_pControl->cursorPanel = pan;
//                    m_pControl->check = true;
//                    m_pControl->state = AICONTROL;
//                }
//            } else {
//                m_pControl->check = false;
//                m_pControl->state = AICONTROL;
//            }
//            break;
//        }
//        case TRANS_MENU_DOCKING:
//        {
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {
//                CursorPanel *pan = capture->menuDocking();
//                if(pan->activeMenuDocking) {
//                    m_pControl->cursorPanel = pan;
//                    m_pControl->check = true;
//                    m_pControl->state = AICONTROL;
//                }
//            } else {
//                m_pControl->check = false;
//                m_pControl->state = AICONTROL;
//            }
//            break;
//        }
//        case WAIT_MSEC:                                                 // Функция ожидания
//        {
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {

//            } else {
//                m_pControl->check = true;
//                m_pControl->state = AICONTROL;
//            }
//            break;
//        }
//        case APPROACH:                                                 // Функция ожидания
//        {
//            static bool resetDistance = false;
//            static bool approach = false;
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {
//                if(!approach) {
//                    press_key("w");
//                    approach = true;
//                }
//                if(elaps > 4000) {
//                    release_key("w");
//                    approach = true;
//                    for(int i = 0; i < 5; i++) {
//                        push_key("x");
//                        QThread::msleep(1300);
//                        approach = false;
//                        resetDistance = false;
//                        m_pControl->check = true;
//                        m_pControl->state = AICONTROL;
//                    }
//                    qDebug() << "Выход из enginescripte APPROACH";
//                }
////                Distance *distance = capture->recognizDistance();
////                if(!resetDistance) {
////                    distance->distanceList.clear();
////                    distance->distance = 0;
////                    resetDistance = true;
////                } else {
////                    if(!approach) {
////                        press_key("w");
////                        approach = true;
////                    }
////                    if(distance->distance < std::numeric_limits<double>::epsilon () &&
////                       distance->distance > -std::numeric_limits<double>::epsilon () ) {
////                        qDebug() << "distance zero" << distance->distanceList;
////                        break;
////                    }

////                    if(distance->distance <= m_pControl->distance) {
////                        release_key("w");
////                        for(int i = 0; i < 5; i++) {
////                            push_key("x");
////                            QThread::msleep(1300);
////                        }
////                        approach = false;
////                        resetDistance = false;
////                        m_pControl->check = true;
////                        m_pControl->state = AICONTROL;
////                    }
////                }

//            } else {
//                for(int i = 0; i < 5; i++) {
//                    push_key("x");
//                    QThread::msleep(1300);
//                }
//                approach = false;
//                resetDistance = false;
//                m_pControl->check = false;
//                m_pControl->state = AICONTROL;
//            }
//            break;
//        }
//        case SET_FRAME_FREQ:
//            capture->nWaitKey = m_pControl->frameFreq;
//            m_pControl->state = AICONTROL;
//            break;
//        case COMPASS: {
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {
//                Compass *compass = capture->compass();
//                if(!compass->active)
//                    break;
//                int dx = compass->pCenter.x - compass->pDot.x;
//                int dy = compass->pCenter.y - compass->pDot.y;
//                static int powerX;
//                static int seekY;
//                static bool pickup = false;
//                static bool pickdownX = false;
//                static bool pickdownY = false ;
//                static bool rotate = false ;
//                const int approxim = 8;            // Расстояние до центра до которого компас реагирует
//                // Вычисление угла между двумя векторами
//                cv::Point a(compass->pCenter.x - compass->pDot.x, compass->pCenter.y - compass->pDot.y);
//                cv::Point b(0, 1);
//#define SPEED_COMPAS_Y      200
//#define SPEED_COMPAS_ROT    200
//                double radius = sqrt(pow(dx,2) + pow(dy, 2));

//                double atan_a = atan2(a.x * b.y - b.x * a.y, a.x * b.x + a.y * b.y);
//                double grad = atan_a * 180. / M_PI;
//                qDebug() << "grad=" << grad << " radius=" << radius;
//                //  Выравнивание по ротации
//                if(!rotate) {
//                    qDebug() << "radius =" << radius;
//                        if(approxim <= radius) {
//                            if((grad > 10 || grad < -10)) {         // выровнять по градусам
//                                if(dx == 0)
//                                    dx = 1;
//                                powerX = SPEED_COMPAS_ROT * (dx / abs(dx));        // знак полярности
//                                powerX *= -1;
//                                qDebug() << "rotate dx =" << dx << powerX;
//                                move_mouse_rel(powerX, 0);
//                            }
//                        } else {
//                            powerX = 0;
//                            pickdownX = true;
//                            qDebug() << "x rotate complate";
//                        }
//                    rotate = true;
//                }
//                if(grad > -15 && grad < 15)
//                    if(!pickdownX) {
//                        powerX *= -1;
//                        move_mouse_rel(powerX, 0);
//                        pickdownX = true;
//                        qDebug() << "x rotate complate" << dx << powerX;
//                    }
//                //  Выравнивание по оси Y
//                if(!pickup && pickdownX) {
//                    if(approxim <= abs(dy) || !compass->face) {
//                        seekY = -SPEED_COMPAS_Y;
//                    } else {
//                        if(dy < -3) {
//                            seekY = SPEED_COMPAS_Y;
//                        } else {
//                            seekY = 0;
//                            pickdownY = true;
//                            qDebug() << "y complate" ;
//                        }
//                    }
//                    qDebug() << "move Y" << dy << seekY << " face=" << compass->face;
//                    move_mouse_rel(0, seekY);
//                    pickup = true;
//                }
//                if(compass->face && pickdownX) {
//                    if(dy > -3 && dy < approxim  ) {
//                        if(!pickdownY) {
//                            qDebug() << "Делаем обратную силу";
//                            seekY *= -1;
//                            move_mouse_rel(0, seekY);
//                            pickdownY = true;
//                            qDebug() << "y complate" << dy << seekY << " face=" << compass->face;
//                        }
//                    }
//                }
//                if(pickdownX && pickdownY)
//                {
//                    qDebug() << "success compass" ;
//                    QThread::msleep(30);
//                    push_key(" ");
//                    QThread::msleep(30);

//                    pickdownX = false;
//                    pickdownY = false;
//                    pickup = false;
//                    rotate = false;
////                    m_pControl->check = true;
//                    m_pControl->state = AICONTROL;
//                }
//            } else {
//                m_pControl->check = false;
//                m_pControl->state = AICONTROL;
//            }
//            break;
//        }
//        case AIMING:
//        {
//            qint64 elaps = m_pControl->timeElapsed.elapsed();
//            if(elaps < m_pControl->timeWaitMsec) {
//                CursorTarget *tar = capture->takeAimp();

//                int dx = tar->pointCenter.x - tar->pointTarget.x;
//                int dy = tar->pointCenter.y - tar->pointTarget.y;
//                static int powerX;
//                static int seekY;
//                static int seekX;

//                static bool pickup = false;
//                static bool pickdownX = false;
//                static bool pickdownY = false;
//                static bool speed0 = false;
//                static bool speed1 = false;
//                static bool rotate = false;
//                static int count_error = 0;
////                qDebug() << dx;
////                double radius = sqrt(pow(dx,2) + pow(dy, 2));
//                if(!tar->active) {
//                    qDebug() << "target not active err=" << count_error;
//                    if(count_error == 3) {
//                        qDebug() << "target RESET err=" << count_error;
//                        count_error = 0;
//                        push_key(" ");
//                        QThread::msleep(30);
//                        pickup = false;
//                        speed1 = false;
//                        speed0 = false;
//                        pickdownX = false;
//                        rotate = false;
//                        pickdownY = false;
//                        m_pControl->check = false;
//                        m_pControl->target = true;
//                        m_pControl->state = AICONTROL;
//                    }
//                    if(timeElapsedForFunc.elapsed() > 500 ) {
//                        count_error++;
//                        timeElapsedForFunc.restart();
//                    }
//                    break;


//                }
//                count_error = 0;
//                double radius = sqrt(pow(dx,2) + pow(dy, 2));
///*                // Вычисление угла между двумя векторами
//                cv::Point a(tar->pointCenter.x - tar->pointTarget.x, tar->pointCenter.y - tar->pointTarget.y);
//                cv::Point b(0, 1);
//                double atan_a = atan2(a.x * b.y - b.x * a.y, a.x * b.x + a.y * b.y);
//                double grad = atan_a * 180. / M_PI;
//                qDebug() << "grad=" << grad << " radius=" << radius;


//#define ROTATE_SPEED_0  180
//#define ROTATE_SPEED_2  100
//#define ROTATE_SPEED_3  100
//#define ROTATE_SPEED_2_SEEK  45
//#define ROTATE_SPEED_3_SEEK  90
//#define ROTATE_SPEDD_DELTA  7
//                //  Выравнивание по ротации
//                if(!rotate) {
//                    qDebug() << "radius =" << radius;
//                        if(abs(dx) > 5) {
//                            if((grad > 3 || grad < -3)) {         // выровнять по градусам
//                                if(dx == 0)
//                                    dx = 1;
//                                powerX = ROTATE_SPEED_0 * (dx / abs(dx));        // знак полярности
//                                powerX *= -1;
//                                qDebug() << "rotate dx =" << dx << powerX;
////                                if(abs(grad) > 30) {
////                                    powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_2);
////                                    speed0 = true;
////                                }
////                                if(abs(grad) > 90) {
////                                    powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_3);
////                                    speed1 = true;
////                                }
//                                move_mouse_rel(powerX , 0);
//                            }
//                        } else {
//                            powerX = 0;
//                            pickdownX = true;
//                            qDebug() << "x rotate complate" << dx;
//                        }
//                        rotate = true;
//                } else {
//                    if(powerX != 0 && !pickdownX) {
//                        if(abs(grad) > ROTATE_SPEED_2_SEEK + ROTATE_SPEDD_DELTA && !speed0 ) {
//                            powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_2);
//                            speed0 = true;
//                            move_mouse_rel((powerX / abs(powerX)) * ROTATE_SPEED_2, 0);
//                            qDebug() << "UP rotate speed" << powerX;
//                        }
//                        if(abs(grad) > ROTATE_SPEED_3_SEEK + ROTATE_SPEDD_DELTA && !speed1) {
//                            powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_3);
//                            speed1 = true;
//                            move_mouse_rel((powerX / abs(powerX)) * ROTATE_SPEED_3, 0);
//                            qDebug() << "UP rotate speed" << powerX;
//                        }

//                        if(abs(grad) < ROTATE_SPEED_2_SEEK - ROTATE_SPEDD_DELTA && speed0 ) {
//                            powerX = (powerX / abs(powerX)) * (abs(powerX) - ROTATE_SPEED_2 );
//                            move_mouse_rel(-((powerX / abs(powerX)) * ROTATE_SPEED_2), 0);
//                            speed0 = false;
//                            qDebug() << "DOWN rotate speed" << powerX;
//                        }
//                        if(abs(grad) < ROTATE_SPEED_3_SEEK - ROTATE_SPEDD_DELTA && speed1 ) {
//                            powerX = (powerX / abs(powerX)) * (abs(powerX) - ROTATE_SPEED_3);
//                            move_mouse_rel(-((powerX / abs(powerX)) * ROTATE_SPEED_3), 0);
//                            speed1 = false;
//                            qDebug() << "DOWN rotate speed" << powerX;
//                        }
//                    }

//                    if(abs(grad) < 3) {
//                        if(!pickdownX) {
//                            //                        move_mouse_rel(-powerX, 0);
//                            push_key(" ");
//                            pickdownX = true;
//                            qDebug() << "x rotate complate" << dx << powerX;
//                        }
//                    }

//*/

//                    if(!pickup) {
//                        if(dy == 0)
//                            dy = 1;
//                        if(dx == 0)
//                            dx = 1;

//                        seekY = dy / abs(dy) * 150;
//                        seekY *= -1;
//                        seekX = dx / abs(dx);
//                        qDebug() << "START Y X move" << dy << seekY;
//                        move_mouse_rel(0, seekY);
//                        QThread::msleep(30);
//                        pickup = true;
//                        if(dx > 0) {
//                            press_key("a");
//                            QThread::msleep(30);
//                            qDebug() << "press_key(a)";
//                        } else if(dx < 0) {
//                            press_key("d");
//                            QThread::msleep(30);
//                            qDebug() << "press_key(d)";
//                        }

//                    }
//                    if(seekX > 0 && !pickdownX) {
//                        if(dx < 25)
//                        {
//                            release_key("a");
//                            QThread::msleep(30);
//                            pickdownX = true;
//                            qDebug() << "release_key(a)";
//                        }
//                    } else if(seekX < 0 && !pickdownX) {
//                        if(dx > -25)
//                        {
//                            release_key("d");
//                            pickdownX = true;
//                            QThread::msleep(30);
//                            qDebug() << "release_key(d)";
//                        }
//                    }


//                    if(seekY > 0 ) {
//                        if(dy > 0)
//                            if(!pickdownY) {
//                                move_mouse_rel(0, -seekY);
//                                pickdownY = true;
//                            }
//                    }
//                    else if(seekY <= 0 ){
//                        if(dy < 0)
//                            if(!pickdownY) {
//                                move_mouse_rel(0, -seekY);
//                                pickdownY = true;


//                            }
//                    }
//                    qDebug() << "dx =" << dx << " dy =" << dy;


//                if(pickdownX && pickdownY)
//                {
//                    qDebug() << "success aiming" ;
//                    pickdownX = false;
//                    pickdownY = false;
//                    pickup = false;
////                    speed0 = false;
////                    speed1 = false;
////                    rotate = false;
//                    m_pControl->check = true;
//                    m_pControl->radius = radius;
//                    m_pControl->state = AICONTROL;
//                }


//            } else {
//                qDebug() << "exit" ;
//                m_pControl->check = false;
//                m_pControl->state = AICONTROL;
//            }
//            break;
//        }

//        }
//        QCoreApplication::processEvents();
//        capture->update();
//    }
}

void EngineScript::initDisplay()
{
    QFile configDisplayFile(PATH_CONFIG_DISPLAY);
    int number = 0;
    if(!configDisplayFile.exists()) {
        auto screens = QGuiApplication::screens();
        for(int i = 0; i < screens.size(); i++) {
            QMessageBox msgBox;
            QRect rect = screens[i]->availableGeometry();

            msgBox.move(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
            msgBox.setText("этот монитор используется для игры?");
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No);
            int ret = msgBox.exec();
            if(ret == QMessageBox::Ok) {
                qDebug() << screens[i]->size() << " " << i+1 << "monitor" << screens[i]->availableSize() << screens[i]->availableGeometry() << screens[i]->availableVirtualSize() << screens[i]->availableVirtualGeometry() ;
                m_screen = screens[i]->availableGeometry();
                number = i;
            }
        }
        if(configDisplayFile.open(QFile::WriteOnly | QFile::Text | QFile::Append)) {
            QJsonObject jObj;
            jObj["number"] = number;
            jObj["x"] = m_screen.x();
            jObj["y"] = m_screen.y();
            jObj["width"] = m_screen.width();
            jObj["height"] = m_screen.height();
            configDisplayFile.write(QJsonDocument(jObj).toJson());
            configDisplayFile.close();

        } else {
            qDebug() << "Не удаётся записать конфигурацию дисплея";
        }

    } else {
        if(configDisplayFile.open(QFile::ReadOnly | QFile::Text)) {
            QByteArray _array = configDisplayFile.readAll();
            QJsonDocument _jdoc = QJsonDocument::fromJson(_array);
            configDisplayFile.close();
            QJsonObject jObj = _jdoc.object();
            number = jObj["number"].toInt();
            m_screen.setX(jObj["x"].toInt());
            m_screen.setY(jObj["y"].toInt());
            m_screen.setWidth(jObj["width"].toInt());
            m_screen.setHeight(jObj["height"].toInt());
            qDebug() << number << "monitor" << m_screen;

        } else {
            qDebug() << "not open file to read data file config";
        }
    }
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

bool EngineScript::srchAreaOnceInPart(QString as_imageROI, int anCount, int anStart, int anEnd, double coeff)
{
    cv::Rect cvRect = calcRectFromPartOfIndex(anCount, anStart, anEnd);
    if(mp_dataSet->find(as_imageROI.toStdString()) != mp_dataSet->end())
        return capture->srchAreaOnceInRect(as_imageROI.toStdString(), cvRect, coeff);
    return false;
}

cv::Rect EngineScript::calcRectFromPart(int anXCount, int anYCount, int anXStart, int anYStart, int anXEnd, int anYEnd)
{
    int partWidth = m_screen.width() / anXCount;
    int partHeight = m_screen.height() / anYCount;

    if((partWidth + 1) * anXEnd > m_screen.width()) {
        qDebug() << "border right behind limits";
        return cv::Rect();
    }
    if((partHeight + 1) * anYEnd > m_screen.height()) {
        qDebug() << "border bottom behind limits";
        return cv::Rect();
    }

    return cv::Rect((anXStart+1) * partWidth, (anYStart + 1) * partHeight, (partWidth + 1) * anXEnd, (partHeight + 1) * anYEnd);

}

cv::Rect EngineScript::calcRectFromPartOfIndex(int anCount, int aiStart, int aiEnd)
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

void EngineScript::run()
{
    update();
}

//void EngineScript::update()
//{

//    engine();

//}

void EngineScript::slotSaveImage(cv::Mat acvMat, QString asName)
{
    m_pIOData->saveImageForDebug(acvMat, asName);
}

void EngineScript::slotEngineEnable(bool aState)
{
    if(aState) {
        m_pControl->state = RESTOR_GAME;
        qDebug() << "restor game";
    } else {
        m_pControl->state = DEBUG_STATE;
        qDebug() << "debug state";
    }
}

void EngineScript::slotSetCurStation(QString asStation)
{
    qDebug() << "Текущая станция" << asStation;
    m_pControl->setCurStation(asStation);
}

cv::Mat EngineScript::slotCheckRoiMat()
{
    return capture->checkRoiMat();
}

cv::Rect EngineScript::slotCheckRoiRect()
{
    return capture->checkRoiRect();
}

void EngineScript::slotSetDrawLine(bool abCheck, int anCount)
{
    capture->slotSetDrawLine(abCheck, anCount);
}

void EngineScript::setMinScalar(int n1, int n2, int n3)
{
    capture->setMinScalar(cv::Scalar(n1, n2, n3));
}

void EngineScript::setMaxScalar(int n1, int n2, int n3)
{
    capture->setMaxScalar(cv::Scalar(n1, n2, n3));
}

void EngineScript::setMinNumber(int aNumber)
{
    capture->setMinNumber(aNumber);
}

void EngineScript::setMidNumber(int aNumber)
{
    capture->setMidNumber(aNumber);
}

void EngineScript::setMaxNumber(int aNumber)
{
    capture->setMaxNumber(aNumber);
}

