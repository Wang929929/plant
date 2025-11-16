#include "pepper.h"
#include "zombies.h"
#include <QDebug>
#include <QGraphicsScene>

Pepper::Pepper() {
    hp = 300;
    setMovie(":/image/plant/Pepper.gif");
    m_readyToDelete = false;
}

bool Pepper::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    Q_UNUSED(mode)
    QPointF pepperPos = pos();
    int row = (int(pepperPos.y()) - 81) / 98;
    QPointF zombiePos = other->pos();
    int zombieRow = (int(zombiePos.y()) - 20) / 100;
    return state && other->type() == Zombies::Type && zombieRow == row;
}

void Pepper::advance(int phase) {
    if (!phase) return;
    if (m_readyToDelete) {
        // 在下一帧中安全删除
        if (scene()) {
            scene()->removeItem(this);
            delete this;  // 让Qt在安全的时候删除
        }
        return;
    }

    update();

    if (state == 0 && movie->currentFrameNumber() == movie->frameCount() - 1) {
        state = 1;
        explode();
    }
}

void Pepper::explode() {
    setMovie(":/image/plant/Boom.gif");

    QPointF pepperPos = pos();
    int row = (int(pepperPos.y()) - 81) / 98;
    int pepperCol = (int(pepperPos.x()) - 249) / 82;

    if (!scene()) {
        qDebug() << "Pepper: No scene available for explosion";
        return;
    }

    for (int i = 0; i < 9; i++) {
        FireItem* fire = new FireItem();

        int fireX = 208 + i * 82;
        int fireY = pepperPos.y() - 98;

        fire->setPos(fireX, fireY);
        fire->setZValue(10);
        fire->startAnimation();

        scene()->addItem(fire);
        // 不需要再存储火焰，因为火焰会自己删除
    }

    QList<QGraphicsItem *> items = collidingItems();
    foreach (QGraphicsItem *item, items)
    {
        Zombies *zombie = qgraphicsitem_cast<Zombies *>(item);
        zombie->dead();
    }

    // 僵尸已被杀死，设置为可以删除
    m_readyToDelete = true;
}

// 由于火焰现在会自己删除，不再需要这个方法
// void Pepper::removeAllFires() {
//     // 方法内容已删除
// }
