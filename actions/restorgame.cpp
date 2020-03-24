#include "restorgame.h"

RestorGame::RestorGame(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "RESTORGAME";
}

void RestorGame::init(QStringList &asListParam)
{
    trigger = false;
}

bool RestorGame::logic(QStringList &asListParam)
{
    mouse_move_click(m_screen.width()/2, m_screen.height()/2);
    return true;
}

void RestorGame::reset()
{
}
