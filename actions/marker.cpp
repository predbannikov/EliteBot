#include "marker.h"

Marker::Marker(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "MARKER";

}

void Marker::init(QStringList &asListParam)
{
    sys_debugLog = false;
}

bool Marker::logic(QStringList &asListParam)
{
    return true;
}

void Marker::printDebug()
{

}

void Marker::reset()
{
}
