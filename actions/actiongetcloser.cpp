#include "actiongetcloser.h"

ActionGetCloser::ActionGetCloser(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "ACTIONGETCLOSER";
}

void ActionGetCloser::init(QStringList &asListParam)
{
    trigger = false;
    timer.restart();
    waitMSec = 6770;
}

bool ActionGetCloser::logic(QStringList &asListParam)
{
//    static bool resetDistance = false;
    qint64 elaps = timer.elapsed();
        if(!trigger) {
            press_key("w");
            trigger = true;
        }
        if(elaps > waitMSec) {
            release_key("w");
            for(int i = 0; i < 7 ; i++) {
                push_key("x");
                QThread::msleep(1000);
//                resetDistance = false;
            }
            qDebug() << "Выход из enginescripte APPROACH";
            return true;
        }
        //                Distance *distance = capture->recognizDistance();
        //                if(!resetDistance) {
        //                    distance->distanceList.clear();
        //                    distance->distance = 0;
        //                    resetDistance = true;
        //                } else {
        //                    if(!approach) {
        //                        press_key("w");
        //                        approach = true;
        //                    }
        //                    if(distance->distance < std::numeric_limits<double>::epsilon () &&
        //                       distance->distance > -std::numeric_limits<double>::epsilon () ) {
        //                        qDebug() << "distance zero" << distance->distanceList;
        //                        break;
        //                    }

        //                    if(distance->distance <= m_pControl->distance) {
        //                        release_key("w");
        //                        for(int i = 0; i < 5; i++) {
        //                            push_key("x");
        //                            QThread::msleep(1300);
        //                        }
        //                        approach = false;
        //                        resetDistance = false;
        //                        m_pControl->check = true;
        //                        m_pControl->state = AICONTROL;
        //                    }
        //                }


    return false;
}

void ActionGetCloser::reset()
{
}
