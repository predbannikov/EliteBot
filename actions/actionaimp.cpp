#include "actionaimp.h"

ActionAimp::ActionAimp(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "ACTIONAIMP";
}

void ActionAimp::init(QStringList &asListParam)
{
    m_bCompass = false;
    waitEnable = 1500;
    confirmTime = 500;
    trigger = false;
    timer.restart();
    confirmTimer.restart();

}

bool ActionAimp::logic(QStringList &asListParam)
{
    if(!m_bCompass) {
#define SPEED_COMPAS_Y      200
#define SPEED_COMPAS_ROT    200
        Compass *compass = capture->compass();
        if(!compass->active)
            return false;
        int dx = compass->pCenter.x - compass->pDot.x;
        int dy = compass->pCenter.y - compass->pDot.y;
        static int powerX;
        static int seekY;
        static bool pickup = false;
        static bool pickdownX = false;
        static bool pickdownY = false ;
        static bool rotate = false ;
        const int approxim = 8;            // Расстояние до центра до которого компас реагирует
        // Вычисление угла между двумя векторами
        cv::Point a(compass->pCenter.x - compass->pDot.x, compass->pCenter.y - compass->pDot.y);
        cv::Point b(0, 1);
        double radius = sqrt(pow(dx,2) + pow(dy, 2));

        double atan_a = atan2(a.x * b.y - b.x * a.y, a.x * b.x + a.y * b.y);
        double grad = atan_a * 180. / M_PI;
        qDebug() << "grad=" << grad << " radius=" << radius;
        //  Выравнивание по ротации
        if(!rotate) {
            qDebug() << "radius =" << radius;
            if(approxim <= radius) {
                if((grad > 10 || grad < -10)) {         // выровнять по градусам
                    if(dx == 0)
                        dx = 1;
                    powerX = SPEED_COMPAS_ROT * (dx / abs(dx));        // знак полярности
                    powerX *= -1;
                    qDebug() << "rotate dx =" << dx << powerX;
                    mouse_move_rel(powerX, 0);
                }
            } else {
                powerX = 0;
                pickdownX = true;
                qDebug() << "x rotate complate";
            }
            rotate = true;
        }
        if(grad > -15 && grad < 15)
            if(!pickdownX) {
                powerX *= -1;
                mouse_move_rel(powerX, 0);
                pickdownX = true;
                qDebug() << "x rotate complate" << dx << powerX;
            }
        //  Выравнивание по оси Y
        if(!pickup && pickdownX) {
            if(approxim <= abs(dy) || !compass->face) {
                seekY = -SPEED_COMPAS_Y;
            } else {
                if(dy < -3) {
                    seekY = SPEED_COMPAS_Y;
                } else {
                    seekY = 0;
                    pickdownY = true;
                    qDebug() << "y complate" ;
                }
            }
            qDebug() << "move Y" << dy << seekY << " face=" << compass->face;
            mouse_move_rel(0, seekY);
            pickup = true;
        }
        if(compass->face && pickdownX) {
            if(dy > -3 && dy < approxim  ) {
                if(!pickdownY) {
                    qDebug() << "Делаем обратную силу";
                    seekY *= -1;
                    mouse_move_rel(0, seekY);
                    pickdownY = true;
                    qDebug() << "y complate" << dy << seekY << " face=" << compass->face;
                }
            }
        }
        if(pickdownX && pickdownY)
        {
            qDebug() << "success compass" ;
            push_key(" ");
            QThread::msleep(500);   // Ожидание завершения инерционных движений

            pickdownX = false;
            pickdownY = false;
            pickup = false;
            rotate = false;
            m_bCompass = true;
        }
    } else {
        CursorTarget *tar = capture->takeAimp();

        int dx = tar->pointCenter.x - tar->pointTarget.x;
        int dy = tar->pointCenter.y - tar->pointTarget.y;
//        static int powerX;
        static int seekY;
        static int seekX;

        static bool pickup = false;
        static bool pickdownX = false;
        static bool pickdownY = false;
        static bool speed0 = false;
        static bool speed1 = false;
        static bool rotate = false;
        static int count_error = 0;
        //                qDebug() << dx;
        //                double radius = sqrt(pow(dx,2) + pow(dy, 2));
        if(!tar->active) {
            qDebug() << "target not active err=" << count_error;
            if(count_error == 3) {
                qDebug() << "target RESET err=" << count_error;
                count_error = 0;
                push_key(" ");
                QThread::msleep(500);   // Ожидание завершения инерционных движений
                pickup = false;
                speed1 = false;
                speed0 = false;
                pickdownX = false;
                rotate = false;
                pickdownY = false;
//                m_pControl->check = false;
//                m_pControl->target = true;
//                m_pControl->state = AICONTROL;
                m_bCompass = false;
            }
            if(confirmTimer.elapsed() > 500 ) {
                count_error++;
                confirmTimer.restart();
            }
            return false;
        }
        count_error = 0;
        double radius = sqrt(pow(dx,2) + pow(dy, 2));
        /*                // Вычисление угла между двумя векторами
                        cv::Point a(tar->pointCenter.x - tar->pointTarget.x, tar->pointCenter.y - tar->pointTarget.y);
                        cv::Point b(0, 1);
                        double atan_a = atan2(a.x * b.y - b.x * a.y, a.x * b.x + a.y * b.y);
                        double grad = atan_a * 180. / M_PI;
                        qDebug() << "grad=" << grad << " radius=" << radius;


        #define ROTATE_SPEED_0  180
        #define ROTATE_SPEED_2  100
        #define ROTATE_SPEED_3  100
        #define ROTATE_SPEED_2_SEEK  45
        #define ROTATE_SPEED_3_SEEK  90
        #define ROTATE_SPEDD_DELTA  7
                        //  Выравнивание по ротации
                        if(!rotate) {
                            qDebug() << "radius =" << radius;
                                if(abs(dx) > 5) {
                                    if((grad > 3 || grad < -3)) {         // выровнять по градусам
                                        if(dx == 0)
                                            dx = 1;
                                        powerX = ROTATE_SPEED_0 * (dx / abs(dx));        // знак полярности
                                        powerX *= -1;
                                        qDebug() << "rotate dx =" << dx << powerX;
        //                                if(abs(grad) > 30) {
        //                                    powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_2);
        //                                    speed0 = true;
        //                                }
        //                                if(abs(grad) > 90) {
        //                                    powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_3);
        //                                    speed1 = true;
        //                                }
                                        move_mouse_rel(powerX , 0);
                                    }
                                } else {
                                    powerX = 0;
                                    pickdownX = true;
                                    qDebug() << "x rotate complate" << dx;
                                }
                                rotate = true;
                        } else {
                            if(powerX != 0 && !pickdownX) {
                                if(abs(grad) > ROTATE_SPEED_2_SEEK + ROTATE_SPEDD_DELTA && !speed0 ) {
                                    powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_2);
                                    speed0 = true;
                                    move_mouse_rel((powerX / abs(powerX)) * ROTATE_SPEED_2, 0);
                                    qDebug() << "UP rotate speed" << powerX;
                                }
                                if(abs(grad) > ROTATE_SPEED_3_SEEK + ROTATE_SPEDD_DELTA && !speed1) {
                                    powerX = (powerX / abs(powerX)) * (abs(powerX) + ROTATE_SPEED_3);
                                    speed1 = true;
                                    move_mouse_rel((powerX / abs(powerX)) * ROTATE_SPEED_3, 0);
                                    qDebug() << "UP rotate speed" << powerX;
                                }

                                if(abs(grad) < ROTATE_SPEED_2_SEEK - ROTATE_SPEDD_DELTA && speed0 ) {
                                    powerX = (powerX / abs(powerX)) * (abs(powerX) - ROTATE_SPEED_2 );
                                    move_mouse_rel(-((powerX / abs(powerX)) * ROTATE_SPEED_2), 0);
                                    speed0 = false;
                                    qDebug() << "DOWN rotate speed" << powerX;
                                }
                                if(abs(grad) < ROTATE_SPEED_3_SEEK - ROTATE_SPEDD_DELTA && speed1 ) {
                                    powerX = (powerX / abs(powerX)) * (abs(powerX) - ROTATE_SPEED_3);
                                    move_mouse_rel(-((powerX / abs(powerX)) * ROTATE_SPEED_3), 0);
                                    speed1 = false;
                                    qDebug() << "DOWN rotate speed" << powerX;
                                }
                            }

                            if(abs(grad) < 3) {
                                if(!pickdownX) {
                                    //                        move_mouse_rel(-powerX, 0);
                                    push_key(" ");
                                    pickdownX = true;
                                    qDebug() << "x rotate complate" << dx << powerX;
                                }
                            }

        */

        if(!pickup) {
            if(dy == 0)
                dy = 1;
            if(dx == 0)
                dx = 1;

            seekY = dy / abs(dy) * 150;
            seekY *= -1;
            seekX = dx / abs(dx);
            qDebug() << "START Y X move" << dy << seekY;
            mouse_move_rel(0, seekY);
            QThread::msleep(30);
            pickup = true;
            if(dx > 0) {
                press_key("a");
                QThread::msleep(30);
                qDebug() << "press_key(a)";
            } else if(dx < 0) {
                press_key("d");
                QThread::msleep(30);
                qDebug() << "press_key(d)";
            }

        }
        if(seekX > 0 && !pickdownX) {
            if(dx < 25)
            {
                release_key("a");
                QThread::msleep(30);
                pickdownX = true;
                qDebug() << "release_key(a)";
            }
        } else if(seekX < 0 && !pickdownX) {
            if(dx > -25)
            {
                release_key("d");
                pickdownX = true;
                QThread::msleep(30);
                qDebug() << "release_key(d)";
            }
        }


        if(seekY > 0 ) {
            if(dy > 0)
                if(!pickdownY) {
                    mouse_move_rel(0, -seekY);
                    pickdownY = true;
                }
        }
        else if(seekY <= 0 ){
            if(dy < 0)
                if(!pickdownY) {
                    mouse_move_rel(0, -seekY);
                    pickdownY = true;


                }
        }
        qDebug() << "dx =" << dx << " dy =" << dy << " radius =" << radius;


        if(pickdownX && pickdownY)
        {
            qDebug() << "success aiming" ;
            pickdownX = false;
            pickdownY = false;
            pickup = false;
            //                    speed0 = false;
            //                    speed1 = false;
            //                    rotate = false;
            return true;
        }

    }


    return false;
}

void ActionAimp::reset()
{
}
