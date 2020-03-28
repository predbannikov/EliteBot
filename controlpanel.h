#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QMainWindow>
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
    QString     m_sTarget;
    QStringList slistSystems;
public:
    explicit ControlPanel(EngineScript *aEngine, QWidget *parent = nullptr);
    ~ControlPanel();

    void test();
    void ring();
    void setTrack();
    void init();

public slots:
    void slotReceivReturnCommand(QStringList aList);
private slots:



    void on_checkBox_2_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_checkBox_clicked();

    void on_comboBox_activated(const QString &arg1);

    void on_radioButton_4_clicked();

    void on_radioButton_6_clicked();

    void on_radioButton_7_clicked();

    void on_radioButton_3_clicked();

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
