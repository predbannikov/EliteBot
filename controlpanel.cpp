#include "controlpanel.h"
#include "ui_controlpanel.h"

ControlPanel::ControlPanel(EngineScript *aEngine, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ControlPanel)
{
    ui->setupUi(this);
    loadJson();


    m_engine = aEngine;
    connect(this, &ControlPanel::signalEngineEnable, m_engine, &EngineScript::slotEngineEnable, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalEngineSetCurStation, m_engine, &EngineScript::slotSetCurStation, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalEngineEnableTest, m_engine, &EngineScript::slotStartTest, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalFreeze, m_engine, &EngineScript::slotFreeze, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalResizeImage, m_engine, &EngineScript::slotResizeImage, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalSetQueue, m_engine, &EngineScript::slotSetQueue, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalSetSide, m_engine, &EngineScript::slotSetSide, Qt::QueuedConnection);
    connect(m_engine, &EngineScript::signalReturnCommand, this, &ControlPanel::slotReceivReturnCommand, Qt::QueuedConnection);



    QWidget *mainWidgte = new QWidget;

    QVBoxLayout *vbl = new QVBoxLayout;

    QWidget *wgt = new QWidget;
    QScrollArea *scrollArea = new QScrollArea;

    mainWidgte->setLayout(vbl);
    vbl->addWidget(scrollArea);

    scrollArea->setWidget(wgt);
    scrollArea->setWidgetResizable(true);

    vblayout = new QVBoxLayout;



    centralWidget()->layout()->addWidget(mainWidgte);

    wgt->setLayout(vblayout);

    QPushButton *pushAddFlyConfig = new QPushButton("+");
    vbl->addWidget(pushAddFlyConfig);
    connect(pushAddFlyConfig, &QPushButton::clicked, [this] () {
        addConfig();
    });
//    pushAddFlyConfig->clicked();
//    addConfig();
    loadData();

}

void ControlPanel::loadJson()
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

                mapSystemsAndStationLoad.insert(sKey, jArr[i].toObject()["station"].toString()) ;
            }
        }
        file.close();
    }
//    qDebug() << map;
}

ControlPanel::~ControlPanel()
{
    delete ui;
}

void ControlPanel::setCursorStation()
{
    qDebug() << "setCursorStation";
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "1"});      // включить панель
    queue.enqueue(QStringList {"PANEL1CASEHEADER",          "0", "навигация"});

    if(state_delivery == STATE_FROM_STATION) {
        queue.enqueue(QStringList {"PANEL1CASEMENUNAV",         "0", m_sFromStationTarget, m_sFromSystemTarget});
    } else if(state_delivery == STATE_TO_STATION) {
        queue.enqueue(QStringList {"PANEL1CASEMENUNAV",         "0", m_sToStationTarget, m_sToSystemTarget});
    }

    queue.enqueue(QStringList {"PANEL1SUBNAV",              "0", "fix_target"});
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "0"});      // выключить панель
    queue.enqueue(QStringList {"MARKER",                    "0", "setCursorStation"});
}

void ControlPanel::setCursorSystem()
{
    qDebug() << "setCursorSystem";
    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "m" });
    if(state_delivery == STATE_FROM_STATION) {
        queue.enqueue(QStringList {"MAPSYSTEM", "0", m_sFromSystemTarget});
    } else if(state_delivery == STATE_TO_STATION) {

        queue.enqueue(QStringList {"MAPSYSTEM", "0", m_sToSystemTarget});
    }
    queue.enqueue(QStringList {"MARKER",                    "0", "setCursorSystem"});
}

void ControlPanel::test()
{
    queue.clear();
    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "," });

    //    queue.enqueue(QStringList {"MARKER",                    "0", "test"});
}

void ControlPanel::transitToHyper()
{
//    queue.enqueue(QStringList {"PICKUPSPEED",               "0", "7000"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "TAB" });
    queue.enqueue(QStringList {"PICKUPSPEED",               "0", "6000"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "j" });
    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "10000"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "x" });
    queue.enqueue(QStringList {"MARKER",                    "0", "transitToHyper"});
}

void ControlPanel::dockAutoStart()
{
    qDebug() << "dockAutoStart";
    queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_autostart"});
    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "20000"});
    queue.enqueue(QStringList {"IMAGEEXPECTEDCLOSE",        "0", "pic_autoPilot", "10000", "0.80", "17", "92", "128", ""});
//    queue.enqueue(QStringList {"IMAGEEXPECTEDCLOSE",        "0", "pic_warningRadTriangle", "9000", "0.83", "19", "123", "143"});
//    queue.enqueue(QStringList {"STOPAFTERAUTOSTART",        "0"});
    queue.enqueue(QStringList {"PICKUPSPEED",               "0", "15000"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "j" });
    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "11000"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "x" });
    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "3000"});
    queue.enqueue(QStringList {"MARKER",                    "0", "dockAutoStart"});
}

void ControlPanel::flyToSystem()
{
    qDebug() << "flyToSystem";
    queue.enqueue(QStringList {"AIMPFLYAROUND",             "0", "system"});
    queue.enqueue(QStringList {"PICKUPSPEED",               "0", "6500"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "k" });

    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "25000"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "x" });
    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "1000"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "x" });
    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "1000"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "x" });

    queue.enqueue(QStringList {"IMAGEEXPECTED",             "0", "pic_hypermodResetPull", "0.93", "9", "6", "26"});
    queue.enqueue(QStringList {"MARKER",                    "0", "flyToSystem"});
}

void ControlPanel::flytoStation()
{
    qDebug() << "flytoStation";



    queue.enqueue(QStringList {"AIMPFLYAROUND",             "0", "station"});
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "1"});
    queue.enqueue(QStringList {"PANEL1CASEHEADER",          "0", "навигация"});
    if(state_delivery == STATE_FROM_STATION) {
        queue.enqueue(QStringList {"PANEL1CASEMENUNAV",         "0", m_sFromStationTarget, m_sFromSystemTarget});
    } else if(state_delivery == STATE_TO_STATION) {
        queue.enqueue(QStringList {"PANEL1CASEMENUNAV",         "0", m_sToStationTarget, m_sToSystemTarget});
    }

    queue.enqueue(QStringList {"PANEL1SUBNAV",              "0", "enable_hypermode_helper"});
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "0"});
    queue.enqueue(QStringList {"PICKUPSPEED",               "0", "6500"});      // !!!!!!!!!!!!!!!!
    queue.enqueue(QStringList {"MARKER",                    "0", "flytoStation"});
}

void ControlPanel::waitFlyToStation()
{
    qDebug() << "waitFlyToStation";
    queue.enqueue(QStringList {"IMAGEEXPECTEDCLOSE",        "0", "pic_autoPilot", "10000", "0.80", "11", "38", "49", "flyToStation"});
    queue.enqueue(QStringList {"IMAGEEXPECTEDCLOSE",        "0", "pic_warningRadTriangle", "2000", "0.75", "10", "34", "35", ""});
    queue.enqueue(QStringList {"MARKER",                    "0", "waitFlyToStation"});
}

//      ###################################################################################################
//      ######################                      LANDING                      ##########################
//      ###################################################################################################

void ControlPanel::landing()
{
    qDebug() << "landing";
    queue.enqueue(QStringList {"ACTIONGETCLOSER",           "0"});
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "1"});
    queue.enqueue(QStringList {"PANEL1CASEHEADER",          "0", "контакты"});
    if(state_delivery == STATE_FROM_STATION) {
        queue.enqueue(QStringList {"PANEL1CASEMENUCONT",        "0", m_sFromStationTarget});
        queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "0"});
        queue.enqueue(QStringList {"WAITMENUDOCKING",           "0"});
        queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_service"});
        queue.enqueue(QStringList {"ACTIONDELIVERYPAPER",       "0", m_sFromNameActionCargo, m_sCountCargo, "загрузка", m_sTypeCargoUpload});


    } else if(state_delivery == STATE_TO_STATION) {
        queue.enqueue(QStringList {"PANEL1CASEMENUCONT",        "0", m_sToStationTarget});
        queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "0"});
        queue.enqueue(QStringList {"WAITMENUDOCKING",           "0"});
        queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_service"});
        queue.enqueue(QStringList {"ACTIONDELIVERYPAPER",       "0", m_sToNameActionCargo, m_sCountCargo, "разгрузка", m_sTypeCargoUpload});
    }

    if(m_jObject["refuel"].toInt() == 1) {
        queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_service"});
        queue.enqueue(QStringList {"IMAGEEXPECTED",             "0", "menuServiceExit", "0.9", "4", "12", "13"});
        queue.enqueue(QStringList {"ACTIONWAIT",                "0", "1000"});
        queue.enqueue(QStringList {"SERVICEMENU",               "0"});
    }
//    if(m_jObject["actionDeliver"].toString() == "доставка") {
//        queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_service"});
//        queue.enqueue(QStringList {"IMAGEEXPECTED",             "0", "menuServiceExit", "0.9", "4", "12", "13"});

//        queue.enqueue(QStringList {"ACTIONDELIVERYPAPER",       "0", m_jObject["typeAction"].toString(), "4", m_jObject["typeDeliver"].toString()});
////        queue.enqueue(QStringList {"ACTIONDELIVERYPAPER",       "0", "LOADING", "4", "case"});
//    }
    queue.enqueue(QStringList {"MARKER",                    "0", "landing"});

}


//      #######################################################################################################
//      #######################################################################################################
//      #######################################################################################################

void ControlPanel::flyAround()
{
    qDebug() << "flyAround";
    queue.enqueue(QStringList {"AIMPFLYAROUND",             "0", "station"});
    queue.enqueue(QStringList {"MARKER",                    "0", "flyAround"});
}

void ControlPanel::setTrack()
{
    qDebug() << "создаём новый скрипт setTrack";
    queue.clear();
    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "m" });
    if(state_delivery == STATE_FROM_STATION) {
        queue.enqueue(QStringList {"MAPSYSTEM",             "0", m_sFromSystemTarget});
    } else if(state_delivery == STATE_TO_STATION) {
        queue.enqueue(QStringList {"MAPSYSTEM",             "0", m_sToSystemTarget});
    }

    queue.enqueue(QStringList {"MARKER",                    "0", "setTrack"});


}

void ControlPanel::checkCurSystem()
{
    queue.clear();
//    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "mouse_move", QString::number(g_screen.width() / 2), QString::number(g_screen.height() / 2) }); // Поставить в центр
    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "m" });
//    queue.enqueue(QStringList {"GETSTRSTATICFIELD",         "0", "pic_fieldSystemName"});
//    queue.enqueue(QStringList {"CHECKCURSYSTEM",            "0", "response"});
    if(state_delivery == STATE_FROM_STATION) {
        queue.enqueue(QStringList {"CHECKCURSYSTEM",            "0", m_sFromSystemTarget});
    } else if(state_delivery == STATE_TO_STATION) {
        queue.enqueue(QStringList {"CHECKCURSYSTEM",            "0", m_sToSystemTarget});
    }

    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "m" });
    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "2000"});
    queue.enqueue(QStringList {"MARKER",                    "0", "checkCurSystem"});
}

void ControlPanel::prepScript()
{
    qDebug() << "Чтение очереди ";

    m_jObject = takeJsonObj();
    qDebug() << m_jObject;


    initKeyNames();


    qDebug() << m_sFromSystemTarget << m_sFromStationTarget << m_sToSystemTarget << m_sToStationTarget << m_sFromNameActionCargo << m_sToNameActionCargo << m_sRefuel << m_sTypeCargoUpload << m_sCountCargo;

//    state_delivery = STATE_TO_STATION;

    queue.clear();
    if(!started) {
        if(ui->radioButton_2->isChecked()) {
            state_delivery = STATE_FROM_STATION;
        } else if(ui->radioButton_7->isChecked()) {
            state_delivery = STATE_TO_STATION;
        }

        if(ui->radioButton_6->isChecked()) {
            setCursorSystem();
        } else if(ui->radioButton->isChecked()) {
            setCursorStation();
        } else if(ui->radioButton_4->isChecked()) {
            flytoStation();
        } else if(ui->radioButton_5->isChecked()) {
            dockAutoStart();
        } else if(ui->radioButton_3->isChecked()) {

            flyToSystem();
        }
        started = true;
    } else {
        setCursorSystem();
    }

    qDebug() << "m_jObject " << m_jObject;
}

void ControlPanel::slotReceivReturnCommand(QStringList aList)
{
    if(!aList.isEmpty()) {
        if(ui->checkBox_3->checkState() && ui->checkBox_2->checkState()) {
            if(aList[1].toInt() == 1) {



                qDebug() << "";
                qDebug() << "Control panel success";
                qDebug() << "send first part queue";
                qDebug() << "";



                flyToSystem();
                emit signalSetQueue(queue);
            } else {
                qDebug() << "Control panel not recognize return value";
            }
        } else {
            queue.clear();
            qDebug() << "receiv return command " << aList;
            if(aList[1] == "1") {                                                   // Не выполнилась комманда
                if(aList[0] == "AIMPFLYAROUND") {
                    if(aList[2] == "compass not found") {
                        qDebug() << "Пропал компас - прилетели на пункт назначения / помехи в данных";
                        qDebug() << "Текущее решение, устанавливаем новый пункт назначения";
                        checkCurSystem();
                        emit signalSetQueue(queue);
                    } else if(aList[2] == "") {
                        qDebug() << "Не обработыннй выход из процедуры" << aList[0];
                        checkCurSystem();
                    }
                } else if(aList[0] == "IMAGEEXPECTEDCLOSE") {
                    if(aList[2] == "warning triangle") {
                        qDebug() << "Вышли из режима автопилота, появилось предупреждение";
                        flyAround();
                        emit signalSetQueue(queue);
                    } else if(aList[2] == "detected intercept") {                                                                           // Доделать обнаруженный перехват
                        qDebug() << "Обнаружен перехват";
                        transitToHyper();
                        emit signalSetQueue(queue);
                    }
                } else if(aList[0] == "MAPSYSTEM") {
                    if(aList[2] == "no cursor set system") {
                        qDebug() << "Скрипт установки курсора сброшен, повторяем попытку";
                        setCursorSystem();
                        emit signalSetQueue(queue);
                    }
                } else {
                    qDebug() << "slotReceivReturnCommand" << " if(aList[1] == 1) не обработанные состояния";
                }
            } else if(aList[1] == "0") {                                            // Выполнилась комманда
                if(aList[0] == "MARKER") {
                    if(aList[2] == "setCursorSystem") {
                        dockAutoStart();
                    } else if(aList[2] == "flyToSystem") {
                        flyToSystem();
                    } else if(aList[2] == "setCursorStation") {
                        flytoStation();
                    } else if(aList[2] == "flytoStation") {
                        waitFlyToStation();
                    } else if(aList[2] == "waitFlyToStation") {
                        landing();
                    } else if(aList[2] == "transitToHyper") {
                        flytoStation();
                    } else if(aList[2] == "flyAround") {
                        waitFlyToStation();
                    } else if(aList[2] == "landing") {
                        if(state_delivery == STATE_FROM_STATION) {
                            state_delivery = STATE_TO_STATION;
                            setCursorSystem();
                        } else if(state_delivery == STATE_TO_STATION) {
                            state_delivery = STATE_FROM_STATION;
                            prepScript();
                        }
                    } else if(aList[2] == "dockAutoStart") {
                        flyToSystem();
                    }
                    emit signalSetQueue(queue);
                } else {
                    qDebug() << "slotReceivReturnCommand" << " if(aList[0] == MARKER) { не обработанные состояния";
                }

            } else if(aList[1] == "2") {                                            // Возврат результата
                if(aList[0] == "MARKER") {
                    if(aList[2] == "checkCurSystem") {

//                        int ret = comparisonStr(m_sSystemTarget, aList[3]);
//                        if(ret <= 2) {
                        int ret = comparisonStr("found", aList[3]);
                        if(ret <= 2) {
                            qDebug() << "Проверка на место прибытия успешна";
//                            QMultiMap<QString, QString>::iterator it = mMapSystems.find(m_sSystemTarget);
//                            while(it != mMapSystems.end() && it.key() == m_sSystemTarget) {
//                                m_sStationTarget = it.value();
//                                ++it;
//                            }
                            setCursorStation();
                            emit signalSetQueue(queue);
                        } else {
                            qDebug() << "Не обработанное исключение, компаса нет, но в систему не прибыли" ;
                        }

                    }
                }
            } else {
                qDebug() << "slotReceivReturnCommand" << " } else if(aList[1] == 0) { не обработанные состояния";
            }


        }
    }

}

void ControlPanel::initKeyNames()
{
    m_sFromSystemTarget = m_jObject["fromSystem"].toString();
//    m_sFromSystemTarget = m_sFromSystemTarget.simplified();
//    m_sFromSystemTarget = m_sFromSystemTarget.toLower();
//    deleteCharExtra(m_sFromSystemTarget);
    m_sFromStationTarget = m_jObject["fromStation"].toString();
//    m_sFromStationTarget = m_sFromStationTarget.simplified();
//    m_sFromStationTarget = m_sFromStationTarget.toLower();
//    deleteCharExtra(m_sFromStationTarget);

    m_sToSystemTarget = m_jObject["toSystem"].toString();
//    m_sToSystemTarget = m_sToSystemTarget.simplified();
//    m_sToSystemTarget = m_sToSystemTarget.toLower();
//    deleteCharExtra(m_sToSystemTarget);
    m_sToStationTarget = m_jObject["toStation"].toString();
//    m_sToStationTarget = m_sToStationTarget.simplified();
//    m_sToStationTarget = m_sToStationTarget.toLower();
//    deleteCharExtra(m_sToStationTarget);

    m_sFromNameActionCargo = m_jObject["fromNameActionCargo"].toString();
//    m_sFromNameActionCargo = m_sFromNameActionCargo.simplified();
//    m_sFromNameActionCargo = m_sFromNameActionCargo.toLower();
//    deleteCharExtra(m_sFromNameActionCargo);
    m_sToNameActionCargo = m_jObject["toNameActionCargo"].toString();
//    m_sToNameActionCargo = m_sToNameActionCargo.simplified();
//    m_sToNameActionCargo = m_sToNameActionCargo.toLower();
//    deleteCharExtra(m_sToNameActionCargo);

    m_sRefuel = QString::number(m_jObject["refuel"].toInt());
    m_sTypeCargoUpload = QString::number(m_jObject["typeCargoLoad"].toInt());
    m_sCountCargo = QString::number(m_jObject["countCargo"].toInt());

}

void ControlPanel::deleteConfig(int anIndex)
{
    delete listFlyAction[anIndex];
    listFlyAction.removeAt(anIndex);
    for(int i = 0; i < listFlyAction.size(); i++) {
        listFlyAction[i]->setIndex(i);
    }
}

void ControlPanel::addConfig()
{
    int index = listFlyAction.size();
    FlyAction *flyAction = new FlyAction(index);
    listFlyAction.append(flyAction);
    flyAction->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    vblayout->addWidget(flyAction);

    connect(flyAction, &FlyAction::deleteFlyAction, this, &ControlPanel::deleteConfig);
}

void ControlPanel::loadData()
{
    QString path = PATH_SAVE;
    path.append("\\configflights.json");
    QFile file(path);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "save file not opening";
    } else {
        QJsonDocument jDoc = QJsonDocument::fromJson(file.readAll());
        QJsonArray jArr = jDoc.array();
        file.close();
        QMap<int , QJsonObject> configFlightMap;
        for(int i = 0; i < jArr.size(); i++) {
            QJsonObject jObj = jArr[i].toObject();
            configFlightMap.insert(jObj["index"].toInt(), jObj);
        }
        listFlyAction.clear();
        for(int i = 0; i < configFlightMap.size(); i++) {
//            qDebug() << configFlightMap[i];
            int index = i;
            FlyAction *flyAction = new FlyAction(index);
            listFlyAction.append(flyAction);
            flyAction->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
            vblayout->addWidget(flyAction);
            flyAction->setJsonObj(configFlightMap[i]);
            connect(flyAction, &FlyAction::deleteFlyAction, this, &ControlPanel::deleteConfig);

        }
    }
}

void ControlPanel::saveData()
{
    QJsonArray jArr;
    for(int i = 0; i < listFlyAction.size(); i++) {
        jArr.append(listFlyAction[i]->getJsonObj());
    }
    QString path = PATH_SAVE;
    path.append("\\configflights.json");
    QFile file(path);
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "write file not opening";
    } else {
        QJsonDocument jDoc(jArr);
        file.write(jDoc.toJson());
        file.close();
    }
}

QJsonObject ControlPanel::takeJsonObj()
{

    if(listFlyAction.isEmpty()) {
        qDebug() << "Нет заданы цели";
        return QJsonObject();
    }
    QJsonObject jObj = listFlyAction[0]->getJsonObj();
    QJsonObject jSubObj = jObj["object"].toObject();
    QJsonObject jRetObj = jSubObj;
    if(jSubObj["countFlights"].toInt() == 1) {
        delete listFlyAction[0];
        listFlyAction.removeAt(0);
        for(int i = 0; i < listFlyAction.size(); i++) {
            listFlyAction[i]->setIndex(i);
        }
    } else {
        qDebug() << jSubObj;
        jSubObj["countFlights"] = jSubObj["countFlights"].toInt() - 1;
        jObj["object"] = jSubObj;
        listFlyAction[0]->setJsonObj(jObj);
    }
    saveData();
    return jRetObj;
}

void ControlPanel::on_checkBox_2_clicked()          // Пуск
{
//    setTrack();
//    if(ui->radioButton->isChecked())
//        m_sSystemTarget = ui->radioButton->text();
//    else
//        m_sSystemTarget = ui->radioButton_2->text();
//    m_sStationTarget = mMapSystems.first();
//    if(ui->radioButton_5->isChecked()) {
//        dockAutoStart();
//    } else {
//        if(ui->checkBox_4->isChecked()) {
//            checkCurSystem();
//        } else {
//            flyToSystem();
//        }
//    }

//    flytoStation();
//    test();
    QThread::msleep(1000);
    started = false;
    prepScript();
    queue.prepend(QStringList {"RESTORGAME",                "0" });

    emit signalSetQueue(queue);
}


void ControlPanel::on_radioButton_3_clicked()
{
    emit signalSetSide(0);
}

void ControlPanel::on_pushButton_clicked()          //  ТЕСТ
{
    queue.clear();
//    m_sStationTarget = m_sSystemTarget;
    bTest = !bTest;
//    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", QString::number(bTest)});
//    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "0" });

//    queue.enqueue(QStringList {"PANEL1CASEHEADER",          "0", "навигация"});

//    queue.enqueue(QStringList {"PANEL1CASEHEADER",          "0", "навигация"});
//    queue.enqueue(QStringList {"PANEL1CASEMENUNAV",         "0", m_sStationTarget});
//    queue.enqueue(QStringList {"PANEL1SUBNAV",              "0", "enable_hypermode"});
//    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "13000"});
//    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "1"});
//    queue.enqueue(QStringList {"PANEL1CASEHEADER",          "0", "контакты"});
//    queue.enqueue(QStringList {"PANEL1CASEMENUCONT",        "0", m_sStationTarget});
//    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "0"});
//    queue.enqueue(QStringList {"WAITMENUDOCKING",           "0"});
//    queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_service"});
//    queue.enqueue(QStringList {"IMAGEEXPECTED",             "0", "menuServiceExit", "0.9", "4", "12", "13"});
//    queue.enqueue(QStringList {"SERVICEMENU",               "0"});
//    queue.enqueue(QStringList {"MARKER",                    "0", "flytoStation"});

//    checkCurSystem();

//    queue.enqueue(QStringList {"PICKUPSPEED",               "0", "7000"});
//    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "j" });
//    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "12000"});
//    queue.enqueue(QStringList {"MARKER",                    "0", "dockAutoStart"});


//    queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_autostart"});
//    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "5000"});
//    queue.enqueue(QStringList {"IMAGEEXPECTEDCLOSE",        "0", "pic_autoPilot", "9000", "0.83", "19", "123", "143"});

//    queue.enqueue(QStringList {"IMAGEEXPECTEDCLOSE",        "0", "pic_warningRadTriangle", "2000", "0.75", "10", "34", "35"});
//    queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_service"});

//    QJsonObject jObj;
//    if(ui->checkBox_5->isChecked())
//        jObj["type"] = "разгрузка";
//    queue.enqueue(QStringList {"ACTIONDELIVERYPAPER",       "0", ui->comboBox_3->currentText(), "8", jObj["type"].toString() });      // strengthen_sys   prep_sys_for_power   prep_sys    strengthen_for_power

//    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "mouse_move", QString::number(g_screen.width() / 2), QString::number(g_screen.height() / 2) }); // Поставить в центр
//    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "m" });
//    queue.enqueue(QStringList {"CHECKCURSYSTEM",            "0", "response"});

//    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "mouse_move", QString::number(150), QString::number(500) }); // Поставить в центр
//    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "," });
//    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "1000"});
//    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "," });
//    queue.enqueue(QStringList {"GETSTRSTATICFIELD",         "0", "pic_fieldSystemName"});
//    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "," });
//    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "2000"});
//    checkCurSystem();
//    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "mouse_move", QString::number(g_screen.width() / 2), QString::number(g_screen.height() / 2) }); // Поставить в центр


//    if(sTest == "harma") {
//        sTest = "hip 112400";
//    } else {
//        sTest = "harma";
//    }
//    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "m" });
//    queue.enqueue(QStringList {"MAPSYSTEM",                 "0", sTest});


    m_jObject = takeJsonObj();
    initKeyNames();

    qDebug() << m_sFromSystemTarget << m_sFromStationTarget << m_sToSystemTarget << m_sToStationTarget << m_sFromNameActionCargo << m_sToNameActionCargo << m_sRefuel << m_sTypeCargoUpload << m_sCountCargo;


    queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_service"});
    QString typeCargoLoad;
    if(ui->radioButton_2->isChecked()) {
        queue.enqueue(QStringList {"ACTIONDELIVERYPAPER",       "0", m_sFromNameActionCargo, m_sCountCargo, "загрузка", m_sTypeCargoUpload});
    } else if(ui->radioButton_7->isChecked()) {
        queue.enqueue(QStringList {"ACTIONDELIVERYPAPER",       "0", m_sToNameActionCargo, m_sCountCargo, "разгрузка", m_sTypeCargoUpload});
    }

//    queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_autostart"});


    queue.prepend(QStringList {"RESTORGAME",                "0" });
    emit signalSetQueue(queue);
}

//void ControlPanel::on_pushButton_7_clicked()
//{
//    saveData();
//}

//void ControlPanel::on_pushButton_8_clicked()
//{
//    qDebug() << qPrintable(QByteArray(QJsonDocument(takeJsonObj()).toJson()));
//}

void ControlPanel::on_pushButton_2_clicked()
{
    saveData();
}

void ControlPanel::on_pushButton_3_clicked()
{

    QMessageBox msgBox;
    msgBox.setText("Загрузка систем и станций");
    msgBox.setInformativeText("Процесс загрузки может занять несколько минут, согласны?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok) {
        CreateMapSystem *wid;
        wid = new CreateMapSystem;
        wid->setModal(true);

        wid->exec();
    } else if( ret == QMessageBox::Cancel) {
        qDebug() << "cancle";
    }





}
