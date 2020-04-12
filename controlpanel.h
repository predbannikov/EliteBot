#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QQueue>
#include "enginescript.h"
#include "global.h"

namespace Ui {
class ControlPanel;
}

class ControlPanel : public QMainWindow
{
    Q_OBJECT
    EngineScript *m_engine;
    QQueue<QStringList> queue;
    QString     m_sSystemTarget;
    QString     m_sStationTarget;
    QJsonObject m_jObject;
//    QStringList slistSystems;
    bool started = false;
    QMultiMap<QString, QString>   mMapSystems;
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

private:
    Ui::ControlPanel *ui;

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

#endif // CONTROLPANEL_H
