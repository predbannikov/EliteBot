#include "aimpflyaround.h"


#define SPEED_COMPAS_Y      230
#define SPEED_COMPAS_ROT    210



AimpFlyAround::AimpFlyAround(CaptureWindow *aCapture, SocketIO *aSock) : BaseAction(aCapture, aSock)
{
    m_sActionName = "AIMPFLYAROUND";
}

bool AimpFlyAround::moveAway(Primitives *aPrim)
{
    switch(moveAwayTrans) {
    case MOVEAWAY_TRANS_1:
        if(foundPrimitive(aPrim)) {
            qDebug() << "Тут есть птимитивы";
            if(mapSide == 1) {
                key = "d";
            } else {
                key = "a";
            }
            qDebug() << "will be press" << key;
            press_key(key);
            moveAwayTrans = MOVEAWAY_TRANS_2;
        } else {
            qDebug() << "this all clear";
            return true;
        }
        break;
    case MOVEAWAY_TRANS_2:
        if(!aPrim->found) {
            qDebug() << "Вышли из зоны примитивов";
            moveAwayTrans = MOVEAWAY_TRANS_1;
            release_key(key);
            press_key("w");
            QThread::msleep(5000);
            release_key("w");
            mapSide = 0;
            offSet = true;
            return true;
        }
        break;
    }
    return false;
}

#define COMPARRADIUS_FOR_COMPAS     5
#define COMPARGRAD_FOR_COMPAS       10



bool AimpFlyAround::compassAimp()
{
    Compass *compass = capture->compass();
//    Primitives *prim = capture->test(mapSide);
    // Вычисление угла между двумя векторами
    cv::Point a(compass->pCenter.x - compass->pDot.x, compass->pCenter.y - compass->pDot.y);
    cv::Point b(0, 1);
    int dx = compass->pCenter.x - compass->pDot.x;
    int dy = compass->pCenter.y - compass->pDot.y;
    double radius = sqrt(pow(dx,2) + pow(dy, 2));

    double atan_a = atan2(a.x * b.y - b.x * a.y, a.x * b.x + a.y * b.y);
    double grad = atan_a * 180. / M_PI;
//    qDebug() << "grad=" << grad << " radius=" << radius;

    if(!compass->active) {
        if(timer.elapsed() > waitMSec) {
            waitCompasActive = true;
        }
        return false;
    }
    timer.restart();
    const int approxim = COMPARRADIUS_FOR_COMPAS;            // Расстояние до центра до которого компас реагирует
    //  Выравнивание по ротации
    if(!rotate) {
        qDebug() << "Rotate: radius =" << radius;
        if(approxim <= radius) {
            if((grad > COMPARGRAD_FOR_COMPAS || grad < -COMPARGRAD_FOR_COMPAS)) {         // выровнять по градусам
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
            qDebug() << "x rotate complate" << dx << powerX;
        }
        rotate = true;
    }
    if(grad > -COMPARGRAD_FOR_COMPAS && grad < COMPARGRAD_FOR_COMPAS)
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
            if(dy < -1) {
                seekY = SPEED_COMPAS_Y;
            } else {
                seekY = 0;
                pickdownY = true;
                qDebug() << "y move complate" ;
            }
        }
        qDebug() << "move Y" << dy << seekY << " face=" << compass->face;
        mouse_move_rel(0, seekY);
        pickup = true;
    }
    if(compass->face && pickdownX) {
        if(dy > -3 && dy < approxim  ) {            // -3 кажется для того чтоб компас реагировал когда точка снизу близка к центру, во избежания случая когда не видно таргета
            if(!pickdownY) {
                qDebug() << "Делаем обратную силу";
                seekY *= -1;
                mouse_move_rel(0, seekY);
                pickdownY = true;
                qDebug() << "y move complate" << dy << seekY << " face=" << compass->face;
            }
        }
    }
    if(pickdownX && pickdownY)
    {
        qDebug() << "success compass" ;
        push_key(" ");
        QThread::msleep(700);   // Ожидание завершения инерционных движений

        pickdownX = false;
        pickdownY = false;
        pickup = false;
        rotate = false;
        return true;
    }
    return false;
}


bool AimpFlyAround::aimpSideWays()
{
    int sign = 0;
    Compass *compass = capture->compass();
    Primitives *prim = capture->test(mapSide);
    cv::Point a(compass->pCenter.x - compass->pDot.x, compass->pCenter.y - compass->pDot.y);
    cv::Point b(0, 1);
    int dx = compass->pCenter.x - compass->pDot.x;
    int dy = compass->pCenter.y - compass->pDot.y;
    double radius = sqrt(pow(dx,2) + pow(dy, 2));
//    double atan_a = atan2(a.x * b.y - b.x * a.y, a.x * b.x + a.y * b.y);
//    double grad = atan_a * 180. / M_PI;
//    qDebug() << "grad=" << grad << " radius=" << radius;
    switch (aimpSideWaysTrans) {
    case AIMPSIDEWAY_TRANS_1:
        if(moveAway(prim)) {
            if(offSet) {
                qDebug() << "Переключаемся на следующее состояние AIMPSIDEWAY_TRANS_2";
                aimpSideWaysTrans = AIMPSIDEWAY_TRANS_2;
            } else {
                qDebug() << "Вчё чисто выходим";
                return true;
            }
        }
        break;
    case AIMPSIDEWAY_TRANS_2:
        sign = dx / abs(dx);
        if(sign > 0) {
            key = "a";
        } else {
            key = "d";
        }
        qDebug() << "Определили куда нам поворачивать нажимаем" << key << " radius =" << radius << " dx:dy =" << dx << ":" << dy;
        press_key(key);
        aimpSideWaysTrans = AIMPSIDEWAY_TRANS_3;

        break;
    case AIMPSIDEWAY_TRANS_3:
        qDebug() << "Поворачиваем до события radius =" << radius << " prim =" << prim->found;
        if(prim->found) {
            release_key(key);
            if(sSide == "left")
                key = "d";
            else
                key = "a";
            press_key(key);
            aimpSideWaysTrans = AIMPSIDEWAY_TRANS_4;
        }
        if(abs(radius) <= COMPARRADIUS_FOR_COMPAS) {
            qDebug() << "Пришли к цели ожидания radius =" << radius << " prim =" << prim->found << " отпускаем клавишу" << key;
            release_key(key);
            push_key(" ");
            QThread::msleep(2000);
            aimpSideWaysTrans = AIMPSIDEWAY_TRANS_1;
            return true;
        }
        if(abs(lastRadius) < (abs(radius) - 3)) {
            qDebug() << "Требуется поправка, метка остановки пропущена, TEST!!! radius ="
                     << QString::number(radius, 'f', 2) << " lastRadius =" << lastRadius;
            push_key("x");
            reset();
        }
        lastRadius = radius;
        break;
    case AIMPSIDEWAY_TRANS_4:
        if(!prim->found) {
            release_key(key);
            push_key(" ");
            QThread::msleep(2000);
            aimpSideWaysTrans = AIMPSIDEWAY_TRANS_2;
        }
        break;
    }
    return false;
}

bool AimpFlyAround::aimpTarget()
{
    CursorTarget *tar = capture->takeAimp();

    int dx = tar->pointCenter.x - tar->pointTarget.x;
    int dy = tar->pointCenter.y - tar->pointTarget.y;
    //                double radius = sqrt(pow(dx,2) + pow(dy, 2));
    if(!tar->active) {
        qint64 timerConfirmElapse = confirmTimer.elapsed();
        qDebug() << "target not active ms =" << timerConfirmElapse;
        if(confirmTimer.elapsed() > waitMSec) {
            reset();
        }
        return false;
    }
    confirmTimer.restart();
//    double radius = sqrt(pow(dx,2) + pow(dy, 2));
    if(!pickup) {
        if(dy == 0)
            dy = 1;
        if(dx == 0)
            dx = 1;
        seekY = dy / abs(dy) * 150;
        seekY *= -1;
        seekX = dx / abs(dx);
        qDebug() << "START Y  X move" << dy << seekY;
        mouse_move_rel(0, seekY);
        pickup = true;
        if(dx > 0) {
            press_key("a");
        } else if(dx < 0) {
            press_key("d");
        }
    }
    if(seekX > 0 && !pickdownX) {
        if(dx < 25)
        {
            release_key("a");
            pickdownX = true;
            qDebug() << "X move complate dx =" << dx << " dy =" << dy;
        }
    } else if(seekX < 0 && !pickdownX) {
        if(dx > -25)
        {
            release_key("d");
            pickdownX = true;
            qDebug() << "X move complate dx =" << dx << " dy =" << dy;
        }
    }
    if(seekY > 0 ) {
        if(dy > 0)
            if(!pickdownY) {
                mouse_move_rel(0, -seekY);
                pickdownY = true;
                qDebug() << "Y move complate dy =" << dy << " dx =" << dx;
            }
    }
    else if(seekY <= 0 ){
        if(dy < 0)
            if(!pickdownY) {
                mouse_move_rel(0, -seekY);
                pickdownY = true;
                qDebug() << "Y move complate dy =" << dy << " dx =" << dx;
            }
    }
//    qDebug() << "dx =" << dx << " dy =" << dy << " radius =" << radius;
    if(pickdownX && pickdownY)
    {
        qDebug() << "success aiming dy =" << dy << " dx =" << dx;
        return true;
    }
    return false;
}

bool AimpFlyAround::foundPrimitive(Primitives *aPrim)
{
    if(aPrim->found) {
        if(aPrim->area > 20000) {
            if(aPrim->rect.x < aPrim->screen.width - (aPrim->rect.x + aPrim->rect.width)) {
                mapSide = 1;
                sSide = "left";
            } else {
                sSide = "right";
                mapSide = 3;
            }
            qDebug() << sSide;
            push_key(" ");
            QThread::msleep(1500);
            return true;
        }
    }
    return false;
}

void AimpFlyAround::init(QStringList &asListParam)
{
    moveAwayTrans = MOVEAWAY_TRANS_1;
    aimpSideWaysTrans = AIMPSIDEWAY_TRANS_1;
    trans = TRANS_1;
    waitCompasActive = false;
    offSet = false;
    pickup = false;
    pickdownX = false;
    pickdownY = false ;
    rotate = false ;
    mapSide = 0;
    waitMSec = 6000;
    lastRadius = 0;
}

bool AimpFlyAround::logic(QStringList &asListParam)
{
    if(waitCompasActive) {
        qDebug() << "Компас не найден";
        push_key("x");
        push_key(" ");
        asListParam[1] = "1";
        asListParam[2] = "compas not found";
        return true;
    }

    switch (trans) {
    case TRANS_1:
        if(compassAimp()) {
            trans = TRANS_2;
        }
        break;
    case TRANS_2:
        if(aimpSideWays()) {
            trans = TRANS_3;
            confirmTimer.restart();
            qDebug() << "Закончили последнюю стадию прицеливания";
        }
        break;
    case TRANS_3:
        if(aimpTarget()) {
            qDebug() << "Закончили последнюю стадию прицеливания";
            trans = TRANS_1;
            return true;
        }
        break;
    }
    return false;
}

void AimpFlyAround::reset()
{
    qDebug() << "RESET";
    push_key(" ");
    QThread::msleep(2000);   // Ожидание завершения инерционных движений
    init(sys_listCommand);
}
