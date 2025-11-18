#include "peashooter.h"
#include "pea.h"
#include "zombies.h"
#include <QGraphicsScene>
#include <QList>
#include <QDebug>

PeaShooter::PeaShooter()
    : shootInterval(2000), gameScene(nullptr)
{
    hp = 300;
    lastShootTime = QTime::currentTime();
    setMovie(":/image/plant/PeaShooter.gif");
    // qDebug() << "Peashooter created (default constructor)!";
}

bool PeaShooter::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const{
    Q_UNUSED(mode)
    QPointF peashooterPos = pos();
    int row = (int(peashooterPos.y()) - 81) / 98;
    QPointF zombiePos = other->pos();
    int zombieRow = (int(zombiePos.y()) - 20) / 100;
    return other->type() == Zombies::Type && zombieRow == row;
}

PeaShooter::PeaShooter(QGraphicsScene *scene, QGraphicsItem *parent)
    : Plant(), shootInterval(2000), gameScene(scene)
{
    hp = 300;
    lastShootTime = QTime::currentTime();
    setMovie(":/image/plant/PeaShooter.gif");
    // qDebug() << "Peashooter created with scene!";
}

void PeaShooter::advance(int phase)
{
    if(!phase) return; // 第一阶段不处理，只在第二阶段更新

    // 检查血量
    if(hp <= 0) {
        if(scene()) {
            scene()->removeItem(this);
        }
        delete this;
        return;
    }

    //射击逻辑
    QTime currentTime = QTime::currentTime();

    if(lastShootTime.msecsTo(currentTime) >= shootInterval) {
        shoot();
        lastShootTime = currentTime;
    }
}

void PeaShooter::shoot()
{
    // qDebug() << "Shoot function called!";

    if(!gameScene) {
        // qDebug() << "gameScene is null, trying to get from parent..."; //？？
        if(scene()) {
            gameScene = scene();
            // qDebug() << "Got scene from parent:" << gameScene; //？？
        } else {
            // qDebug() << "Cannot get scene from parent!";
            return;
        }
    }

    if(!gameScene) {
        // qDebug() << "gameScene is still null, cannot shoot!";
        return;
    }

    // 创建豌豆
    Pea* pea = new Pea();
    pea->setPos(x() + 20, y() - 30);

    // 豌豆添加到场景
    gameScene->addItem(pea);
    // qDebug() << "Peashooter successfully shot a pea!";
}


