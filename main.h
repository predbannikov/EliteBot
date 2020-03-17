#ifndef MAIN_H
#define MAIN_H
#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QFileInfo>
#include <QDateTime>
#include "global.h"

#define STANDART_HD_WIDTH 1280
#define STANDART_HD_HEIGHT 768
#define STANDART_FULLHD_WIDTH 1920
#define STANDART_FULLHD_HEIGHT 1080

//#define m_screen.width() STANDART_FULLHD_WIDTH
//#define m_screen.height() STANDART_FULLHD_HEIGHT

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

//void myLoger();


#endif // MAIN_H
