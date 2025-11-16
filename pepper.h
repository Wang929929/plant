#ifndef PEPPER_H
#define PEPPER_H

#include "plant.h"
#include "fire.h"  // 包含新的火焰类
#include <QList>
#include <QGraphicsPixmapItem>
#include <QMovie>

class Pepper : public Plant {
public:
    Pepper();
    void advance(int phase) override;
    void explode();
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;

private:
    bool m_readyToDelete;
};

#endif // PEPPER_H
