#include "pepper.h"
#include "zombies.h"
#include <QDebug>
#include <QGraphicsScene>

Pepper::Pepper() {
    hp = 300;
    setMovie(":/image/plant/Pepper.gif");
    m_readyToDelete = false;
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
    int pepperCol = (int(pepperPos.x()) - 290) / 82;

    if (!scene()) {
        qDebug() << "Pepper: No scene available for explosion";
        return;
    }

    for (int i = 0; i < 9; i++) {
        FireItem* fire = new FireItem();

        int fireX = pepperPos.x() + (i - pepperCol) * 82;  // 从辣椒位置开始
        int fireY = pepperPos.y() - 98;

        fire->setPos(fireX, fireY);
        fire->setZValue(10);
        fire->startAnimation();

        scene()->addItem(fire);
        // 不需要再存储火焰，因为火焰会自己删除
    }

    // 消灭同一行的僵尸
    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : items) {
        if (!item || !item->scene()) {
            continue;
        }
        Zombies* zombie = dynamic_cast<Zombies*>(item);
        if (zombie) {
            QPointF zombiePos = zombie->pos();
            int zombieRow = (int(zombiePos.y()) - 20) / 100;
            if (zombieRow == row) {
                zombie->dead();
            }
        }
    }

    // 僵尸已被杀死，设置为可以删除
    m_readyToDelete = true;
}

// 由于火焰现在会自己删除，不再需要这个方法
// void Pepper::removeAllFires() {
//     // 方法内容已删除
// }
