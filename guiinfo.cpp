#include "guiinfo.h"
GuiInfo::GuiInfo(IOData *t_ioData, QWidget *parent) : QWidget(parent)
{
    m_ioData = t_ioData;
    if(m_ioData->prepWorkPath()) {
        std::cout << "load DataSet success" << std::endl;

    } else  {
        std::cout << "load DataSet not seccess" << std::endl;
    }

    mp_dataSet = t_ioData->assignpDataSet();



    // layouts
    vboxlayout = new QVBoxLayout;
    setLayout(vboxlayout);
    initLayoutProjects();
    initLayoutRegion();
    initLayoutManipulation();
    updateGuiInfo();

    installEventFilter(this);



    sliderMaxContours = new QSlider;
    sliderMaxContours->setMaximumWidth(m_screen.width());
    sliderMaxContours->setValue(30);
    connect(sliderMaxContours, &QSlider::valueChanged, [this] (int anValue) {
        emit signalSendMaxContourForLength(anValue);
    });

    vboxlayout->addLayout(hboxProjects);
    vboxlayout->addLayout(hboxRegion);
    vboxlayout->addLayout(hboxManipulation);
    vboxlayout->addLayout(hblEngineControl);
    vboxlayout->addLayout(hblSlider);
    vboxlayout->addWidget(sliderMaxContours);



    engine = new EngineScript(m_ioData);
    connect(this, &GuiInfo::signalGetMatRoi, engine, &EngineScript::slotCheckRoiMat);
    connect(this, &GuiInfo::signalGetRectRoi, engine, &EngineScript::slotCheckRoiRect);
    connect(this, &GuiInfo::signalDrawMesh, engine, &EngineScript::slotSetDrawLine);
    connect(this, &GuiInfo::signalSendMinScalar, engine, &EngineScript::setMinScalar);
    connect(this, &GuiInfo::signalSendMaxScalar, engine, &EngineScript::setMaxScalar);
    connect(this, &GuiInfo::signalSendMinNumber, engine, &EngineScript::setMinNumber);
    connect(this, &GuiInfo::signalSendMidNumber, engine, &EngineScript::setMidNumber);
    connect(this, &GuiInfo::signalSendMaxNumber, engine, &EngineScript::setMaxNumber);
    connect(this, &GuiInfo::signalEngineEnable, engine, &EngineScript::slotEngineEnable);
    connect(this, &GuiInfo::signalEngineSetCurStation, engine, &EngineScript::slotSetCurStation);


    thread = new QThread;



    engine->moveToThread(thread);
    connect(thread, &QThread::finished, engine, &QObject::deleteLater);
    connect(thread, &QThread::started, engine, &EngineScript::run);
    thread->start();


    this->sendAllNumbData();
}

GuiInfo::~GuiInfo()
{
//    qDebug() << "begin exit";
    engine->cycle = false;
    thread->quit();
    cv::destroyAllWindows();
    thread->wait();
    delete thread;
    qDebug() << "end exit";
}

void GuiInfo::sendAllNumbData()
{

//    cv::Scalar minNumber( 0, 0, 0 );
//    cv::Scalar maxNumber (255, 255, 255 );


//    sliderMin1->setValue(  static_cast<int>(minScalar[0] ) );
//    sliderMin2->setValue(  static_cast<int>(minScalar[1] ) );
//    sliderMin3->setValue(  static_cast<int>(minScalar[2] ) );

//    sliderMax1->setValue(  static_cast<int>(maxScalar[0] ) );
//    sliderMax2->setValue(  static_cast<int>(maxScalar[1] ) );
//    sliderMax3->setValue(  static_cast<int>(maxScalar[2] ) );

    sliderMinNumber->setValue( 30 );
    sliderMidNumber->setValue( 40 );
    sliderMaxNumber->setValue( 1 );

    emit signalSendMinNumber(sliderMinNumber->value());
    emit signalSendMidNumber(sliderMidNumber->value());
    emit signalSendMaxNumber(sliderMaxNumber->value());

    emit signalSendMinScalar(this->sliderMin1->value(), this->sliderMin2->value(), this->sliderMin3->value());
    emit signalSendMaxScalar(this->sliderMax1->value(), this->sliderMax2->value(), this->sliderMax3->value());
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
    connect(chckBoxDrawMesh, &QCheckBox::stateChanged, [this] () {
        emit signalDrawMesh(chckBoxDrawMesh->checkState(), spboxCountCell->value());
    });
    connect(spboxCountCell, qOverload<const int>(&QSpinBox::valueChanged), [this] (int aValue) {
        emit signalDrawMesh(chckBoxDrawMesh->checkState(), aValue);
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
    sliderMin1->setValue(0);
    sliderMin2->setValue(0);
    sliderMin3->setValue(0);
    sliderMax1->setValue(255);
    sliderMax2->setValue(255);
    sliderMax3->setValue(255);

    sliderMinNumber = new QSlider(Qt::Horizontal, this);
    sliderMidNumber = new QSlider(Qt::Horizontal, this);
    sliderMaxNumber = new QSlider(Qt::Horizontal, this);
    sliderMinNumber->setMaximum(255);
    sliderMaxNumber->setMaximum(255);
    sliderMidNumber->setMaximum(255);
    sliderMinNumber->setValue(0);
    sliderMinNumber->setValue(127);
    sliderMaxNumber->setValue(250);


    connect(sliderMin1, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMinScalar(anValue, this->sliderMin2->value(), this->sliderMin3->value());
    });
    connect(sliderMin2, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMinScalar(this->sliderMin1->value(), anValue, this->sliderMin3->value());
    });
    connect(sliderMin3, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMinScalar(this->sliderMin1->value(), this->sliderMin2->value(), anValue);
    });
    connect(sliderMax1, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMaxScalar(anValue, this->sliderMax2->value(), this->sliderMax3->value());
    });
    connect(sliderMax2, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMaxScalar(this->sliderMax1->value(), anValue, this->sliderMax3->value());
    });
    connect(sliderMax3, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMaxScalar(this->sliderMax1->value(), this->sliderMax2->value(), anValue);
    });
    connect(sliderMinNumber, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMinNumber(anValue);
    });
    connect(sliderMidNumber, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMidNumber(anValue);
    });
    connect(sliderMaxNumber, &QSlider::valueChanged, [this](int anValue) {
        emit signalSendMaxNumber(anValue);
    });


    vblMinSlider = new QVBoxLayout;
    vblMinSlider->addWidget(sliderMin1);
    vblMinSlider->addWidget(sliderMin2);
    vblMinSlider->addWidget(sliderMin3);
    vblMinSlider->addWidget(sliderMinNumber);
    vblMinSlider->addWidget(sliderMidNumber);
    vblMinSlider->addWidget(sliderMaxNumber);
    vblMaxSlider = new QVBoxLayout;
    vblMaxSlider->addWidget(sliderMax1);
    vblMaxSlider->addWidget(sliderMax2);
    vblMaxSlider->addWidget(sliderMax3);
    hblSlider = new QHBoxLayout;
    hblSlider->addLayout(vblMinSlider);
    hblSlider->addLayout(vblMaxSlider);


    chckBoxEngine = new QCheckBox("Включить");
    connect(chckBoxEngine, &QCheckBox::stateChanged, [this] (bool aState) {
        emit signalEngineEnable(aState);
    });
    cmbCurStation = new QComboBox;
    QStringList list;
    list << "Gabriel enterprise" << "Celebi city";
    cmbCurStation->addItems(list);
    connect(cmbCurStation, qOverload<const QString &>(&QComboBox::activated), [this] (const QString asItem) {
        emit signalEngineSetCurStation(asItem);
    });


    hblEngineControl = new QHBoxLayout;
    hblEngineControl->addWidget(chckBoxEngine);
    hblEngineControl->addWidget(cmbCurStation);

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
    cv::Mat mat = emit signalGetMatRoi();
    if(mat.empty()) {
        QMessageBox::warning(this, "Warnign", "allocate roi please");
        return;
    }
    cv::Rect rect = emit signalGetRectRoi();
    if(rect.x == 0 && rect.y == 0 && rect.width == 0 && rect.height == 0) {
        QMessageBox::warning(this, "Warnign", "allocate roi please");
        return;
    }
    if(mp_dataSet->find(leNameRegion->text().toStdString()) == mp_dataSet->end()) {

        ImageROI set;
        set.mat = mat;
        set.rect = rect;
        set.active = false;
        set.name = leNameRegion->text().toStdString();
        mp_dataSet->insert(std::pair<std::string, ImageROI>(leNameRegion->text().toStdString(), set));
    } else {

        mp_dataSet->at(leNameRegion->text().toStdString()).mat = mat;
        mp_dataSet->at(leNameRegion->text().toStdString()).rect = rect;
        mp_dataSet->at(leNameRegion->text().toStdString()).active = chckBoxReginActiveRect->checkState();
    }
    m_ioData->saveProjectRoi(leNameRegion->text());
}

void GuiInfo::slotReadKey(QChar aChar)
{
    if(aChar == 'a') {
        QKeyEvent event1(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        QKeyEvent event2(QEvent::KeyRelease, Qt::Key_Left, Qt::NoModifier);
        QApplication::sendEvent(this, &event1);
        QApplication::sendEvent(this, &event2);
    } else if(aChar == 'd') {
        QKeyEvent event1(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
        QKeyEvent event2(QEvent::KeyRelease, Qt::Key_Right, Qt::NoModifier);
        QApplication::sendEvent(this, &event1);
        QApplication::sendEvent(this, &event2);
    }
}
