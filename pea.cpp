#include "pea.h"
#include "zombies.h"
#include <QGraphicsScene>
#include <QList>
#include <QDebug>

Pea::Pea(QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent), damage(10), speed(5)
{
    // 设置豌豆png
    QPixmap pixmap(":/image/plant/Pea.png");
    if(!pixmap.isNull()) {
        setPixmap(pixmap.scaled(20, 20, Qt::KeepAspectRatio));
    } else {
        // 否则创建占位符
        QPixmap placeholder(20, 20);
        placeholder.fill(Qt::yellow);
        setPixmap(placeholder);
    }
}

void Pea::advance(int phase)
{
    if(!phase) return;

    // 豌豆向右移动
    setPos(x() + speed, y());

    // 如果移出屏幕，删除豌豆
    if(x() > 1000) {
        if(scene()) {
            scene()->removeItem(this);
        }
        delete this;
        return;
    }

    // 检测碰撞
    QList<QGraphicsItem*> collidingItems = this->collidingItems();
    // qDebug() << "Pea colliding with" << collidingItems.size() << "items";

    for(QGraphicsItem* item : collidingItems) {
        Zombies* zombie = dynamic_cast<Zombies*>(item);
        if(zombie) {
            // qDebug() << "Pea hit zombie! Damage:" << damage;

            // 对僵尸造成伤害
            zombie->lessHealth(damage);

            // 删除豌豆
            if(scene()) {
                scene()->removeItem(this);
            }
            delete this;
            return;
        }
    }
}
