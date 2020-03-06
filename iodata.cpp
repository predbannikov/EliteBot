#include "iodata.h"

IOData::IOData()
{
}

bool IOData::prepWorkPath()
{
    QString pathApp = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + appDirName + "/" + appConfig;
    jConfig["pathAppConfig"] = pathApp;

    if(!QFileInfo::exists(pathApp)) {               // Если папка отсутствует инициализируем по умолчанию
        QDir pathAppDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + appDirName);
        if(!pathAppDir.exists())
            pathAppDir.mkdir(pathAppDir.path());
        QDir pathDirData(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/" + appDirName);
        if(!pathDirData.exists())
            pathDirData.mkdir(pathDirData.path());
        jConfig["pathDirData"] = pathDirData.path();
        QDir pathDefaultProject(pathDirData.path() + "/" + defaultProjectName);
        if(!pathDefaultProject.exists())
            pathDefaultProject.mkdir(pathDefaultProject.path());
        addNewProject(pathDefaultProject.path());
    } else {
        loadAppConfig();
        dataFolder.setPath(jConfig["defaultProject"].toString());
        loadProjectData();
    }

    return true;
}

QString IOData::pathDataConfig()
{
    return QString(jConfig["pathDirData"].toString() + "/" + fileNameConfig);
}

QString IOData::currentNameProject()
{
    return jConfig["pathDirData"].toString().split('/').last();
}

bool IOData::addNewProject(QString t_newProject)
{
    QJsonArray jCurArr = jConfig["projectPatchs"].toArray();
    int i = 0;
    QJsonObject obj;
    for(; i < jCurArr.size(); i++) {
        if(t_newProject == jCurArr[i].toString())
            return false;
    }
    QDir _dir(t_newProject);
    if(!_dir.exists()) {
        _dir.mkdir(_dir.path());
    }
    jConfig["defaultProject"] = t_newProject;
    dataFolder.setPath(jConfig["defaultProject"].toString());
    jCurArr.append(t_newProject);
    jConfig["projectPatchs"] = jCurArr;
    saveAppConfig();
    return true;
}

QStringList IOData::getListProjects()
{
    QStringList listItems;
    for(QJsonValue curItem: jConfig["projectPatchs"].toArray()) {
        listItems << curItem.toString();
    }
    return listItems;
}

void IOData::deleteProject(QString t_name)
{
    if(t_name == defaultProjectName)
        return;
    QStringList list;
    QJsonArray jarray;
    for(QJsonValue item: jConfig["projectPatchs"].toArray()) {
        if(item.toString() == t_name)
            continue;
        jarray << item;
    }

    jConfig["projectPatchs"] = jarray;
    jConfig["defaultProject"] = defaultProjectName;
    saveAppConfig();
}

void IOData::copyCurrentScript(QString t_nameSrc, QString t_nameDst)
{
    QFile _file(dataFolder.filePath(t_nameDst + ".script"));
    qDebug() << "hit the target";
    if(!_file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "not open file to save app config";
        return;
    }
    _file.write(QJsonDocument(m_scripts[t_nameSrc]).toJson());
    _file.close();
    loadProjectData();
}

void IOData::saveProjectRoi(QString t_name)
{
    ImageROI set; // = m_dataSet.at(t_name.toStdString());
    set = m_dataSet.at(t_name.toStdString());
    cv::imwrite(dataFolder.filePath(t_name).toStdString() + ".png", set.mat);
    QJsonObject _jobj;
    _jobj["rect_x"] = set.rect.x;
    _jobj["rect_y"] = set.rect.y;
    _jobj["rect_width"] = set.rect.width;
    _jobj["rect_height"] = set.rect.height;
    _jobj["active"] = set.active;
    QFile file(dataFolder.filePath(t_name) + ".json");
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "not open file to save data_file.json";
        return;
    }
    file.write(QJsonDocument(_jobj).toJson());
    file.close();

    saveProjectConfig();
}

void IOData::saveProjectConfig()
{
    QString pathName = QString(dataFolder.filePath(defaultScriptName + ".script"));
    QFile file(pathName);
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "not open file to save" << defaultScriptName;
        return;
    }
    QList<QJsonObject> _listJObj;
    for(int i = 0; i < mja_script.size(); i++) {
        _listJObj.insert(mja_script[i].toObject()["index"].toInt(), mja_script[i].toObject());
    }
    QJsonArray _jArray;
    for(int i = 0; i < _listJObj.size(); i++) {
        _listJObj[i]["index"] = i;
        _jArray.append(_listJObj[i]);
    }
    mja_script = _jArray;
    file.write(QJsonDocument(mja_script).toJson());
    file.close();
    loadProjectData();
}

QStringList IOData::loadProjectListItems()
{
    QStringList _list = dataFolder.entryList(QDir::Files);
    QStringList _listNames;
    for(int i = 0; i < _list.size(); i++) {
        if(_list[i].contains(QRegExp(".json$")) && _list[i] != defaultScriptName) {
            _listNames << _list[i].left(_list[i].indexOf('.'));
        }
    }
    return _listNames;
}

void IOData::loadProjectData()
{
    m_scripts.clear();
    m_dataSet.clear();
    QStringList _listDir = dataFolder.entryList(QDir::Files);
    for(QString _name: _listDir) {
        if(_name.contains(QRegExp(".script$"))) {
            mja_script = loadProjectConfig(_name);
            m_scripts.insert(_name.left(_name.indexOf('.')), mja_script);
        } else if(_name.contains(QRegExp(".json$")))
        {
            if(m_dataSet.find(_name.toStdString()) == m_dataSet.end()) {
                QJsonObject _jobj = loadProjectConfigFile(_name);
                cv::Rect _rect(_jobj["rect_x"].toInt(), _jobj["rect_y"].toInt(), _jobj["rect_width"].toInt(), _jobj["rect_height"].toInt());
                ImageROI set;
                set.mat = cv::imread(dataFolder.filePath(_name.split('.')[0]).toStdString() + ".png");
                set.rect = _rect;
                set.name = _name.split('.')[0].toStdString();
                set.active = _jobj["active"].toBool();
                m_dataSet.insert(std::pair<std::string, ImageROI>(
                                     _name.split('.')[0].toStdString(),
                                     set));
            }
        }
    }

}

void IOData::deleteElement(QString t_name)
{
    QFile file(dataFolder.filePath(t_name) + ".png");
    file.remove();
    file.setFileName(dataFolder.filePath(t_name) + ".json");
    file.remove();
    m_dataSet.erase(t_name.toStdString());
}

void IOData::deleteScript(QString t_name)
{
    QFile file(dataFolder.filePath(t_name) + ".script");
    file.remove();
    loadProjectData();
}

std::map<std::string, ImageROI> *IOData::assignpDataSet()
{
    return &m_dataSet;
}

QJsonObject IOData::loadProjectConfigFile(QString t_name)
{
    QFile file(dataFolder.filePath(t_name));
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "not open file to read data file config";
        return QJsonObject();
    }
    QJsonObject _jobj;
    _jobj = QJsonDocument::fromJson(file.readAll()).object();
    file.close();
    return _jobj;
}

QJsonArray IOData::loadProjectConfig(QString t_name)
{
    QFile file(dataFolder.filePath(t_name));
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "not open file to read data file config";
        return QJsonArray();
    }
    QByteArray _array = file.readAll();
    QJsonDocument _jdoc = QJsonDocument::fromJson(_array);
    file.close();
    return _jdoc.array();
}

QJsonArray *IOData::assignpScript(QString t_name)
{
    defaultScriptName = t_name;
    mja_script = m_scripts[t_name];
    return &mja_script;
}

QStringList IOData::getListScripts()
{
    QStringList _list;
    for(QJsonValue _value: mja_script) {
        _list << QString::number(_value.toObject()["index"].toInt());
    }
    return _list;
}

QStringList IOData::getScripts()
{
    QStringList _list;
    for(QString _name: m_scripts.keys()) {
        _list << _name;
    }
    return _list;
}

int IOData::getLastIndexScript()
{
    int max = 0;
    for(int i = 0; i < mja_script.size(); i++) {
        int index = mja_script[i].toObject()["index"].toInt();
        if(index > max)
            max = index;
    }
    return max;
}

QString IOData::getPathForDebug(QString asName)
{
    return dataFolder.filePath(asName);
}

void IOData::saveImageForDebug(cv::Mat acvMat, QString asName)
{
    cv::imwrite(getPathForDebug(asName).toStdString() + ".png", acvMat);
}

QString IOData::getCurrentPathProject()
{
    return jConfig["defaultProject"].toString();
}

void IOData::setCurrentNameProject(QString t_name)
{
    jConfig["defaultProject"] = t_name;
    dataFolder.setPath(jConfig["defaultProject"].toString());
    saveAppConfig();
}

void IOData::loadAppConfig()
{
    QFile file(jConfig["pathAppConfig"].toString());
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "not open file to read app config";
        return;
    }
    jConfig = QJsonDocument::fromJson(file.readAll()).object();
    file.close();
}

void IOData::saveAppConfig()
{
    QFile file(jConfig["pathAppConfig"].toString());
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "not open file to save app config";
        return;
    }
    file.write(QJsonDocument(jConfig).toJson());
    file.close();
}
