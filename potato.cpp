#include "potato.h"
#include <QDebug>

Potato::Potato()
{
    hp = 600;
    setMovie(":/image/plant/Potato.gif");
}

void Potato::advance(int phase)
{
    if (!phase)
        return;
    update();
    if (hp <= 0)
        delete this;
}
