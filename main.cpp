//#include <iostream>
#include "guiinfo.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <QTextCodec>

#include <QApplication>



void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type)
    Q_UNUSED(context)
    static const QString logFileName = QString("%1\\%2.log").arg(PATH_LOG).arg(QDateTime::currentDateTime().toString("yyMMdd-hhmmss"));

    #if defined (Q_OS_WIN)
    QFile logFile(logFileName);
    #elif defined (Q_OS_LINUX)
    QFile logFile("./log/file.log");
    #endif
    QString sContext = QString("%1%2%3").arg(context.file).arg(context.line).arg(context.function);
    QString localMsg;
    localMsg = QString("%0\n%1\n%2\n%3\n")
            .arg(sContext)
            .arg(msg)
            .arg(QDateTime::currentDateTime().toString("yy.MM.dd hh:mm:ss"))
            .arg("------------------------------------------------------------");
    if (logFile.open(QFile::WriteOnly | QFile::Text | QFile::Append)) {
        QTextStream out(&logFile);
        out << localMsg;
        logFile.close();
//        if(type == QtDebugMsg)
            qDebug() << msg.toUtf8().data();
    } else {
        std::cout << "error opening output file" << std::endl;
    }

}




int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN32
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));
#endif
#ifdef Q_OS_LINUX
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif


    QString path = PATH_LOG;
    if(!QFileInfo::exists(path)) {
        QDir pathDir;
        pathDir.setPath(pathDir.currentPath() + '/' + path);
        qDebug() << pathDir.path();
        if(!pathDir.mkdir(pathDir.path()))
            qDebug() << "log create not success";

    }
    QString config = PATH_CONFIG;
    if(!QFileInfo::exists(config)) {
        QDir pathDir;
        pathDir.setPath(pathDir.currentPath() + '/' + config);
        qDebug() << pathDir.path();
        if(!pathDir.mkdir(pathDir.path()))
            qDebug() << "config create not success";

    }
    QString videlog = PATH_VIDEO_LOG;
    if(!QFileInfo::exists(videlog)) {
        QDir pathDir;
        pathDir.setPath(pathDir.currentPath() + '/' + videlog);
        qDebug() << pathDir.path();
        if(!pathDir.mkdir(pathDir.path()))
            qDebug() << "videlog create not success";

    }

    QApplication a(argc, argv);

    qInstallMessageHandler(myMessageOutput);

//    key_filter kf(&a);
//    a.installEventFilter(&kf);

    IOData *pIOData = new IOData();
    GuiInfo pGui(pIOData);
    pGui.show();


//    cv::Mat mat = cv::imread("F:/projects/work/elite/autoClickerOnOpenCV/1.jpg", cv::IMREAD_COLOR);
//    cv::imwrite("2.jpg", mat);
//    cv::imshow("win2", mat);



    return a.exec();

}
