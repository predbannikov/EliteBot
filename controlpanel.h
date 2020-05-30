#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QQueue>
#include <QPushButton>
#include <QFile>
#include <QScrollArea>
#include <QComboBox>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QDebug>
#include <QSpinBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QGroupBox>
#include <QMultiMap>
#include <QLabel>
#include "enginescript.h"
#include "global.h"

namespace Ui {
class ControlPanel;
}

static QMultiMap <QString, QString> mapSystemsAndStationLoad;

class FlyAction;
class ConfDelivery;
class FlyToTarget;

class ControlPanel : public QMainWindow
{
    Q_OBJECT
    EngineScript *m_engine;
    QQueue<QStringList> queue;
    QString     m_sSystemTarget;
    QString     m_sStationTarget;
    QString     m_actionRefuel;
    QString     m_actionDelivery;
    QJsonObject m_jObject;
//    QStringList slistSystems;
    bool started = false;
    QMultiMap<QString, QString>   mMapSystems;
    QString sTest;
    bool bTest = false;
public:
    explicit ControlPanel(EngineScript *aEngine, QWidget *parent = nullptr);
    ~ControlPanel();

    void setCursorStation();
    void setCursorSystem();


    void test();
    void transitToHyper();
    void dockAutoStart();
    void flyToSystem();
    void flytoStation();
    void flyAround();
    void waitFlyToStation();
    void landing();
    void setTrack();
    void init();
    void checkCurSystem();
    void performList();
    void prepScript();
    QJsonObject textOutInfo();
    void setGroupBox(const QJsonObject &ajObject);
public slots:
    void slotReceivReturnCommand(QStringList aList);
    void slotCmbSystem(const QString &str);
private slots:

    void deleteConfig(int anIndex);
    void addConfig();

    void on_checkBox_2_clicked();



//    void on_radioButton_4_clicked();

//    void on_radioButton_6_clicked();

//    void on_radioButton_7_clicked();

    void on_radioButton_3_clicked();

    void on_pushButton_clicked();


    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_checkBox_clicked(bool checked);

    void on_pushButton_2_clicked();

//    void on_listWidget_currentRowChanged(int currentRow);

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_checkBox_2_clicked(bool checked);

    void on_checkBox_4_clicked();


    void on_pushButton_5_clicked();

    void on_comboBox_3_currentIndexChanged(const QString &arg1);

    void on_checkBox_5_clicked();

private:
    Ui::ControlPanel *ui;
    QVBoxLayout *vblayout;
    QList<FlyAction *> listFlyAction;
    void loadJson();

signals:
    void signalSetSide(int aSide);
    void signalSetQueue(QQueue<QStringList> aQueue);
    void signalFreeze();
    void signalResizeImage();
    void signalEngineEnable(bool aState);
    void signalEngineSetCurStation(QString asStation);
    void signalEngineEnableTest();
    void closeWindow();

};






class FlyToTarget : public QWidget{
    Q_OBJECT
public:
    FlyToTarget(QWidget *parent = nullptr): QWidget(parent)
    {
        this->setObjectName("FlyToTarget");

        QHBoxLayout *hbl = new QHBoxLayout;
        setLayout(hbl);


        QStringList listSystems = mapSystemsAndStationLoad.keys();
        listSystems.removeDuplicates();
        cmbTargetSystem = new QComboBox;
        cmbTargetSystem->addItems(listSystems);
        hbl->addWidget(cmbTargetSystem);

        QStringList listStation = mapSystemsAndStationLoad.values(cmbTargetSystem->currentText());
        cmbTargetStation = new QComboBox;
        cmbTargetStation->addItems(listStation);
        hbl->addWidget(cmbTargetStation);

        connect(cmbTargetSystem, qOverload<const QString &>(&QComboBox::activated), [=] (const QString asName) {
            cmbTargetStation->clear();
            QStringList list = mapSystemsAndStationLoad.values(asName);
            cmbTargetStation->addItems(list);
        });

    }
    QJsonObject getJsonObj() {

    }
    QComboBox *cmbTargetSystem;
    QComboBox *cmbTargetStation;
};

class ConfDelivery : public QWidget
{
    Q_OBJECT
public:
    ConfDelivery(QWidget *parent = nullptr): QWidget(parent) {
        this->setObjectName("ConfDelivery");

        QHBoxLayout *hbl = new QHBoxLayout;
        setLayout(hbl);



        hbl->addWidget(new QLabel("from"));
        QStringList listSystems = mapSystemsAndStationLoad.keys();
        listSystems.removeDuplicates();
        cmbFromTargetSystem = new QComboBox;
        cmbFromTargetSystem->addItems(listSystems);
        hbl->addWidget(cmbFromTargetSystem);

        QStringList listStation = mapSystemsAndStationLoad.values(cmbFromTargetSystem->currentText());
        cmbFromTargetStation = new QComboBox;
        cmbFromTargetStation->addItems(listStation);
        hbl->addWidget(cmbFromTargetStation);

        connect(cmbFromTargetSystem, qOverload<const QString &>(&QComboBox::activated), [=] (const QString asName) {
            cmbFromTargetStation->clear();
            QStringList list = mapSystemsAndStationLoad.values(asName);
            cmbFromTargetStation->addItems(list);
        });




        groupRadioBtSide = new QGroupBox;
        radioTo = new QRadioButton;
        radioFrom = new QRadioButton;
        QHBoxLayout *hblRadio = new QHBoxLayout;
        groupRadioBtSide->setLayout(hblRadio);
        groupRadioBtSide->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        hblRadio->addWidget(radioFrom);
        hblRadio->addWidget(new QLabel("погрузка"));
        hblRadio->addWidget(radioTo);
        hbl->addWidget(groupRadioBtSide);





        hbl->addWidget(new QLabel("to"));
        cmbToTargetSystem = new QComboBox;
        cmbToTargetSystem->addItems(listSystems);
        hbl->addWidget(cmbToTargetSystem);

        cmbToTargetStation = new QComboBox;
        QStringList listStation2 = mapSystemsAndStationLoad.values(cmbToTargetSystem->currentText());
        listStation2.removeDuplicates();
        cmbToTargetStation->addItems(listStation2);
        hbl->addWidget(cmbToTargetStation);

        connect(cmbToTargetSystem, qOverload<const QString &>(&QComboBox::activated), [=] (const QString asName) {
            cmbToTargetStation->clear();
            QStringList list = mapSystemsAndStationLoad.values(asName);
            cmbToTargetStation->addItems(list);
        });


        hbl->addWidget(new QLabel("\tcount"));
        spinBoxCount = new QSpinBox;
        spinBoxCount->setMaximumWidth(50);
        hbl->addWidget(spinBoxCount);

        hbl->addWidget(new QLabel("\tfuel"));
        chboxRefuel = new QCheckBox;
        hbl->addWidget(chboxRefuel);


    }
    QJsonObject getJsonObj() {
        QJsonObject jObj;


        return jObj;
    }
    QComboBox *cmbFromTargetSystem;
    QComboBox *cmbFromTargetStation;
    QComboBox *cmbToTargetSystem;
    QComboBox *cmbToTargetStation;
    QSpinBox *spinBoxCount;
    QCheckBox *chboxRefuel;

    QGroupBox    *groupRadioBtSide;
    QRadioButton *radioFrom;
    QRadioButton *radioTo;
};


class FlyAction : public QWidget
{
    Q_OBJECT
public:
    FlyAction(int anIndex = 0, QWidget *parent = nullptr): QWidget(parent)
    {
        QGridLayout *hbl = new QGridLayout;
        setLayout(hbl);

        index = anIndex;
        indexLabel = new QLabel(QString::number(anIndex));
        indexLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        hbl->addWidget(indexLabel, 0, 1);

        QStringList action1 = {"delivery", "fly"};
        cmbAction = new QComboBox;
        cmbAction->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        cmbAction->addItems(action1);
        hbl->addWidget(cmbAction, 0, 2);
        hbl->addWidget(new ConfDelivery, 0, 3);

        connect(cmbAction, qOverload<const QString &>(&QComboBox::activated), [=] (const QString asName) {
            QWidget *wid;
            QList<QWidget*> myWidget = this->findChildren<QWidget*>();
            QListIterator<QWidget*> it(myWidget);
            while (it.hasNext()) {
                wid = it.next(); // take each widget in the list
//                    qDebug() << wid->objectName();
                if(FlyToTarget *flyToTarget = qobject_cast<FlyToTarget*>(wid)) {  // check if iterated widget is of type QLineEdit
                    //                            qDebug() << "test hit the target";
                    flyToTarget->deleteLater();
                    delete flyToTarget;
                    break;
                }
                if(ConfDelivery *confDelivery = qobject_cast<ConfDelivery*>(wid)) {  // check if iterated widget is of type QLineEdit
                    //                            qDebug() << "test hit the target";
                    confDelivery->deleteLater();
                    delete confDelivery;
                    break;
                }
            }
            if(asName == "fly") {
                hbl->addWidget(new FlyToTarget);

            } else if(asName == "delivery") {
                hbl->addWidget(new ConfDelivery);

            }
        });


        QPushButton *pbtnDelete = new QPushButton("x");
        connect(pbtnDelete, &QPushButton::clicked, [this] () {
            emit deleteFlyAction(index);
//            this->deleteLater();
        });
        pbtnDelete->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        pbtnDelete->setMaximumWidth(21);
        hbl->addWidget(pbtnDelete, 0, 4);
    }
    void setIndex(int anIndex) {
        index = 0;
        indexLabel->setText(QString::number(anIndex));
    }
    int getIndex() {
        return index;
    }
    QJsonObject getJsonObj() {
        QJsonObject jObj;
        jObj["index"] = index;
        QWidget *wid;
        QList<QWidget*> myWidget = this->findChildren<QWidget*>();
        QListIterator<QWidget*> it(myWidget);
        while (it.hasNext()) {
            wid = it.next(); // take each widget in the list
//                    qDebug() << wid->objectName();
            if(FlyToTarget *flyToTarget = qobject_cast<FlyToTarget*>(wid)) {  // check if iterated widget is of type QLineEdit
                jObj["action"] = "FlyToTarget";
                jObj["object"] = flyToTarget->getJsonObj();
                break;
            }
            if(ConfDelivery *confDelivery = qobject_cast<ConfDelivery*>(wid)) {  // check if iterated widget is of type QLineEdit
                jObj["action"] = "ConfDelivery";
                jObj["object"] = confDelivery->getJsonObj();
                break;
            }
        }
        return jObj;
    }
    QLabel *indexLabel;
    QComboBox *cmbAction;
    int index = 0;

signals:
    void deleteFlyAction(int anIndex);
};






#endif // CONTROLPANEL_H
