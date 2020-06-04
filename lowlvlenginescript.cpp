#include "lowlvlenginescript.h"

LowLvlEngineScript::LowLvlEngineScript()
{

}

BaseAction *LowLvlEngineScript::parseAction()
{
    for(int i = 0; i < actions.size(); i++) {
        if(command[0] == actions[i]->getName())
            return actions[i];
    }
    return nullptr;
}

void LowLvlEngineScript::update()
{
    if(DEBUG2)
        qDebug() << ".";

    BaseAction  *action;
    while(cycle) {
//        if(DEBUG2)
//            qDebug() << "while start";
        readCommand(command);
//        if(DEBUG2)
//            qDebug() << "after readCommand";
        action = parseAction();
//        if(DEBUG2)
//            qDebug() << "after parseAction";
        if(action->getName() != "DEBUG") {
            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
//            qDebug() << "new action" << action->getName();
        }
        do {
//            if(DEBUG2)
//                qDebug() << "in do while update";
            capture->update();
//            action->printDebug();
            QCoreApplication::processEvents();
//            if(DEBUG2)
//                qDebug() << "after update";

        } while(!action->perform(command) && cycle);
        QCoreApplication::processEvents();
    }
}

void LowLvlEngineScript::init()
{
    m_sock = new SocketIO(QPoint( g_screen.x(), g_screen.y()));

    actions.append( new Panel1Enable(capture, m_sock));
    actions.append( new Panel1CaseHead(capture, m_sock));
    actions.append( new Panel1CaseMenuNav(capture, m_sock));
    actions.append( new Panel1SubNav(capture, m_sock));
    actions.append( new DockingMenuCase(capture, m_sock));
    actions.append( new ActionWait(capture, m_sock));
    actions.append( new ActionDebug(capture, m_sock));
    actions.append( new RestorGame(capture, m_sock));
    actions.append( new StopAfterAutoStart(capture, m_sock));
    actions.append( new PickUpSpeed(capture, m_sock));
    actions.append( new ActionAimp(capture, m_sock));
    actions.append( new ImageExpectedClose(capture, m_sock));
    actions.append( new ImageExpected(capture, m_sock));
    actions.append( new ActionGetCloser(capture, m_sock));
    actions.append( new Panel1CaseMenuCont(capture, m_sock));
    actions.append( new WaitMenuDocking(capture, m_sock));
    actions.append( new ServiceMenu(capture, m_sock));
    actions.append( new MapSystemEnable(capture, m_sock));
    actions.append( new MapSystem(capture, m_sock));
    actions.append( new AimpFlyAround(capture, m_sock));
    actions.append( new SendEventControl(capture, m_sock));
    actions.append( new Marker(capture, m_sock));
    actions.append( new GetStrStaticField(capture, m_sock));
    actions.append( new ActionDeliveryPaper(capture, m_sock));
    actions.append( new CheckCurSystem(capture, m_sock));
}

LowLvlEngineScript::~LowLvlEngineScript()
{
    delete m_sock;
}
