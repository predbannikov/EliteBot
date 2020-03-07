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
        connect(ginfo, &GuiInfo::signalSendMaxContourForLength, capture, &CaptureWindow::slotSetMaxContourForLength);



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


    ginfo->openDialog();
    ginfo->move(1280, 550);


}

void EngineScript::engine()
{
    m_pControl->state = AICONTROL;
//    m_pControl->state = RESTOR_GAME;
    while(cycle) {
        switch (m_pControl->state) {
        case RESTOR_GAME:
            break;
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
                    if(srchAreaOnceInPart(m_pControl->searchImage, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd)) {
                        m_pControl->check = true;
                        m_pControl->state = AICONTROL;
                    }
                } else {
                    m_pControl->state = AICONTROL;
                }
            }
            break;
        case WHICH_IMAGE_MORE_SIMILAR:
            m_pControl->searchImage = getImageMoreSimilarInRect(m_pControl->listForSearch, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd);
            m_pControl->state = AICONTROL;
            break;
        case CLICK_POINT_IMAGE_AFTER_LOOK:
            {
                cv::Point cvPoint = getPointAfterLookAreaInRect(m_pControl->searchImage, m_pControl->nCount, m_pControl->iStart, m_pControl->iEnd);
                mouse_move_click(cvPoint);
                m_pControl->state = AICONTROL;
            }
            break;
        case CLICK_TO_POINT:
            {
                cv::Rect rectTmp = mp_dataSet->at(m_pControl->searchImage.toStdString()).rect;
                cv::Point cvPoint(rectTmp.x + (rectTmp.width / 2), rectTmp.y + (rectTmp.height / 2));
                mouse_move_click(cvPoint);
                m_pControl->state = AICONTROL;
            }
            break;
        case TYPING_TEXT:
            typingText();
            m_pControl->state = AICONTROL;
            break;
        case TRANS_PANEL1:
            {
                qint64 elaps = m_pControl->timeElapsed.elapsed();
                if(elaps < m_pControl->timeWaitMsec) {
                    CursorPanel *pan = capture->panel1();
                    if(pan->headerActive) {
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
        case TRANS_BODY_CURSOR:
            {
                qint64 elaps = m_pControl->timeElapsed.elapsed();
                if(elaps < m_pControl->timeWaitMsec) {
                    CursorPanel *pan = capture->panel1();
                    if(pan->headerActive && pan->bodyActive) {
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
                    if(pan->subNavList) {
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
            }
            break;
        case WAIT_MSEC:                                                 // Функция ожидания
            {
                qint64 elaps = m_pControl->timeElapsed.elapsed();
                if(elaps < m_pControl->timeWaitMsec) {

                } else {
                    m_pControl->check = true;
                    m_pControl->state = AICONTROL;
                }
            }
            break;
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

bool EngineScript::srchAreaOnceInPart(QString as_imageROI, int anCount, int anStart, int anEnd)
{
    cv::Rect cvRect = calcRectFromPartOfIndex(anCount, anStart, anEnd);
    if(mp_dataSet->find(as_imageROI.toStdString()) != mp_dataSet->end())
        return capture->srchAreaOnceInRect(as_imageROI.toStdString(), cvRect);
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
    jMsg["method"] = "press_key";
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
    QJsonObject _jMsgToSend;
    _jMsgToSend["target"] = "mouse";
    _jMsgToSend["method"] = "move_click";
    _jMsgToSend["code"] = "BTN_LEFT";
    _jMsgToSend["x"] = 2100;
    _jMsgToSend["y"] = -USING_HEIGHT + 150;
    sendDataToSlave(QJsonDocument(_jMsgToSend).toJson());           // Активировать окно справа сверху
    _jMsgToSend = QJsonObject();
    QThread::msleep(150);

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
