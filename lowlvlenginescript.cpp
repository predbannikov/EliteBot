#include "lowlvlenginescript.h"



LowLvlEngineScript::LowLvlEngineScript()
{
    m_sock = new SocketIO(QPoint( m_screen.x(), m_screen.y()));
}

BaseAction *LowLvlEngineScript::parseAction()
{
    if(command[0] == "PANEL1ENABLE") {
        return panel1Enable;
    } else if(command[0] == "WAIT") {
        return actionWait;
    } else if(command[0] == "BODY") {
        //
    }
    return nullptr;
}

void LowLvlEngineScript::update()
{
    BaseAction  *action;
    while(cycle) {
        readCommand(command);
        action = parseAction();
        do {
            capture->update();
            action->printDebug();
            QCoreApplication::processEvents();
        } while(!action->perform(command) && cycle);
        QCoreApplication::processEvents();
    }
}

void LowLvlEngineScript::init()
{

    panel1Enable = new Panel1Enable(capture, m_sock);
    panel1Enable->init();
    actionWait = new ActionWait(capture, m_sock);
    actionWait->init();
}

LowLvlEngineScript::~LowLvlEngineScript()
{
    delete m_sock;
}
