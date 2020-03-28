#include "pickupspeed.h"

PickUpSpeed::PickUpSpeed(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "PICKUPSPEED";
}

void PickUpSpeed::init(QStringList &asListParam)
{
    waitMSec = asListParam[2].toInt();
}

bool PickUpSpeed::logic(QStringList &asListParam)
{
    if(!trigger) {
        timer.restart();
        press_key("w");
        trigger = true;
        qDebug() << "pickUpSpeed -> press_key 'w' elapse =" << timer.elapsed();
    } else {
        if(timer.elapsed() > waitMSec) {
            qDebug() << "pickUpSpeed => release_key 'w' elapse =" << timer.elapsed();
            release_key("w");
            return true;
        }
    }
    return false;
}

void PickUpSpeed::reset()
{

}
