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

    // 新增：能量增强相关方法 - 标记辣椒不能被增强
    bool canBeEnhanced() const override { return false; }
    void enhanceWithEnergy() override {
        qDebug() << "Pepper cannot be enhanced! It's an explosive plant.";
    }
    bool isEnhanced() const override { return false; }

private:
    bool m_readyToDelete;
};

#endif // PEPPER_H
