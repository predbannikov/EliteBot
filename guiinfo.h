#ifndef GUIINFO_H
#define GUIINFO_H

#include <QWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QTcpSocket>
#include <QCheckBox>
#include <QLabel>
#include <QSpacerItem>
#include <QComboBox>
#include <QSpinBox>
#include "iodata.h"
#include "global.h"

class GuiInfo : public QWidget
{
    Q_OBJECT
    QVBoxLayout     *vblayout;
    //  Panel of projects
    QHBoxLayout     *hboxProjects;
    QComboBox       *cmbBoxProjects;
    QPushButton     *buttonCreateProject;
    QPushButton     *buttonDeleteProject;
    //  Panel add region
    QHBoxLayout     *hboxRegion;
    QLineEdit       *leNameRegion;
    QComboBox       *cmbBoxRegions;
    QPushButton     *buttonDeleteRegion;
    QPushButton     *buttonSaveRegion;
    QCheckBox       *chckBoxReginActiveRect;

    // Panel manipulation
    QHBoxLayout     *hboxManipulation;
    QPushButton     *buttonCheckRoi;
    QPushButton     *buttonCheckXRoi;
    QPushButton     *buttonCheckXRect;
    QCheckBox       *chckBoxDrawMesh;
    QSpinBox        *spboxCountCell;

    QHBoxLayout     *hblSlider;
    QVBoxLayout     *vblMinSlider;
    QVBoxLayout     *vblMaxSlider;
    QSlider         *sliderMin1;
    QSlider         *sliderMin2;
    QSlider         *sliderMin3;
    QSlider         *sliderMax1;
    QSlider         *sliderMax2;
    QSlider         *sliderMax3;

    QSlider         *sliderMaxContours;



    //
    QVBoxLayout     *vboxlayout;
    cv::Mat         *m_mat;
    cv::Rect        *m_rect;
    QTcpSocket      *sock;
    QString nameProject = "project";
    IOData *m_ioData;
    std::map<std::string, ImageROI> *mp_dataSet;
    bool contains(std::string t_name);
    void initInfoDataRegion();
    void initInfoProjects();
    void initLayoutProjects();
    void initLayoutRegion();
    void initLayoutManipulation();
//    void loadDataSet();


public:
    explicit GuiInfo(IOData *t_ioData, QWidget *parent = nullptr);
    void updateGuiInfo();
    void saveRoi();
//    void loadDialog(cv::Mat t_mat);
    void openDialog(cv::Rect *t_rect, cv::Mat *t_mat);
    void openDialog();

private:
    void closeEvent(QCloseEvent *event);

signals:
    void signalDrawMatchRect(Qt::CheckState t_state);
    void signalSetLoop(Qt::CheckState t_state);
    void sendScriptPerform(QJsonArray);

    void signalDrawMesh(bool abCheck, int anCount);
    cv::Mat *signalGetMatRoi();
    cv::Rect *signalGetRectRoi();
    void signalSendMinScalar(cv::Scalar acvMinScalar);
    void signalSendMaxScalar(cv::Scalar acvMaxScalar);
    void signalSendMaxContourForLength(int i);
    void closeWindow();
public slots:
};

// *************************************************************************************************************

class DialogCreateProject : public QDialog
{
    Q_OBJECT
    QLineEdit *ledit;
public:
    explicit DialogCreateProject(IOData *_iodata, QWidget *parent = nullptr):
        QDialog(parent)
    {
        iodata = _iodata;
        ledit = new QLineEdit;
        ledit->setText(iodata->jConfig.value("defaultProject").toString());
        ledit->installEventFilter(this);
        QHBoxLayout *vbox = new QHBoxLayout;
        QPushButton *btOk = new QPushButton("ok");
        connect(btOk, &QPushButton::clicked, this, &DialogCreateProject::saveAndExit);
        QPushButton *btCancel = new QPushButton("cancle");
        connect(btCancel, &QPushButton::clicked, [this] () { this->close();});
        setLayout(vbox);
        vbox->addWidget(ledit);
        vbox->addWidget(btOk);
        vbox->addWidget(btCancel);
    }
private:
    IOData *iodata;
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::KeyPress && obj == ledit) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if(keyEvent->key() == Qt::Key_Return) {
                saveAndExit();
            }
            return QObject::eventFilter(obj, event);
        } else {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }
    }
protected:

signals:

private slots:
    void saveAndExit() {
        if(!iodata->addNewProject(ledit->text())) {
            QMessageBox::critical(this, "warning", "not use this name. This is name already used");
            return;
        }
        iodata->jConfig["defaultProject"] = ledit->text();
        this->close();
    }
};



// *************************************************************************************************************

class DialogCopyScript: public QDialog
{
    Q_OBJECT
    QLineEdit *ledit;
public:
    explicit DialogCopyScript(QString t_scriptName, QString *t_newName,QWidget *parent = nullptr) :
        QDialog(parent),
        m_scriptName(t_newName)
    {
        ledit = new QLineEdit(t_scriptName);
        ledit->installEventFilter(this);
        QHBoxLayout *vbox = new QHBoxLayout;
        QPushButton *btOk = new QPushButton("ok");
        connect(btOk, &QPushButton::clicked, this, &DialogCopyScript::saveAndExit);
        QPushButton *btCancel = new QPushButton("cancle");
        connect(btCancel, &QPushButton::clicked, [this] () { this->close();});
        setLayout(vbox);
        vbox->addWidget(ledit);
        vbox->addWidget(btOk);
        vbox->addWidget(btCancel);
    }
private:
    QString *m_scriptName;
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::KeyPress && obj == ledit) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if(keyEvent->key() == Qt::Key_Return) {
                saveAndExit();
            }
            return QObject::eventFilter(obj, event);
        } else {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }
    }
protected:

signals:

private slots:
    void saveAndExit() {
        if(ledit->text().isEmpty()) {
            QMessageBox::critical(this, "warning", "empty string not allowed used");
            return;
        }
        if(ledit->text() == m_scriptName) {
            QMessageBox::critical(this, "warning", "this is name already used");
            return;
        }
        *m_scriptName = ledit->text();
        this->close();
    }
};


#endif // GUIINFO_H
