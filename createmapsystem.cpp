#include "createmapsystem.h"

CreateMapSystem::CreateMapSystem(QWidget *parent) : QDialog(parent)
{

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    this->setLayout(vboxLayout);

    readData();
    m_cmbBoxListSystem = new QComboBox;
    m_listSystem = reqMap.keys();
    m_listSystem.removeDuplicates();
    m_cmbBoxListSystem->addItems(m_listSystem);
    connect(m_cmbBoxListSystem, qOverload<const QString &>(&QComboBox::activated), [this] (const QString asName) {
        m_lineEdit->setText(asName);
    });
    vboxLayout->addWidget(m_cmbBoxListSystem);

    m_lineEdit = new QLineEdit;
    vboxLayout->addWidget(m_lineEdit);

    m_lineEdit->setText(m_cmbBoxListSystem->currentText());

    m_buttonRemoveItem = new QPushButton("Удалить систему");
    connect(m_buttonRemoveItem, &QPushButton::clicked, [this] () {
        if(m_listSystem.contains(m_lineEdit->text())) {
            m_listSystem.removeOne(m_lineEdit->text());
            m_cmbBoxListSystem->clear();
            m_cmbBoxListSystem->addItems(m_listSystem);
        } else {
            QMessageBox msgBox;
            msgBox.setText("Система в списке не найдена");
            msgBox.exec();
        }
    });
    vboxLayout->addWidget(m_buttonRemoveItem);


    m_buttonSaveItem = new QPushButton("Сохранить систему");
    connect(m_buttonSaveItem, &QPushButton::clicked, [this] () {
        if(m_listSystem.contains(m_lineEdit->text())) {
            QMessageBox msgBox;
            msgBox.setText("Система уже в списке");
            msgBox.exec();
        } else {
            m_listSystem.append(m_lineEdit->text());
            m_cmbBoxListSystem->clear();
            m_cmbBoxListSystem->addItems(m_listSystem);
        }
    });
    vboxLayout->addWidget(m_buttonSaveItem);

    m_buttonPrepJson = new QPushButton("Создать карту");
    connect(m_buttonPrepJson, &QPushButton::clicked, [this] () {
        writeOut();
    });
    vboxLayout->addWidget(m_buttonPrepJson);


    QDir pathDir;
    pathDir.setPath(pathDir.currentPath() + '/' + "mapSystemData");
    QString pathToMapSystemData = pathDir.path();
    QString fileNameStation;



    fileNameStation = pathToMapSystemData + "/stations.json";
    qDebug() << fileNameStation;
    QFile fileStation(fileNameStation);
    if(fileStation.open(QFile::ReadOnly | QFile::Text)) {
        QByteArray btArray = fileStation.readAll();
//        qDebug() << qPrintable(btArray);
        qDebug() << "загрузка файла 1";
        QJsonParseError jError;
        QJsonDocument jDoc = QJsonDocument::fromJson(btArray, &jError);
        if(jError.error == QJsonParseError::NoError) {
            m_jStationArray1 = jDoc.array();
            qDebug() << "файл 1 распакован" << m_jStationArray1.size();
        } else {
            qDebug() << jError.error << jError.errorString();
        }
        fileStation.close();

    } else {
        qDebug() << "File " << fileNameStation << " not open";
        return;
    }



    fileNameStation = pathToMapSystemData + "/stations2.json";
    qDebug() << fileNameStation;
    QFile fileStation2;
    fileStation2.setFileName(fileNameStation);
    if(fileStation2.open(QFile::ReadOnly | QFile::Text)) {
        QByteArray btArray = fileStation2.readAll();
        //        qDebug() << qPrintable(btArray);
        qDebug() << "загрузка файла 2";
        QJsonParseError jError;
        QJsonDocument jDoc = QJsonDocument::fromJson(btArray, &jError);
        if(jError.error == QJsonParseError::NoError) {
            m_jStationArray2 = jDoc.array();
            qDebug() << "файл 2 распакован" << m_jStationArray2.size();
        } else {
            qDebug() << jError.error << jError.errorString();
        }
        fileStation2.close();

    } else {
        qDebug() << "File " << fileNameStation << " not open";
        return;
    }


    QString fileName;
    fileName = pathToMapSystemData + "/systems_populated.json";
    qDebug() << fileName;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly | QFile::Text)) {
        QByteArray btArray = file.readAll();
        qDebug() << "загрузка файла 3";
//        qDebug() << qPrintable(btArray);
        QJsonDocument jDoc = QJsonDocument::fromJson(btArray);
        m_jSystemArray = jDoc.array();
        qDebug() << "файл 3 распакован" << m_jSystemArray.size();
        file.close();

    } else {
        qDebug() << "File " << fileName << " not open";
        return;
    }

    qDebug() << "end loading";
}

void CreateMapSystem::writeOut()
{

    for(QString sSystem: m_listSystem) {
        QJsonArray jArrOut;
        //        QJsonObject jObjOut;
        for(int i = 0; i < m_jSystemArray.size(); i++) {
            QJsonObject jObj = m_jSystemArray[i].toObject();
            if(jObj["name"].toString() == sSystem) {
                qDebug() << "";
                qDebug() << "found system: " << jObj["name"].toString() << " id:" << jObj["id"].toInt();
                if(!m_jStationArray1.isEmpty()) {
                    int srchId = jObj["id"].toInt();

                    for(int j = 0; j < m_jStationArray1.size(); j++) {
                        QJsonObject jObjSt = m_jStationArray1[j].toObject();
                        if(jObjSt["system_id"].toInt() == srchId) {
                            QJsonObject jFound;
                            jFound["station"] = jObjSt["name"].toString();
                            jFound["id"] = jObjSt["id"].toInt();
                            jFound["system_id"] = jObjSt["system_id"].toInt();
                            jArrOut.append(jFound);
                            qDebug() << "found" << jObjSt["name"].toString() << " id:" << jObjSt["id"].toInt() << " system_id:" << jObjSt["system_id"].toInt();

                        }
                    }

                    for(int j = 0; j < m_jStationArray2.size(); j++) {
                        QJsonObject jObjSt = m_jStationArray2[j].toObject();
                        if(jObjSt["system_id"].toInt() == srchId) {
                            QJsonObject jFound;
                            jFound["station"] = jObjSt["name"].toString();
                            jFound["id"] = jObjSt["id"].toInt();
                            jFound["system_id"] = jObjSt["system_id"].toInt();
                            jArrOut.append(jFound);
                            qDebug() << "found" << jObjSt["name"].toString() << " id:" << jObjSt["id"].toInt() << " system_id:" << jObjSt["system_id"].toInt();

                        }
                    }

                }
            }
        }
        m_jObjOut[sSystem] = jArrOut;
        //        m_jArrOut.append(jObjOut);
    }





    QString path_config = PATH_CONFIG;
    path_config.append("//map.json");
    QFile fileOut(path_config);
    if(fileOut.open(QFile::WriteOnly | QFile::Text)) {
        QJsonDocument jDoc = QJsonDocument(m_jObjOut);
        fileOut.write(jDoc.toJson());
        fileOut.close();
        qDebug() << qPrintable(jDoc.toJson());

    } else {
        qDebug() << "file open for write not open";
    }

}

void CreateMapSystem::readData()
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

                reqMap.insert(sKey, jArr[i].toObject()["station"].toString()) ;
            }
        }
        file.close();
    }
}
