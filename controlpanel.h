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

//    enum CURRENT_MARK {STATE_setCursorSystem, STATE_dockAutoStart, STATE_flyToSystem, STATE_checkCurSystem,
//      STATE_setCursorStation, STATE_flytoStation, STATE_waitFlyToStation, STATE_landing, STATE_prepScript };
    enum STATE_DELIVERY {STATE_FROM_STATION, STATE_TO_STATION} state_delivery;
    EngineScript *m_engine;
    QQueue<QStringList> queue;
//    QString     m_sSystemTarget;
//    QString     m_sStationTarget;
    QString     m_sToSystemTarget;
    QString     m_sToStationTarget;
    QString     m_sFromSystemTarget;
    QString     m_sFromStationTarget;
    QString     m_sRefuel;
    QString     m_sTypeCargoUpload;
    QString     m_sCountCargo;
    QString     m_sFromNameActionCargo;
    QString     m_sToNameActionCargo;
//    QString     m_action
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
    void initKeyNames();
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
    void loadData();
    void saveData();
    QJsonObject takeJsonObj();

    void on_checkBox_2_clicked();

    void on_radioButton_3_clicked();

    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_checkBox_clicked(bool checked);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_checkBox_2_clicked(bool checked);

    void on_checkBox_4_clicked();

    void on_pushButton_5_clicked();

    void on_comboBox_3_currentIndexChanged(const QString &arg1);

    void on_checkBox_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

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
        return QJsonObject();
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
        sNamesActionCargo << "Усилить систему" << "Укрепить для державы" << "Подготовить системы для державы" << "Подготовить систему";


        QHBoxLayout *hbl = new QHBoxLayout;
        setLayout(hbl);



        QLabel *lblFrom = new QLabel("from");
        lblFrom->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        hbl->addWidget(lblFrom);
        QStringList listSystems = mapSystemsAndStationLoad.keys();
        listSystems.removeDuplicates();
        cmbFromTargetSystem = new QComboBox;
        cmbFromTargetSystem->addItems(listSystems);
        hbl->addWidget(cmbFromTargetSystem);

        QStringList listStation = mapSystemsAndStationLoad.values(cmbFromTargetSystem->currentText());
        cmbFromTargetStation = new QComboBox;
        cmbFromTargetStation->addItems(listStation);
        hbl->addWidget(cmbFromTargetStation);

        connect(cmbFromTargetSystem, qOverload<const QString &>(&QComboBox::currentTextChanged), [=] (const QString asName) {
            cmbFromTargetStation->clear();
            QStringList list = mapSystemsAndStationLoad.values(asName);
            cmbFromTargetStation->addItems(list);
        });

//****

        cmbFromNameActionCargo = new QComboBox;
        cmbFromNameActionCargo->addItems(sNamesActionCargo);
        cmbFromNameActionCargo->setMaximumWidth(150);
        hbl->addWidget(cmbFromNameActionCargo);





        QLabel *lblTo = new QLabel("to");
        lblTo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        hbl->addWidget(lblTo);

        cmbToTargetSystem = new QComboBox;
        cmbToTargetSystem->addItems(listSystems);
        hbl->addWidget(cmbToTargetSystem);

        cmbToTargetStation = new QComboBox;
        QStringList listStation2 = mapSystemsAndStationLoad.values(cmbToTargetSystem->currentText());
        listStation2.removeDuplicates();
        cmbToTargetStation->addItems(listStation2);
        hbl->addWidget(cmbToTargetStation);

        connect(cmbToTargetSystem, qOverload<const QString &>(&QComboBox::currentTextChanged), [=] (const QString asName) {
            cmbToTargetStation->clear();
            QStringList list = mapSystemsAndStationLoad.values(asName);
            cmbToTargetStation->addItems(list);
        });

        cmbToNameActionCargo = new QComboBox;
        cmbToNameActionCargo->addItems(sNamesActionCargo);
        cmbToNameActionCargo->setMaximumWidth(150);
        hbl->addWidget(cmbToNameActionCargo);

        sboxCountCargo = new QSpinBox;
        sboxCountCargo->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sboxCountCargo->setMaximumWidth(50);
        sboxCountCargo->setMinimum(1);
        hbl->addWidget(sboxCountCargo);





        groupRadioBtSide = new QGroupBox;
        radioType2 = new QRadioButton;
        radioType1 = new QRadioButton;
        radioType1->setChecked(true);
        QHBoxLayout *hblRadio = new QHBoxLayout;
        groupRadioBtSide->setLayout(hblRadio);
        groupRadioBtSide->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        hblRadio->addWidget(new QLabel("тип"));
        hblRadio->addWidget(radioType1);
        hblRadio->addWidget(radioType2);
        hbl->addWidget(groupRadioBtSide);




        QLabel *lblFuel = new QLabel(" fuel");
        lblFuel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        hbl->addWidget(lblFuel);
        chboxRefuel = new QCheckBox;
        chboxRefuel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        hbl->addWidget(chboxRefuel);

        QLabel *lblCount = new QLabel(" count");
        lblCount->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        hbl->addWidget(lblCount);
        spinBoxCountDelivery = new QSpinBox;
        spinBoxCountDelivery->setMinimum(1);
        spinBoxCountDelivery->setMaximumWidth(50);
        spinBoxCountDelivery->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        hbl->addWidget(spinBoxCountDelivery);
    }


    QJsonObject getJsonObj() {
        QJsonObject jObj;
        jObj["fromSystem"] =            cmbFromTargetSystem->currentText();
        jObj["fromStation"] =           cmbFromTargetStation->currentText();
        jObj["countCargo"] =            sboxCountCargo->value();
        jObj["toSystem"] =              cmbToTargetSystem->currentText();
        jObj["toStation"] =             cmbToTargetStation->currentText();
        jObj["fromNameActionCargo"] =   cmbFromNameActionCargo->currentText();
        jObj["toNameActionCargo"] =     cmbToNameActionCargo->currentText();
        if(radioType1->isChecked())
            jObj["typeCargoLoad"] =     0;
        else if(radioType2->isChecked())
            jObj["typeCargoLoad"] =     1;
        jObj["refuel"] =          chboxRefuel->isChecked() ? 1 : 0;
        jObj["countFlights"] =          spinBoxCountDelivery->value();

        return jObj;
    }
    void setJsonObj(QJsonObject ajObj) {
        cmbFromTargetSystem->setCurrentText(ajObj["fromSystem"].toString());
        cmbFromTargetStation->setCurrentText(ajObj["fromStation"].toString());
        sboxCountCargo->setValue(ajObj["countCargo"].toInt());
        cmbToTargetSystem->setCurrentText(ajObj["toSystem"].toString());
        cmbToTargetStation->setCurrentText(ajObj["toStation"].toString());
        cmbFromNameActionCargo->setCurrentText(ajObj["fromNameActionCargo"].toString());
        cmbToNameActionCargo->setCurrentText(ajObj["toNameActionCargo"].toString());

        if(ajObj["typeCargoLoad"].toInt() == 0)
            radioType1->setChecked(true);
        else if(ajObj["typeCargoLoad"].toInt() == 1)
            radioType2->setChecked(true);
        chboxRefuel->setChecked(static_cast<bool>(ajObj["refuel"].toInt()));
        spinBoxCountDelivery->setValue(ajObj["countFlights"].toInt());
    }
    QComboBox *cmbFromTargetSystem;
    QComboBox *cmbFromTargetStation;
    QComboBox *cmbToTargetSystem;
    QComboBox *cmbToTargetStation;
    QComboBox *cmbFromNameActionCargo;
    QComboBox *cmbToNameActionCargo;
    QSpinBox *spinBoxCountDelivery;
    QSpinBox *sboxCountCargo;
    QCheckBox *chboxRefuel;

    QGroupBox    *groupRadioBtSide;
    QRadioButton *radioType1;
    QRadioButton *radioType2;

    QStringList sNamesActionCargo;

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
        cmbAction->setEnabled(false);                                                                                                    // Заблокированный комбобокс
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
        index = anIndex;
        indexLabel->setText(QString::number(anIndex));
    }
    int getIndex() {
        return index;
    }
    void setJsonObj(QJsonObject ajObj) {
        if(ajObj["action"] == "ConfDelivery") {
            QWidget *wid;
            QList<QWidget*> myWidget = this->findChildren<QWidget*>();
            QListIterator<QWidget*> it(myWidget);
            while (it.hasNext()) {
                wid = it.next(); // take each widget in the list
    //                    qDebug() << wid->objectName();

                if(ConfDelivery *confDelivery = qobject_cast<ConfDelivery*>(wid)) {  // check if iterated widget is of type QLineEdit
                    confDelivery->setJsonObj(ajObj["object"].toObject());
                    break;
                }
            }
        } else if(ajObj["action"] == "FlyToTarget") {
                // Доработать
        }
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
