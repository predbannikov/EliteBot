#include "restorgame.h"

RestorGame::RestorGame(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "RESTORGAME";
}

void RestorGame::init(QStringList &asListParam)
{
}

bool RestorGame::logic(QStringList &asListParam)
{
    mouse_move_click(g_screen.width()/2, g_screen.height()/2);
    return true;
}

void RestorGame::reset()
{
}
