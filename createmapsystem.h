#ifndef CREATEMAPSYSTEM_H
#define CREATEMAPSYSTEM_H

#include <QDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QBoxLayout>
#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QFileDialog>
#include <global.h>
#include <QDebug>

class CreateMapSystem : public QDialog
{
    Q_OBJECT

public:
    CreateMapSystem(QWidget *parent = nullptr);

private:
    QJsonArray m_jSystemArray;
    QJsonArray m_jStationArray1;
    QJsonArray m_jStationArray2;

    QJsonArray m_jArrOut;
    QJsonObject m_jObjOut;

    QComboBox   *m_cmbBoxListSystem;
    QLineEdit   *m_lineEdit;
    QPushButton *m_buttonRemoveItem;
    QPushButton *m_buttonSaveItem;
    QPushButton *m_buttonPrepJson;

    void writeOut();
    void readData();

    QMultiMap <QString, QString> reqMap;
    QStringList m_listSystem;

};

#endif // CREATEMAPSYSTEM_H
