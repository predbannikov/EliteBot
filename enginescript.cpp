#include "enginescript.h"


QRect  g_screen;


EngineScript::EngineScript(IOData *apIOData, QObject *parent) : QObject(parent)
{

    initDisplay();

}

EngineScript::~EngineScript()
{
    capture->deleteLater();
    qDebug() << "end EngineScript exit";
    delete capture;
}

void EngineScript::readCommand(QStringList &aslistCommand)
{
    if(!aslistCommand.isEmpty()) {
        if(aslistCommand[1] == "2") {
            qDebug() << "aslistCommand[1] = 2";
            m_sRetFunction = aslistCommand[2];
        }
        if(aslistCommand[0] != "DEBUG" && aslistCommand[1] == "1") {
            qDebug() << "Код возврата '1' - сбрасываю скрипт";
            m_pControl->queue.clear();
            emit signalReturnCommand(aslistCommand);
        }
    }
    QStringList command = m_pControl->readCommand();

    if(command[0] == "MARKER") {
        qDebug() << "MARKER" << command;
        QStringList tmpList = command;
        if(!m_sRetFunction.isEmpty()) {
            tmpList[1] = "2";
            tmpList.append(m_sRetFunction);
            m_sRetFunction.clear();
        }
        emit signalReturnCommand(tmpList);
    }

    aslistCommand = command;
}

void EngineScript::engine()
{

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
                g_screen = screens[i]->availableGeometry();
                number = i;
            }
        }
        if(configDisplayFile.open(QFile::WriteOnly | QFile::Text | QFile::Append)) {
            QJsonObject jObj;
            jObj["number"] = number;
            jObj["x"] = g_screen.x();
            jObj["y"] = g_screen.y();
            jObj["width"] = g_screen.width();
            jObj["height"] = g_screen.height();
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
            g_screen.setX(jObj["x"].toInt());
            g_screen.setY(jObj["y"].toInt());
            g_screen.setWidth(jObj["width"].toInt());
            g_screen.setHeight(jObj["height"].toInt());
            qDebug() << number << "monitor" << g_screen;

        } else {
            qDebug() << "not open file to read data file config";
        }
    }
}

//bool EngineScript::srchAreaOnceInRect(QString as_ImageROI, QString as_rectInWhichLook)
//{
//    if(m_pDataSet->find(as_rectInWhichLook.toStdString()) != m_pDataSet->end() &&
//            m_pDataSet->find(as_ImageROI.toStdString()) != m_pDataSet->end())
//        return capture->srchAreaOnceInRect(as_rectInWhichLook.toStdString(), as_rectInWhichLook.toStdString());
//    return false;
//}

cv::Point EngineScript::getPointAfterLookAreaOnceInRect(QString as_ImageROI, QString as_rectInWhichLook)
{
    if(m_pDataSet->find(as_rectInWhichLook.toStdString()) != m_pDataSet->end() &&
            m_pDataSet->find(as_ImageROI.toStdString()) != m_pDataSet->end())
        return capture->getPointAfterLookAreaOnceInRect(as_rectInWhichLook.toStdString(), as_rectInWhichLook.toStdString());
    return cv::Point();
}

//cv::Point EngineScript::getPointAfterLookAreaInRect(QString asImageROI, int anCount, int anStart, int anEnd)
//{
//    cv::Rect cvRect = calcRectFromPartOfIndex(anCount, anStart, anEnd);
//    if(m_pDataSet->find(asImageROI.toStdString()) != m_pDataSet->end())
//        return capture->getPointAfterLookAreaInRect(asImageROI.toStdString(), cvRect);
//    return cv::Point();
//}

//bool EngineScript::srchAreaOnceInPart(QString as_imageROI, int anCount, int anStart, int anEnd, double coeff)
//{
//    cv::Rect cvRect = calcRectFromPartOfIndex(anCount, anStart, anEnd);
//    if(m_pDataSet->find(as_imageROI.toStdString()) != m_pDataSet->end())
//        return capture->srchAreaOnceInRect(as_imageROI.toStdString(), cvRect, coeff);
//    return false;
//}

cv::Rect EngineScript::calcRectFromPart(int anXCount, int anYCount, int anXStart, int anYStart, int anXEnd, int anYEnd)
{
    int partWidth = g_screen.width() / anXCount;
    int partHeight = g_screen.height() / anYCount;

    if((partWidth + 1) * anXEnd > g_screen.width()) {
        qDebug() << "border right behind limits";
        return cv::Rect();
    }
    if((partHeight + 1) * anYEnd > g_screen.height()) {
        qDebug() << "border bottom behind limits";
        return cv::Rect();
    }

    return cv::Rect((anXStart+1) * partWidth, (anYStart + 1) * partHeight, (partWidth + 1) * anXEnd, (partHeight + 1) * anYEnd);

}

cv::Rect EngineScript::calcRectFromPartOfIndex(int anCount, int aiStart, int aiEnd)
{
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

    IOData *pIOData = new IOData(false);
    if(pIOData->prepWorkPath()) {
        qDebug() << "thread IOData load success";
    } else {
        qDebug() << "thread IOData load not success";
    }
    m_pIOData = pIOData;
    m_pDataSet = m_pIOData->assignpDataSet();

//    initDisplay();
//    qDebug() << "test4";



    m_pointOffsetScreen = QPoint( g_screen.x(), g_screen.y());
    capture = new CaptureWindow(m_pDataSet, g_screen.x(), g_screen.y(), g_screen.width(), g_screen.height(), this);
    m_pControl = new AIControl;

    init();

    // for debug
    connect(capture, &CaptureWindow::signalSaveImageForDebug, this, &EngineScript::slotSaveImage);



    timeElapsedForFunc.start();
    update();
}

void EngineScript::slotSetSide(int aSide)
{
    capture->setSide(aSide);
}

void EngineScript::slotFreeze()
{
    capture->freeze();
}

void EngineScript::slotSetQueue(QQueue<QStringList> aQueue)
{
    m_pControl->setQueue(aQueue);
}

void EngineScript::slotResizeImage()
{
    capture->enableResizeImage();
}

void EngineScript::slotStartTest()
{
    m_pControl->test();
}



void EngineScript::slotSaveImage(cv::Mat acvMat, QString asName)
{
//    m_pIOData->saveImageForDebug(acvMat, asName);
    cv::imwrite(PATH_IMAGE_LOG + tr("\\").toStdString() + (asName).toStdString() + ".png", acvMat);

}

void EngineScript::slotSaveVideo()
{
    capture->enableSaveVideo();
}

void EngineScript::slotEngineEnable(bool aState)
{
//    m_sock->mouse_move_click(cv::Point(m_screen.width()/2, m_screen.height()/2));
//    update();
    m_pControl->init();

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

