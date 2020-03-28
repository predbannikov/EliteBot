#include "controlpanel.h"
#include "ui_controlpanel.h"

ControlPanel::ControlPanel(EngineScript *aEngine, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ControlPanel)
{
    ui->setupUi(this);
    m_engine = aEngine;
    connect(this, &ControlPanel::signalEngineEnable, m_engine, &EngineScript::slotEngineEnable, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalEngineSetCurStation, m_engine, &EngineScript::slotSetCurStation, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalEngineEnableTest, m_engine, &EngineScript::slotStartTest, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalFreeze, m_engine, &EngineScript::slotFreeze, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalResizeImage, m_engine, &EngineScript::slotResizeImage, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalSetQueue, m_engine, &EngineScript::slotSetQueue, Qt::QueuedConnection);
    connect(this, &ControlPanel::signalSetSide, m_engine, &EngineScript::slotSetSide, Qt::QueuedConnection);
    connect(m_engine, &EngineScript::signalReturnCommand, this, &ControlPanel::slotReceivReturnCommand, Qt::QueuedConnection);


    init();
    m_sTarget = slistSystems[0];
}

ControlPanel::~ControlPanel()
{
    delete ui;
}

void ControlPanel::test()
{
    queue.clear();
//    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0" });
//    queue.enqueue(QStringList {"MAPSYSTEM",                 "0", m_sTarget});
    queue.enqueue(QStringList {"AIMPFLYAROUND",             "0", "0"});
    queue.enqueue(QStringList {"PICKUPSPEED",               "0", "6000"});
    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "k" });
//    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "7000"});
//    queue.enqueue(QStringList {"SENDEVENTCONTROL",          "0", "push_key", "k" });
    queue.enqueue(QStringList {"IMAGEEXPECTED",             "0", "pic_hypermodResetPull", "9", "6", "26", "0.93"});
    queue.enqueue(QStringList {"MARKER",                    "0", "test"});
}

void ControlPanel::ring()
{
    queue.clear();
    queue.enqueue(QStringList {"RESTORGAME",            "0" });
    queue.enqueue(QStringList {"PANEL1ENABLE",          "0", "1"});      // включить панель
    queue.enqueue(QStringList {"PANEL1CASEHEADER",      "0", "навигация"});
    queue.enqueue(QStringList {"PANEL1CASEMENUNAV",     "0", m_sTarget});
    queue.enqueue(QStringList {"PANEL1SUBNAV",          "0", "fix_target"});
    queue.enqueue(QStringList {"PANEL1ENABLE",          "0", "0"});      // выключить панель
    queue.enqueue(QStringList {"DOCKINGMENUCASE",       "0", "menu_docking_autostart"});
    queue.enqueue(QStringList {"STOPAFTERAUTOSTART",    "0"});
    queue.enqueue(QStringList {"PICKUPSPEED",           "0", "5000"});
    queue.enqueue(QStringList {"ACTIONAIMP",            "0"});
    queue.enqueue(QStringList {"PANEL1ENABLE",          "0", "1"});
    queue.enqueue(QStringList {"PANEL1CASEHEADER",      "0", "навигация"});
    queue.enqueue(QStringList {"PANEL1CASEMENUNAV",     "0", m_sTarget});
    queue.enqueue(QStringList {"PANEL1SUBNAV",          "0", "enable_hypermode"});
    queue.enqueue(QStringList {"ACTIONWAIT",            "0", "13000"});
    queue.enqueue(QStringList {"PANEL1ENABLE",          "0", "1"});
    queue.enqueue(QStringList {"PANEL1CASEHEADER",      "0", "навигация"});
    queue.enqueue(QStringList {"PANEL1CASEMENUNAV",     "0", m_sTarget});
    queue.enqueue(QStringList {"PANEL1SUBNAV",          "0", "enable_hypermode_helper"});
    queue.enqueue(QStringList {"PANEL1ENABLE",          "0", "0"});
    queue.enqueue(QStringList {"WAITENDHYPERMODEHELP",  "0"});
    queue.enqueue(QStringList {"ACTIONGETCLOSER",       "0"});
    queue.enqueue(QStringList {"PANEL1ENABLE",          "0", "1"});
    queue.enqueue(QStringList {"PANEL1CASEHEADER",      "0", "контакты"});
    queue.enqueue(QStringList {"PANEL1CASEMENUCONT",    "0", m_sTarget});
    queue.enqueue(QStringList {"PANEL1ENABLE",          "0", "0"});
    queue.enqueue(QStringList {"WAITMENUDOCKING",       "0"});
    queue.enqueue(QStringList {"DOCKINGMENUCASE",       "0", "menu_docking_service"});
    queue.enqueue(QStringList {"IMAGEEXPECTED",         "0", "menuServiceExit", "4", "12", "13", "0.9"});
    queue.enqueue(QStringList {"SERVICEMENU",           "0"});

}

void ControlPanel::setTrack()
{
    int iTarget = slistSystems.indexOf(m_sTarget);
    if(iTarget == slistSystems.size() - 1)
        iTarget = 0;
    else
        iTarget++;
    m_sTarget = slistSystems[iTarget];
    queue.clear();
    queue.enqueue(QStringList {"MAPSYSTEMENABLE",           "0" });
//    queue.enqueue(QStringList {"ACTIONWAIT",                "0", "1000"});
    queue.enqueue(QStringList {"MAPSYSTEM",                 "0", m_sTarget});
    queue.enqueue(QStringList {"MARKER",                    "0", "setTrack"});


}

void ControlPanel::init()
{

    slistSystems << "Bluford orbital" << "Karex" << "Harma";
    ui->comboBox_2->addItems(slistSystems);
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


                if(ui->radioButton->isChecked())
                    m_sTarget = ui->radioButton->text();
                else
                    m_sTarget = ui->radioButton_2->text();


                test();
                emit signalSetQueue(queue);
            } else {
                qDebug() << "Control panel not recognize return value";
            }
        } else {
            queue.clear();
            qDebug() << "receiv return command " << aList;
            if(aList[1] == "1") {
                if(aList[0] == "AIMPFLYAROUND") {
                    qDebug() << "создаём новый скрипт setTrack";
                    setTrack();
                    emit signalSetQueue(queue);
                } else {
                    qDebug() << "slotReceivReturnCommand" << " if(aList[1] == 1) не обработанные состояния";
                }
            } else if(aList[1] == "0") {
                if(aList[0] == "MARKER") {
                    if(aList[2] == "test") {
                        qDebug() << "создаём скрипт setTrack";
                        test();
//                        setTrack();
                    } else if(aList[2] == "setTrack") {
                        qDebug() << "создаём скрипт test";
                        test();
//                        setTrack();
                    }
                    emit signalSetQueue(queue);
                } else {
                    qDebug() << "slotReceivReturnCommand" << " if(aList[0] == MARKER) { не обработанные состояния";
                }

            } else {
                qDebug() << "slotReceivReturnCommand" << " } else if(aList[1] == 0) { не обработанные состояния";
            }


        }
    }

}

void ControlPanel::on_checkBox_2_clicked()          // Пуск
{
//    setTrack();
    if(ui->radioButton->isChecked())
        m_sTarget = ui->radioButton->text();
    else
        m_sTarget = ui->radioButton_2->text();
    setTrack();
//    test();
    queue.prepend(QStringList {"RESTORGAME",                "0" });

    emit signalSetQueue(queue);
}

void ControlPanel::on_radioButton_clicked()
{
    m_sTarget = ui->radioButton->text();
}

void ControlPanel::on_radioButton_2_clicked()
{
    m_sTarget = ui->radioButton_2->text();
}

void ControlPanel::on_checkBox_clicked()
{
    ring();
    queue.prepend(QStringList {"RESTORGAME",                "0" });
    emit signalSetQueue(queue);

}

void ControlPanel::on_comboBox_activated(const QString &arg1)
{
    QString str = arg1;
    deleteCharExtra(str);
    m_sTarget = str;
}

void ControlPanel::on_radioButton_4_clicked()
{
    emit signalSetSide(2);
}

void ControlPanel::on_radioButton_6_clicked()
{
    emit signalSetSide(1);
}

void ControlPanel::on_radioButton_7_clicked()
{
    emit signalSetSide(3);
}

void ControlPanel::on_radioButton_3_clicked()
{
    emit signalSetSide(0);
}
