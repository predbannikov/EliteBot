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

    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(const QString&)),
            this,SLOT(slotCmbSystem(const QString&)));

    init();

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
//    this->layout()->addWidget(mainWidgte);
//    setCentralWidget(mainWidgte);

//    setCentralWidget(scrollArea);
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
    queue.enqueue(QStringList {"PANEL1CASEMENUNAV",         "0", m_sStationTarget, m_sSystemTarget});
    queue.enqueue(QStringList {"PANEL1SUBNAV",              "0", "fix_target"});
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "0"});      // выключить панель
    queue.enqueue(QStringList {"MARKER",                    "0", "setCursorStation"});
}

void ControlPanel::setCursorSystem()
{
    qDebug() << "setCursorSystem";
    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "m" });
    queue.enqueue(QStringList {"MAPSYSTEM",                 "0", m_sSystemTarget});
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
    queue.enqueue(QStringList {"IMAGEEXPECTEDCLOSE",        "0", "pic_autoPilot", "10000", "0.80", "17", "92", "128"});
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
    queue.enqueue(QStringList {"IMAGEEXPECTED",             "0", "pic_hypermodResetPull", "0.93", "9", "6", "26"});
    queue.enqueue(QStringList {"MARKER",                    "0", "flyToSystem"});
}

void ControlPanel::flytoStation()
{
    qDebug() << "flytoStation";



    queue.enqueue(QStringList {"AIMPFLYAROUND",             "0", "station"});
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "1"});
    queue.enqueue(QStringList {"PANEL1CASEHEADER",          "0", "навигация"});
    queue.enqueue(QStringList {"PANEL1CASEMENUNAV",         "0", m_sStationTarget, m_sSystemTarget});
    queue.enqueue(QStringList {"PANEL1SUBNAV",              "0", "enable_hypermode_helper"});
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "0"});
    queue.enqueue(QStringList {"MARKER",                    "0", "flytoStation"});
}

void ControlPanel::waitFlyToStation()
{
    qDebug() << "waitFlyToStation";
    queue.enqueue(QStringList {"IMAGEEXPECTEDCLOSE",        "0", "pic_autoPilot", "10000", "0.80", "11", "38", "49"});
    queue.enqueue(QStringList {"IMAGEEXPECTEDCLOSE",        "0", "pic_warningRadTriangle", "2000", "0.75", "10", "34", "35"});
    queue.enqueue(QStringList {"MARKER",                    "0", "waitFlyToStation"});
}

void ControlPanel::landing()
{
    qDebug() << "landing";
    queue.enqueue(QStringList {"ACTIONGETCLOSER",           "0"});
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "1"});
    queue.enqueue(QStringList {"PANEL1CASEHEADER",          "0", "контакты"});
    queue.enqueue(QStringList {"PANEL1CASEMENUCONT",        "0", m_sStationTarget});
    queue.enqueue(QStringList {"PANEL1ENABLE",              "0", "0"});
    queue.enqueue(QStringList {"WAITMENUDOCKING",           "0"});

    if(m_jObject["actionRefuel"].toString() == "заправиться") {
        queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_service"});
        queue.enqueue(QStringList {"IMAGEEXPECTED",             "0", "menuServiceExit", "0.9", "4", "12", "13"});
        queue.enqueue(QStringList {"ACTIONWAIT",                "0", "1000"});
        queue.enqueue(QStringList {"SERVICEMENU",               "0"});
    }
    if(m_jObject["actionDeliver"].toString() == "доставка") {
        queue.enqueue(QStringList {"DOCKINGMENUCASE",           "0", "menu_docking_service"});
        queue.enqueue(QStringList {"IMAGEEXPECTED",             "0", "menuServiceExit", "0.9", "4", "12", "13"});

        queue.enqueue(QStringList {"ACTIONDELIVERYPAPER",       "0", m_jObject["typeAction"].toString(), "4", m_jObject["typeDeliver"].toString()});
//        queue.enqueue(QStringList {"ACTIONDELIVERYPAPER",       "0", "LOADING", "4", "case"});
    }
    queue.enqueue(QStringList {"MARKER",                    "0", "landing"});

}

void ControlPanel::flyAround()
{
    qDebug() << "flyAround";
    queue.enqueue(QStringList {"AIMPFLYAROUND",             "0", "station"});
    queue.enqueue(QStringList {"MARKER",                    "0", "flyAround"});
}

void ControlPanel::setTrack()
{
    qDebug() << "создаём новый скрипт setTrack";
//    QMultiMap<QString, QString>::iterator it = mMapSystems.begin();
//    while(it != mMapSystems.end()) {
//        if(it.key() == m_sSystemTarget) {
//            if(++it != mMapSystems.end()) {
//                if(m_sSystemTarget != it.key())
//                    m_sSystemTarget = it.key();
//                else
//                    continue;
//            } else {
//                it = mMapSystems.begin();
//                m_sSystemTarget = it.key();
//            }
//            break;
//        }
//        ++it;
//    }

    queue.clear();
    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "m" });
    queue.enqueue(QStringList {"MAPSYSTEM",                 "0", m_sSystemTarget});
    queue.enqueue(QStringList {"MARKER",                    "0", "setTrack"});


}

void ControlPanel::init()
{
    mMapSystems = mapSystemsAndStationLoad;
//    mMapSystems.insert("Harma", "Gabriel Enterprise");
//    mMapSystems.insert("Harma", "Celebi city");
//    mMapSystems.insert("HIP 112400", "Bluford orbital");
//    mMapSystems.insert("HIP 112400", "Springer Colony");
//    mMapSystems.insert("Kakmbutan", "Macgregor Orbital");
//    mMapSystems.insert("Huichi", "Collins Station");
//    mMapSystems.insert("Clayakarma", "Duke City");
//    mMapSystems.insert("Clayakarma", "Sinclair Port");
//    mMapSystems.insert("Xi Ophiuchi", "Khan Dock");
//    mMapSystems.insert("Katuri", "Bogdanov City");
    QStringList systems = mMapSystems.keys();
    systems.removeDuplicates();
    ui->comboBox_2->clear();
    ui->comboBox_2->addItems(systems);

//    ui->comboBox_2->setCurrentIndex(1);
        // Bluford orbital
//    slistSystems << "HIP 112400" << "Harma";
//    ui->comboBox_2->addItems(slistSystems);
}

void ControlPanel::checkCurSystem()
{
    queue.clear();
//    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "mouse_move", QString::number(g_screen.width() / 2), QString::number(g_screen.height() / 2) }); // Поставить в центр
    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0", "m" });
//    queue.enqueue(QStringList {"GETSTRSTATICFIELD",         "0", "pic_fieldSystemName"});
//    queue.enqueue(QStringList {"CHECKCURSYSTEM",            "0", "response"});
    queue.enqueue(QStringList {"CHECKCURSYSTEM",            "0", m_sSystemTarget});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "m" });
    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "2000"});
    queue.enqueue(QStringList {"MARKER",                    "0", "checkCurSystem"});
}

void ControlPanel::prepScript()
{
    qDebug() << "Чтение очереди ";
    if(ui->listWidget->count() == 0) {
        qDebug() << "Очередь пуста";
        ui->checkBox_2->setChecked(false);
        started = false;
        return;
    }
    QListWidgetItem *item = ui->listWidget->takeItem(0);
    QByteArray array = item->text().toUtf8();
    QJsonDocument jDoc = QJsonDocument::fromJson(array);

    m_jObject = jDoc.object();
    qDebug() << m_jObject;
    m_sSystemTarget = m_jObject["system"].toString();
    m_sStationTarget = m_jObject["station"].toString();


    queue.clear();
    if(!started) {
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
//    emit signalSetQueue(queue);
//    flyToSystem();
//    flytoStation();


//    static enum {TRANS_1, TRANS_2, TRANS_3} trans;
//    switch (trans) {
//    case TRANS_1:

//    break;

//    }
    qDebug() << "m_jObject " << m_jObject;
}

QJsonObject ControlPanel::textOutInfo()
{
    QJsonObject jObj;
    jObj["system"] = ui->comboBox_2->currentText();
    jObj["station"] = ui->comboBox->currentText();
    jObj["actionRefuel"] = ui->checkBox->isChecked() ? "заправиться": "ничего не делать";
    jObj["actionDeliver"] = ui->checkBox_4->isChecked() ? "доставка": "без доставки";
    if(jObj["actionDeliver"].toString() == "доставка") {
        jObj["typeAction"] = ui->comboBox_3->currentText();
        if(ui->checkBox_5->isChecked())
            jObj["typeDeliver"] = "разгрузка";
    }
//    QJsonArray jArr;
//    QJsonObject jAction;
//    jAction["action"] = ui->checkBox->isChecked() ? "заправиться": "ничего не делать";
//    jArr.append(jAction);
//    jAction["action"] = ui->checkBox_4->isChecked() ? "доставка": "без доставки";
//    jArr.append(jAction);
//    jObj["actions"] = jArr;
    QString str = QString("Цель: \nСистема: %1 \n станция: %2 \n действие: %3 \n действие: %4 \n тип: %5 %6")
            .arg(jObj["system"].toString())
            .arg(jObj["station"].toString())
            .arg(jObj["actionRefuel"].toString())
            .arg(jObj["actionDeliver"].toString())
            .arg(jObj["typeAction"].toString())
            .arg(jObj["typeDeliver"].toString());
//    jArr = jObj["actions"].toArray();
//    for(auto jObj_: jArr) {
//        str.append("\n действие: " + jObj_.toObject()["action"].toString());
//    }
    ui->label->setText(str);
    return jObj;
}

void ControlPanel::setGroupBox(const QJsonObject &ajObject)
{
    ui->comboBox_2->setCurrentText(ajObject["system"].toString());
    ui->comboBox->setCurrentText(ajObject["station"].toString());
    if(ajObject["actionRefuel"].toString() == "заправиться") {
        ui->checkBox->setChecked(true);
    } else {
        ui->checkBox->setChecked(false);
    }
    if(ajObject["actionDeliver"].toString() == "доставка"){
        ui->checkBox_4->setChecked(true);
    } else {
        ui->checkBox_4->setChecked(false);
    }

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
                        prepScript();
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

void ControlPanel::slotCmbSystem(const QString &str)
{
    QStringList listStation;
    QMultiMap<QString, QString>::iterator it = mMapSystems.begin();
    while(it != mMapSystems.end()) {
        if(it.key() == str)
        {
            listStation.append(it.value());
        }
        ++it;
    }
    ui->comboBox->clear();
    ui->comboBox->addItems(listStation);
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

    queue.enqueue(QStringList {"PANEL1CASEHEADER",          "0", "навигация"});

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


    queue.prepend(QStringList {"RESTORGAME",                "0" });
    emit signalSetQueue(queue);
}

void ControlPanel::on_comboBox_currentIndexChanged(const QString &arg1)
{
    textOutInfo();
}

void ControlPanel::on_checkBox_clicked(bool checked)
{
    textOutInfo();
}

void ControlPanel::on_pushButton_2_clicked()
{
    ui->listWidget->addItem(QJsonDocument(textOutInfo()).toJson());
}

//void ControlPanel::on_listWidget_currentRowChanged(int currentRow)
//{
//    if(currentRow < 0)
//        return;
//    QListWidgetItem *item = ui->listWidget->item(currentRow);
//    QByteArray array = item->text().toUtf8();
//}

void ControlPanel::on_pushButton_3_clicked()
{
    if(ui->listWidget->currentRow() < 0)
        return;
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}

void ControlPanel::on_pushButton_4_clicked()
{
    if(ui->listWidget->currentRow() >= 0) {
        QListWidgetItem *item = ui->listWidget->item(ui->listWidget->currentRow());
        item->setText(QJsonDocument(textOutInfo()).toJson());
    }
}

void ControlPanel::on_listWidget_itemClicked(QListWidgetItem *item)
{

    QByteArray array = item->text().toUtf8();
    QJsonDocument jDoc = QJsonDocument::fromJson(array);
    QJsonObject jObj = jDoc.object();
    setGroupBox(jObj);
}

void ControlPanel::on_checkBox_2_clicked(bool checked)
{

}

void ControlPanel::on_checkBox_4_clicked()
{
    textOutInfo();
}

void ControlPanel::on_pushButton_5_clicked()
{
    if(ui->listWidget->count() == 2) {
        if(ui->spinBox->value() >= 2) {
            QListWidgetItem *item1 = ui->listWidget->takeItem(0);
            QListWidgetItem *item2 = ui->listWidget->takeItem(0);
            QByteArray array1 = item1->text().toUtf8();
            QByteArray array2 = item2->text().toUtf8();
            QJsonDocument jDoc1 = QJsonDocument::fromJson(array1);
            QJsonDocument jDoc2 = QJsonDocument::fromJson(array2);
            for(int i = 0; i < ui->spinBox->value(); i++) {
                ui->listWidget->addItem(jDoc1.toJson());
                ui->listWidget->addItem(jDoc2.toJson());
            }
        }
    }
}

void ControlPanel::on_comboBox_3_currentIndexChanged(const QString &arg1)
{
    textOutInfo();
}

void ControlPanel::on_checkBox_5_clicked()
{
    textOutInfo();
}

void ControlPanel::on_pushButton_6_clicked()
{
    if(listFlyAction.size() > ui->spinBox_2->value()) {
        QByteArray str = QJsonDocument(listFlyAction[ui->spinBox_2->value()]->getJsonObj()).toJson();
        qDebug() << qPrintable(str);
    }
}

void ControlPanel::on_pushButton_7_clicked()
{
    saveData();
}
