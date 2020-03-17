#ifndef IODATA_H
#define IODATA_H
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QJsonObject>
#include "global.h"



class IOData
{
    const QString fileNameConfig = "data_config";
    const QString appConfig = "config";
    const QString appDirName = "cv-bot";
    const QString defaultProjectName = "default";          // Нельзя удалить
    QString defaultScriptName = "project";
    QMap <QString, QJsonArray> m_scripts;
    std::map<std::string, ImageROI>  m_dataSet;
    QDir dataFolder;
    QJsonArray mja_script;
public:
    QJsonObject jConfig;


    IOData();
    bool prepWorkPath();

    // helpers function
    QString pathDataConfig();
    QString currentNameProject();



    // manage projects
    bool addNewProject(QString t_newProject);
    void loadAppConfig();
    void saveAppConfig();
    QStringList getListProjects();
    QString getCurrentPathProject();
    void setCurrentNameProject(QString t_name);
    void deleteProject(QString t_name);

    // work this project
    void copyCurrentScript(QString t_nameSrc, QString t_nameDst);
    void saveProjectRoi(QString t_name);
    void saveProjectConfig();
    QStringList loadProjectListItems();
    void loadProjectData();
    void deleteElement(QString t_name);
    void deleteScript(QString t_name);
    std::map<std::string, ImageROI> *assignpDataSet();
    QJsonArray* assignpScript(QString t_name = "default");
    QJsonObject loadProjectConfigFile(QString t_name);
    QJsonArray loadProjectConfig(QString t_name);
    QStringList getListScripts();
    QStringList getScripts();
    int getLastIndexScript();

    QString getPathForDebug(QString asName = "debug");
    void saveImageForDebug(cv::Mat acvMat, QString asName = "debug");
};

#endif // IODATA_H
