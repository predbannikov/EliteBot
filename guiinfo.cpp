#include "guiinfo.h"
GuiInfo::GuiInfo(IOData *t_ioData, QWidget *parent) : QWidget(parent)
{
    m_ioData = t_ioData;
    mp_dataSet = t_ioData->assignpDataSet();

    // layouts
//    vblayout = new QVBoxLayout;
    vboxlayout = new QVBoxLayout;
    setLayout(vboxlayout);
    initLayoutProjects();
    initLayoutRegion();
    initLayoutManipulation();




    sliderMaxContours = new QSlider;
    sliderMaxContours->setMaximumWidth(USING_WIDTH);
//    connect(sliderMaxContours, &QSlider::valueChanged, &GuiInfo::signalSendMaxContourForLength);
    sliderMaxContours->setValue(30);
    connect(sliderMaxContours, &QSlider::valueChanged, [this] (int anValue) {
        emit signalSendMaxContourForLength(anValue);
    });

    vboxlayout->addLayout(hboxProjects);
    vboxlayout->addLayout(hboxRegion);
    vboxlayout->addLayout(hboxManipulation);
    vboxlayout->addLayout(hblSlider);
    vboxlayout->addWidget(sliderMaxContours);





    updateGuiInfo();

}

bool GuiInfo::contains(std::string t_name)
{
    for(auto _re: *mp_dataSet){
        if(_re.second.name == t_name)
            return true;
    }
    return false;
}

void GuiInfo::initInfoDataRegion()
{
    cmbBoxRegions->clear();
    if(mp_dataSet->empty())
        return;
    QStringList _listItem = m_ioData->loadProjectListItems();
    cmbBoxRegions->addItems(_listItem);
    leNameRegion->setText(cmbBoxRegions->currentText());
    chckBoxReginActiveRect->setChecked(mp_dataSet->at(cmbBoxRegions->currentText().toStdString()).active);
}

void GuiInfo::initInfoProjects()
{
    cmbBoxProjects->clear();
    cmbBoxProjects->addItems(m_ioData->getListProjects());
    cmbBoxProjects->setCurrentText(m_ioData->getCurrentPathProject());
}

void GuiInfo::initLayoutProjects()
{
    hboxProjects = new QHBoxLayout;
    cmbBoxProjects = new QComboBox;
    connect(cmbBoxProjects, qOverload<const QString &>(&QComboBox::activated), [this] (const QString t_name) {
        m_ioData->setCurrentNameProject(t_name);
        m_ioData->loadProjectData();
        mp_dataSet = m_ioData->assignpDataSet();
        updateGuiInfo();
    });
    buttonCreateProject = new QPushButton("create");
    connect(buttonCreateProject, &QPushButton::clicked, [this]() {
        DialogCreateProject dialogCreateProject(m_ioData);
        dialogCreateProject.setModal(true);
        dialogCreateProject.exec();
        qDebug() << m_ioData->jConfig["defaultProject"].toString();
        updateGuiInfo();
    });
    buttonDeleteProject = new QPushButton("delete");
    connect(buttonDeleteProject, &QPushButton::clicked, [this]() {
        m_ioData->deleteProject(cmbBoxProjects->currentText());
        updateGuiInfo();
    });
    hboxProjects->addWidget(cmbBoxProjects);
    hboxProjects->addWidget(buttonCreateProject);
    hboxProjects->addWidget(buttonDeleteProject);
}

void GuiInfo::initLayoutRegion()
{
    hboxRegion = new QHBoxLayout;
    cmbBoxRegions = new QComboBox;
    leNameRegion = new QLineEdit;
    buttonSaveRegion = new QPushButton("save");
    buttonDeleteRegion = new QPushButton("delete");
    chckBoxReginActiveRect = new QCheckBox;

    connect(cmbBoxRegions, qOverload<const QString &>(&QComboBox::activated), [this] (const QString t_name){
        chckBoxReginActiveRect->setChecked(mp_dataSet->at(t_name.toStdString()).active);
        leNameRegion->setText(t_name);
    });
    connect(buttonSaveRegion, &QPushButton::clicked, [this]() {
        saveRoi();
        cmbBoxRegions->clear();
        cmbBoxRegions->addItems(m_ioData->loadProjectListItems());
        cmbBoxRegions->setCurrentText(leNameRegion->text());
    });
    connect(buttonDeleteRegion, &QPushButton::clicked, [this] () {
        m_ioData->deleteElement(cmbBoxRegions->currentText());
        initInfoDataRegion();
    });
    connect(chckBoxReginActiveRect, qOverload<int>(&QCheckBox::stateChanged), [this] (int value) {
        qDebug() << "value" << value;
        if(this->cmbBoxRegions->currentText().isEmpty())
            return;
        this->mp_dataSet->at(this->cmbBoxRegions->currentText().toStdString()).active = static_cast<bool>(value);
    });
    hboxRegion->addWidget(cmbBoxRegions);
    hboxRegion->addWidget(leNameRegion);
    hboxRegion->addWidget(chckBoxReginActiveRect);
    hboxRegion->addWidget(buttonSaveRegion);
    hboxRegion->addWidget(buttonDeleteRegion);
}


void GuiInfo::initLayoutManipulation()
{
    hboxManipulation = new QHBoxLayout;
    vboxlayout->addLayout(hboxManipulation);
    buttonCheckRoi = new QPushButton("chckRoi");
    buttonCheckXRoi = new QPushButton("chckXRoi");
    buttonCheckXRect = new QPushButton("chckXRect");
    chckBoxDrawMesh = new QCheckBox("drawMesh");
    spboxCountCell = new QSpinBox(this);
    hboxManipulation->addWidget(buttonCheckRoi);
    hboxManipulation->addWidget(buttonCheckXRect);
    hboxManipulation->addWidget(buttonCheckXRoi);
    hboxManipulation->addWidget(chckBoxDrawMesh);
    hboxManipulation->addWidget(spboxCountCell);
    spboxCountCell->setValue(4);

    sliderMin1 = new QSlider(Qt::Horizontal, this);
    sliderMin2 = new QSlider(Qt::Horizontal, this);
    sliderMin3 = new QSlider(Qt::Horizontal, this);
    sliderMax1 = new QSlider(Qt::Horizontal, this);
    sliderMax2 = new QSlider(Qt::Horizontal, this);
    sliderMax3 = new QSlider(Qt::Horizontal, this);
    sliderMin1->setMaximum(255);
    sliderMin2->setMaximum(255);
    sliderMin3->setMaximum(255);
    sliderMax1->setMaximum(255);
    sliderMax2->setMaximum(255);
    sliderMax3->setMaximum(255);
    sliderMin2->setValue(30);
    sliderMax1->setValue(255);
    sliderMax2->setValue(255);
    sliderMax3->setValue(255);



    connect(sliderMin1, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMinScalar(cv::Scalar(anValue, this->sliderMin2->value(), this->sliderMin3->value()));
    });
    connect(sliderMin2, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMinScalar(cv::Scalar(this->sliderMin1->value(), anValue, this->sliderMin3->value()));
    });
    connect(sliderMin3, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMinScalar(cv::Scalar(this->sliderMin1->value(), this->sliderMin2->value(), anValue));
    });
    connect(sliderMax1, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMaxScalar(cv::Scalar(anValue, this->sliderMax2->value(), this->sliderMax3->value()));
    });
    connect(sliderMax2, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMaxScalar(cv::Scalar(this->sliderMax1->value(), anValue, this->sliderMax3->value()));
    });
    connect(sliderMax3, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMaxScalar(cv::Scalar(this->sliderMax1->value(), this->sliderMax2->value(), anValue));
    });

    vblMinSlider = new QVBoxLayout;
    vblMinSlider->addWidget(sliderMin1);
    vblMinSlider->addWidget(sliderMin2);
    vblMinSlider->addWidget(sliderMin3);
    vblMaxSlider = new QVBoxLayout;
    vblMaxSlider->addWidget(sliderMax1);
    vblMaxSlider->addWidget(sliderMax2);
    vblMaxSlider->addWidget(sliderMax3);
    hblSlider = new QHBoxLayout;
    hblSlider->addLayout(vblMinSlider);
    hblSlider->addLayout(vblMaxSlider);


    connect(chckBoxDrawMesh, &QCheckBox::stateChanged, [this] () {
        emit signalDrawMesh(chckBoxDrawMesh->checkState(), spboxCountCell->value());
    });
    connect(buttonCheckRoi, &QPushButton::clicked, [this] () {
        QJsonArray _jArray;
        QJsonObject _jObj;
        _jObj["index"] = 0;
        _jArray.append(_jObj);
        emit sendScriptPerform(_jArray);
    });
    connect(buttonCheckXRoi, &QPushButton::clicked, [this] () {
        QJsonArray _jArray;
        QJsonObject _jObj;
        _jObj["index"] = 0;
        _jArray.append(_jObj);
        emit sendScriptPerform(_jArray);
    });
    connect(buttonCheckXRect, &QPushButton::clicked, [this] () {
        QJsonArray _jArray;
        QJsonObject _jObj;
        _jObj["index"] = 0;
        _jArray.append(_jObj);
        emit sendScriptPerform(_jArray);
    });
}




void GuiInfo::updateGuiInfo()
{
    initInfoProjects();
    initInfoDataRegion();
}

void GuiInfo::saveRoi()
{
    QJsonObject _jobj;
    if(leNameRegion->text().isEmpty()) {
        QMessageBox wrngMsg;
        wrngMsg.setText( "do not use empty name");
        int i = wrngMsg.exec();
        qDebug() << "stop" << i;
        return;
    }
    if(contains(leNameRegion->text().toStdString())) {

        if(QMessageBox::warning(this, "Warnign", "this is name already used, would you like set this name?", QMessageBox::No | QMessageBox::Yes) == QMessageBox::No)
            return;
    }
    m_mat = emit signalGetMatRoi();
    if(m_mat == nullptr) {
        QMessageBox::warning(this, "Warnign", "allocate roi please");
        return;
    }
    m_rect = emit signalGetRectRoi();
    if(m_rect == nullptr) {
        QMessageBox::warning(this, "Warnign", "allocate roi please");
        return;
    }
    if(mp_dataSet->find(leNameRegion->text().toStdString()) == mp_dataSet->end()) {

        ImageROI set;
        set.mat = *m_mat;
        set.rect = *m_rect;
        set.active = false;
        set.name = leNameRegion->text().toStdString();
        mp_dataSet->insert(std::pair<std::string, ImageROI>(leNameRegion->text().toStdString(), set));
    } else {

        mp_dataSet->at(leNameRegion->text().toStdString()).mat = *m_mat;
        mp_dataSet->at(leNameRegion->text().toStdString()).rect = *m_rect;
        mp_dataSet->at(leNameRegion->text().toStdString()).active = chckBoxReginActiveRect->checkState();
    }
    m_ioData->saveProjectRoi(leNameRegion->text());
}

//void GuiInfo::loadDialog(cv::Mat t_mat)
//{
//    m_mat = &t_mat;
//    QStringList list;
//    updateGuiInfo();
//    this->setVisible(true);
//}

void GuiInfo::openDialog(cv::Rect *t_rect, cv::Mat *t_mat)
{
    updateGuiInfo();
    buttonSaveRegion->setEnabled(true);
    m_mat = t_mat;
    m_rect = t_rect;
    this->setVisible(true);
//    this->show();

}

void GuiInfo::openDialog()
{
//    updateGuiInfo();
//    if(mp_dataSet->size() == 0) {
//        buttonSaveRegion->setEnabled(false);
//    } else {
//        buttonSaveRegion->setEnabled(true);
//        m_mat = &mp_dataSet->at(cmbBoxRegions->currentText().toStdString()).mat;
//        m_rect = &mp_dataSet->at(cmbBoxRegions->currentText().toStdString()).rect;
//    }
//    this->show();
    this->setVisible(true);
}

void GuiInfo::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->setVisible(false);
}
