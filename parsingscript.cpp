#include "parsingscript.h"

ParsingScript::ParsingScript(QObject *parent) :
    QObject(parent)
{

}

ParsingScript::~ParsingScript()
{

}

void ParsingScript::parse()
{

//    if(timeElapsed.elapsed() > 25000) {
//        qDebug() << "need reset";
//        setScript("reset");

//    }
//    if(m_stack.toggle == true) {
//        timeElapsedFive.restart();
//        m_stack.toggle = false;
//        qDebug() << "m_stack.toggle == true, m_index =" << m_index;
//        if(m_listScript.size() == m_index && m_listScript == m_stack.list) {
//            setListScript(m_stack.jArray);
//            m_index = m_stack.index;
//        } else {
//            m_stack.list.clear();
//        }
//    }
//    if(timeElapsedFive.elapsed() > 50000) {

//        qDebug() << "m_stack.toggle == false";
//        m_stack.toggle = true;
//        m_stack.jArray = QJsonArray();
//        for(QJsonObject _jObj: m_listScript) {
//            m_stack.jArray.append(_jObj);
//        }
//        m_stack.index = m_index;
//        setScript("check_window");
//        m_stack.list = m_listScript;
//    }

//    if(m_index >= m_listScript.size()) {
//        if(m_listRecord.size() != 0) {
//            qDebug() << m_jArrRecord;
//            m_jArrRecord = QJsonArray();
//            m_listRecord.clear();
//        }
//        m_index = loop ? 0 : m_index;
//    } else {
//        if(m_listScript[m_index]["action"].toString() == "roi") {
//            if(m_dataSet->find(m_listScript[m_index]["roi"].toString().toStdString()) != m_dataSet->end()) {
//                cv::Point _point;
//                cv::Mat _roiMat = m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat;
//                if(findPointRoi(_roiMat, win, _point)) {
//                    m_listRecord << m_listScript[m_index]["roi"].toString();                        // rec
//                    cv::Rect rectMatch(_point.x , _point.y, _roiMat.size().width, _roiMat.size().height);
//                    if(m_rectMap.find("rectMatch") == m_rectMap.end())
//                        m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//                    else
//                        m_rectMap["rectMatch"] = rectMatch;

//                    QJsonObject _jMsgToSend = m_listScript[m_index]["object"].toObject();
//                    _jMsgToSend["x"] = _point.x + m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat.cols / 2;
//                    _jMsgToSend["y"] = _point.y + m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat.rows / 2;
//                    emit sendDataToSlave(_jMsgToSend);
////                    sock->write(QJsonDocument(_jMsgToSend).toJson());
////                    sock->waitForBytesWritten();
//                }
//                nextScript();
//            }
//        } else if(m_listScript[m_index]["action"].toString() == "roi_wait") {
//            if(m_dataSet->find(m_listScript[m_index]["roi"].toString().toStdString()) != m_dataSet->end()) {
//                cv::Point _point;
//                cv::Mat _roiMat = m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat;
//                if(findPointRoi(_roiMat, win, _point)) {
//                    cv::Rect rectMatch(_point.x , _point.y, _roiMat.size().width, _roiMat.size().height);
//                    if(m_rectMap.find("rectMatch") == m_rectMap.end())
//                        m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//                    else
//                        m_rectMap["rectMatch"] = rectMatch;
//                    nextScript();
//                }
//            }

//        } else if(m_listScript[m_index]["action"].toString() == "move_cursor") {
//            QJsonObject _jMsgToSend = m_listScript[m_index];
//            emit sendDataToSlave(_jMsgToSend);
////            sock->write(QJsonDocument(_jMsgToSend).toJson());
////            sock->waitForBytesWritten();
//            nextScript();
//        } else if(m_listScript[m_index]["action"].toString() == "wait_msec") {
//            qDebug() << "wait_sec" << m_listScript[m_index]["msec"].toInt();
//            QThread::msleep(static_cast<unsigned long>(m_listScript[m_index]["msec"].toInt()));
//            timeElapsedFive.restart();
//            timeElapsed.restart();
//            nextScript();
//        } else if(m_listScript[m_index]["action"].toString() == "type") {
//            if(m_fieldToRecord.contains(m_listRecord.last())) {
//                QJsonObject _jObj;
//                _jObj[m_listRecord.last()] = m_listScript[m_index]["object"].toObject()["text"].toString();
//                m_jArrRecord.append(_jObj);                // rec

//            }
//            QJsonObject _jMsgToSend = m_listScript[m_index]["object"].toObject();
//            emit sendDataToSlave(_jMsgToSend);
////            sock->write(QJsonDocument(_jMsgToSend).toJson());
////            sock->waitForBytesWritten();
//            nextScript();
//        } else if(m_listScript[m_index]["action"].toString() == "push_f") {
//            QJsonObject _jMsgToSend;
//            _jMsgToSend["target"] = "keyboard";
//            _jMsgToSend["method"] = "push_f";
//            _jMsgToSend["f_code"] = "KEY_F5";
//            emit sendDataToSlave(_jMsgToSend);
////            sock->write(QJsonDocument(_jMsgToSend).toJson());
////            sock->waitForBytesWritten();
//            nextScript();
//        } else if(m_listScript[m_index]["action"].toString() == "switch") {
//            setScript(m_listScript[m_index]["next_script"].toString());
//        } else if(m_listScript[m_index]["action"].toString() == "pull_point") {
//            if(m_dataSet->find(m_listScript[m_index]["xrect"].toString().toStdString()) != m_dataSet->end() &&
//                    m_dataSet->find(m_listScript[m_index]["roi"].toString().toStdString()) != m_dataSet->end()) {
//                cv::Mat _matXRoi;
//                cv::Rect _rect = m_dataSet->at(m_listScript[m_index]["xrect"].toString().toStdString()).rect;
//                win(_rect).copyTo(_matXRoi);
//                cv::Point _point;
//                cv::Mat _roiMat;
//                _roiMat = m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat;

//                if(findPointRoi(_roiMat, _matXRoi, _point)) {
////                    qDebug() << "find abs_wait_once";
//                    cv::Rect rectMatch(_rect.x + _point.x , _rect.y + _point.y, _roiMat.size().width, _roiMat.size().height);
//                    //                        Helper::printRect(rectMatch);
//                    if(m_rectMap.find("rectMatch") == m_rectMap.end())
//                        m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//                    else
//                        m_rectMap["rectMatch"] = rectMatch;
//                    QJsonObject _jMsgToSend;
//                    _jMsgToSend["x"] = _rect.x + _point.x  + _roiMat.size().width / 2;
//                    _jMsgToSend["y"] = _rect.y + _point.y + _roiMat.size().height / 2;
//                    _jMsgToSend["target"] = "mouse";
//                    _jMsgToSend["method"] = "move_click";
//                    _jMsgToSend["code"] = "BTN_LEFT";
////                    qDebug() << _jMsgToSend["x"].toInt() << _jMsgToSend["y"].toInt();
//                    for(int i = 0; i < 8; i++) {
//                        emit sendDataToSlave(_jMsgToSend);
////                        sock->write(QJsonDocument(_jMsgToSend).toJson());
////                        sock->waitForBytesWritten();
//                        QThread::msleep(15);
//                    }

//                    setScript(m_listScript[m_index]["next_script"].toString());
//                    return;
//                }
//                nextScript();
//            }
//        } else if(m_listScript[m_index]["action"].toString() == "wait") {
//            if(m_dataSet->find(m_listScript[m_index]["xroi"].toString().toStdString()) != m_dataSet->end() &&
//                    m_dataSet->find(m_listScript[m_index]["roi"].toString().toStdString()) != m_dataSet->end()) {
//                cv::Rect _rect;
//                if(findRectRoi(m_dataSet->at(m_listScript[m_index]["xroi"].toString().toStdString()).mat, _rect)) {
//                    cv::Mat _matXRoi;
//                    win(_rect).copyTo(_matXRoi);
//                    cv::Point _point;
//                    cv::Mat _roiMat = m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat;
//                    if(findPointRoi(_roiMat, _matXRoi, _point)) {
//                        qDebug() << "find";
//                        cv::Rect rectMatch(_rect.x + _point.x , _rect.y + _point.y, _roiMat.size().width, _roiMat.size().height);
//                        //                        Helper::printRect(rectMatch);
//                        if(m_rectMap.find("rectMatch") == m_rectMap.end())
//                            m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//                        else
//                            m_rectMap["rectMatch"] = rectMatch;

//                        nextScript();
//                    }
//                }
//            }
//        } else if(m_listScript[m_index]["action"].toString() == "wait_set_script") {
//            if(m_dataSet->find(m_listScript[m_index]["xroi"].toString().toStdString()) != m_dataSet->end() &&
//                    m_dataSet->find(m_listScript[m_index]["roi"].toString().toStdString()) != m_dataSet->end()) {
//                cv::Rect _rect;
//                if(findRectRoi(m_dataSet->at(m_listScript[m_index]["xroi"].toString().toStdString()).mat, _rect)) {
//                    cv::Mat _matXRoi;
//                    win(_rect).copyTo(_matXRoi);
//                    cv::Point _point;
//                    cv::Mat _roiMat = m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat;
//                    if(findPointRoi(_roiMat, _matXRoi, _point)) {
//                        qDebug() << "find";
//                        cv::Rect rectMatch(_rect.x + _point.x , _rect.y + _point.y, _roiMat.size().width, _roiMat.size().height);
//                        //                        Helper::printRect(rectMatch);
//                        if(m_rectMap.find("rectMatch") == m_rectMap.end())
//                            m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//                        else
//                            m_rectMap["rectMatch"] = rectMatch;
//                        setScript(m_listScript[m_index]["next_script"].toString());
//                        return;
//                        //                        QJsonObject _jMsgToSend = m_listScripg[m_index]["object"].toObject();
//                        //                        _jMsgToSend["x"] = _point.x + m_dataSet->at(m_listScripg[m_index]["roi"].toString().toStdString()).mat.cols / 2;
//                        //                        _jMsgToSend["y"] = _point.y + m_dataSet->at(m_listScripg[m_index]["roi"].toString().toStdString()).mat.rows / 2;
//                        //                        sock->write(QJsonDocument(_jMsgToSend).toJson());
//                        //                        sock->waitForBytesWritten();
//                    }
//                }
//            }
//            nextScript();
//        } else if(m_listScript[m_index]["action"].toString() == "wait_block_xrect") {
//            if(m_dataSet->find(m_listScript[m_index]["roi"].toString().toStdString()) != m_dataSet->end()) {
//                cv::Rect _rect;
//                cv::Point _point;
//                cv::Mat _roiMat = m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat;
//                if(findPointRoi(_roiMat, win, _point)) {
//                    qDebug() << "find";
//                    cv::Rect rectMatch(_rect.x + _point.x , _rect.y + _point.y, _roiMat.size().width, _roiMat.size().height);
//                    //                        Helper::printRect(rectMatch);
//                    if(m_rectMap.find("rectMatch") == m_rectMap.end())
//                        m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//                    else
//                        m_rectMap["rectMatch"] = rectMatch;

//                    QJsonObject _jMsgToSend = m_listScript[m_index]["object"].toObject();
//                    _jMsgToSend["target"] = "mouse";
//                    _jMsgToSend["method"] = "move_click";
//                    _jMsgToSend["code"] = "BTN_LEFT";
//                    _jMsgToSend["x"] = _point.x + m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat.cols / 2;
//                    _jMsgToSend["y"] = _point.y + m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat.rows / 2;
//                    emit sendDataToSlave(_jMsgToSend);
////                    sock->write(QJsonDocument(_jMsgToSend).toJson());
////                    sock->waitForBytesWritten();
//                    setScript(m_listScript[m_index]["next_script"].toString());
//                    nextScript();
//                }
//            }
//        } else if(m_listScript[m_index]["action"].toString() == "wait_once") {
//            if(m_dataSet->find(m_listScript[m_index]["roi"].toString().toStdString()) != m_dataSet->end()) {
//                cv::Rect _rect;
//                cv::Point _point;
//                cv::Mat _roiMat = m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat;
//                if(findPointRoi(_roiMat, win, _point)) {
//                    qDebug() << "find";
//                    cv::Rect rectMatch(_rect.x + _point.x , _rect.y + _point.y, _roiMat.size().width, _roiMat.size().height);
//                    //                        Helper::printRect(rectMatch);
//                    if(m_rectMap.find("rectMatch") == m_rectMap.end())
//                        m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//                    else
//                        m_rectMap["rectMatch"] = rectMatch;

//                    QJsonObject _jMsgToSend = m_listScript[m_index]["object"].toObject();
//                    _jMsgToSend["target"] = "mouse";
//                    _jMsgToSend["method"] = "move_click";
//                    _jMsgToSend["code"] = "BTN_LEFT";
//                    _jMsgToSend["x"] = _point.x + m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat.cols / 2;
//                    _jMsgToSend["y"] = _point.y + m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat.rows / 2;
////                    sock->write(QJsonDocument(_jMsgToSend).toJson());
////                    sock->waitForBytesWritten();
//                    emit sendDataToSlave(_jMsgToSend);
//                    setScript(m_listScript[m_index]["next_script"].toString());
//                    return;
//                }
//                nextScript();
//            }
//        } else if(m_listScript[m_index]["action"].toString() == "once_set_script") {
//            if(m_dataSet->find(m_listScript[m_index]["xroi"].toString().toStdString()) != m_dataSet->end() &&
//                    m_dataSet->find(m_listScript[m_index]["roi"].toString().toStdString()) != m_dataSet->end()) {
//                cv::Rect _rect;
//                if(findRectRoi(m_dataSet->at(m_listScript[m_index]["xroi"].toString().toStdString()).mat, _rect)) {
//                    cv::Mat _matXRoi;
//                    win(_rect).copyTo(_matXRoi);
//                    cv::Point _point;
//                    cv::Mat _roiMat = m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat;
//                    if(findPointRoi(_roiMat, _matXRoi, _point)) {
//                        qDebug() << "find";
//                        cv::Rect rectMatch(_rect.x + _point.x , _rect.y + _point.y, _roiMat.size().width, _roiMat.size().height);
//                        if(m_rectMap.find("rectMatch") == m_rectMap.end())
//                            m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//                        else
//                            m_rectMap["rectMatch"] = rectMatch;
//                        setScript(m_listScript[m_index]["next_script"].toString());
//                        QJsonObject _jMsgToSend;
//                        _jMsgToSend["x"] = _rect.x + _point.x  + _roiMat.size().width / 2;
//                        _jMsgToSend["y"] = _rect.y + _point.y + _roiMat.size().height / 2;
//                        _jMsgToSend["target"] = "mouse";
//                        _jMsgToSend["method"] = "move_click";
//                        _jMsgToSend["code"] = "BTN_LEFT";
//                        Helper::printRect(rectMatch);
//                        qDebug() << _jMsgToSend["x"].toInt() << _jMsgToSend["y"].toInt() << " point.x" << _point.x << " point.y" << _point.y;
////                        sock->write(QJsonDocument(_jMsgToSend).toJson());
////                        sock->waitForBytesWritten();
//                        emit sendDataToSlave(_jMsgToSend);

//                        return;
//                    }
//                }
//                nextScript();
//            }
//        } else if(m_listScript[m_index]["action"].toString() == "abs_wait_once") {
//            if(m_dataSet->find(m_listScript[m_index]["xrect"].toString().toStdString()) != m_dataSet->end() &&
//                    m_dataSet->find(m_listScript[m_index]["roi"].toString().toStdString()) != m_dataSet->end()) {
//                cv::Mat _matXRoi;
//                cv::Rect _rect = m_dataSet->at(m_listScript[m_index]["xrect"].toString().toStdString()).rect;
//                win(_rect).copyTo(_matXRoi);
//                cv::Point _point;
//                cv::Mat _roiMat;
//                if(m_listScript[m_index]["roi"].toString() == "sneak1")
//                    _roiMat = m_dataSet->at(QString("zedra3").toStdString()).mat;
////                _roiMat = m_dataSet->at(QString("wolfone1").toStdString()).mat;
////                    _roiMat = m_dataSet->at(QString("wolf2").toStdString()).mat;
//                else
//                    _roiMat = m_dataSet->at(m_listScript[m_index]["roi"].toString().toStdString()).mat;

//                if(findPointRoi(_roiMat, _matXRoi, _point, 0.99)) {
//                    setScript(m_listScript[m_index]["next_script"].toString());
////                    qDebug() << "find abs_wait_once";
//                    cv::Rect rectMatch(_rect.x + _point.x , _rect.y + _point.y, _roiMat.size().width, _roiMat.size().height);
//                    //                        Helper::printRect(rectMatch);
//                    if(m_rectMap.find("rectMatch") == m_rectMap.end())
//                        m_rectMap.insert(std::pair<std::string, cv::Rect >("rectMatch", rectMatch));
//                    else
//                        m_rectMap["rectMatch"] = rectMatch;
//                    QJsonObject _jMsgToSend;
//                    _jMsgToSend["x"] = _rect.x + _point.x  + _roiMat.size().width / 2;
//                    _jMsgToSend["y"] = _rect.y + _point.y + _roiMat.size().height / 2;
//                    _jMsgToSend["target"] = "mouse";
//                    _jMsgToSend["method"] = "move_click";
//                    _jMsgToSend["code"] = "BTN_LEFT";
////                    qDebug() << _jMsgToSend["x"].toInt() << _jMsgToSend["y"].toInt();
//                    emit sendDataToSlave(_jMsgToSend);
////                    sock->write(QJsonDocument(_jMsgToSend).toJson());
////                    sock->waitForBytesWritten();
//                    return;
//                }
//                nextScript();
//            }
//        }
//    }
}

//void ParsingScript::nextScript()
//{
//    timeElapsed.restart();
//    m_index = m_listScript[m_index]["index"].toInt() + 1;
//    //    m_rectMap.erase("rectMatch");

//}

//void ParsingScript::setScript(QString a_sScriptName)
//{
////    qDebug() << "switch script:" << a_sScriptName;
////    m_arrayScript = m_ioData->assignPointProject(a_sScriptName);
////    setListScript(*m_arrayScript);
////    m_index = 0;
//}
