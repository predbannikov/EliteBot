#ifndef LOWLVLENGINESCRIPT_H
#define LOWLVLENGINESCRIPT_H

#include <QString>
#include <QDebug>
#include <QElapsedTimer>
#include <QTcpSocket>
#include <QCoreApplication>
#include "capturewindow.h"
#include "global.h"
#include "socketio.h"
#include "actions/panel1Enable.h"
#include "actions/actiondebug.h"
#include "actions/panel1casehead.h"
#include "actions/panel1casemenunav.h"
#include "actions/panel1subnav.h"
#include "actions/dockingmenucase.h"
#include "actions/actionwait.h"
#include "actions/restorgame.h"
#include "actions/stopafterautostart.h"
#include "actions/pickupspeed.h"
#include "actions/actionaimp.h"
#include "actions/imageexpectedclose.h"
#include "actions/actiongetcloser.h"
#include "actions/panel1casemenucont.h"
#include "actions/waitmenudocking.h"
#include "actions/imageexpected.h"
#include "actions/servicemenu.h"
#include "actions/mapsystemenable.h"
#include "actions/mapsystem.h"
#include "actions/aimpflyaround.h"
#include "actions/sendeventcontrol.h"
#include "actions/marker.h"
#include "actions/getstrstaticfield.h"
#include "actions/actiondeliverypaper.h"
#include "actions/checkcursystem.h"




class LowLvlEngineScript
{
    QStringList     command;

    BaseAction          *parseAction();

    QVector<BaseAction *> actions;
    SocketIO            *m_sock;
public:
    virtual ~LowLvlEngineScript();
    LowLvlEngineScript();
    void update();
    bool cycle = true;
    virtual void    init();
    virtual void    readCommand(QStringList &aslistCommand) {}
    CaptureWindow   *capture;
    IOData          *m_pIOData;
    std::map<std::string, ImageROI> *m_pDataSet;

protected:

    QString m_sRetFunction;

private:



};

#endif // LOWLVLENGINESCRIPT_H
