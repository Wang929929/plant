#include "zombiePotato.h"
#include <QDebug>

zombiePotato::zombiePotato()
{
    hp = 5000;
    setMovie(":/image/plant/zombiePotato.png");
}

void zombiePotato::advance(int phase)
{
    if (!phase)
        return;
    update();
    if (hp <= 0)
        delete this;
}


