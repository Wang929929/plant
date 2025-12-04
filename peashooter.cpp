#include "peashooter.h"
#include "pea.h"
#include "zombies.h"
#include <QGraphicsScene>
#include <QList>
#include <QDebug>

PeaShooter::PeaShooter()
    : shootInterval(2000), gameScene(nullptr),
    enhanced(false), originalShootInterval(2000), enhancedShootInterval(1000)  // 增强后射速加倍
{
    hp = 300;
    lastShootTime = QTime::currentTime();

    // 设置GIF路径
    originalGifPath = ":/image/plant/PeaShooter.gif";
    enhancedGifPath = ":/image/plant/PeaShooterEnhanced.gif";

    // 设置初始动画
    setMovie(originalGifPath);
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
    : Plant(), shootInterval(2000), gameScene(scene),
    enhanced(false), originalShootInterval(2000), enhancedShootInterval(1000)
{
    hp = 300;
    lastShootTime = QTime::currentTime();

    // 设置GIF路径
    originalGifPath = ":/image/plant/PeaShooter.gif";
    enhancedGifPath = ":/image/plant/PeaShooterEnhanced.gif";

    // 设置初始动画
    setMovie(originalGifPath);

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
    if (!collidingItems().isEmpty()){
        if(lastShootTime.msecsTo(currentTime) >= shootInterval) {
            shoot();
            lastShootTime = currentTime;
        }
    }
}

void PeaShooter::shoot()
{
    if(!gameScene) {
        if(scene()) {
            gameScene = scene();
        } else {
            return;
        }
    }

    // 创建豌豆
    Pea* pea = new Pea();
    pea->setPos(x() + 20, y() - 30);

    // 豌豆添加到场景
    gameScene->addItem(pea);
}

//新增：能量增强方法-射速加倍
void PeaShooter::enhanceWithEnergy()
{
    if (enhanced) {
        return;
    }

    shootInterval = enhancedShootInterval;  // 应用增强效果-射速加倍
    setMovie(enhancedGifPath);  // 更换GIF动画
    enhanced = true;
}
