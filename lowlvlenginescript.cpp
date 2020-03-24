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
    BaseAction  *action;
    while(cycle) {
        readCommand(command);
        action = parseAction();
        if(action->getName() != "DEBUG") {
            qDebug() << "";
            qDebug() << "";
            qDebug() << "";
            qDebug() << "new action" << action->getName();
        }
        do {
            capture->update();
//            action->printDebug();
            QCoreApplication::processEvents();
        } while(!action->perform(command) && cycle);
        QCoreApplication::processEvents();
    }
}

void LowLvlEngineScript::init()
{
    m_sock = new SocketIO(QPoint( m_screen.x(), m_screen.y()));

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
    actions.append( new WaitEndHyperModeHelp(capture, m_sock));
    actions.append( new ActionGetCloser(capture, m_sock));
    actions.append( new Panel1CaseMenuCont(capture, m_sock));
    actions.append( new WaitMenuDocking(capture, m_sock));
    actions.append( new ImageExpected(capture, m_sock));
    actions.append( new ServiceMenu(capture, m_sock));
}

LowLvlEngineScript::~LowLvlEngineScript()
{
    delete m_sock;
}
