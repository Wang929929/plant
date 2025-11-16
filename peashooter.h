#ifndef PEASHOOTER_H
#define PEASHOOTER_H

#include "plant.h"
#include <QTime>

class Pea;
class QGraphicsScene;

class PeaShooter : public Plant
{
public:
    PeaShooter();
    PeaShooter(QGraphicsScene* scene, QGraphicsItem* parent = nullptr);
    ~PeaShooter() = default;

    void advance(int phase) override;

private:
    void shoot(); // 发射豌豆

    int shootInterval;      // 射击间隔
    QGraphicsScene* gameScene; // 游戏场景指针
    QTime lastShootTime;    // 记录上次射击的时间
};

#endif // PEASHOOTER_H
