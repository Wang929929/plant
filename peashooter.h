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
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;
    void advance(int phase) override;

    // 新增：能量增强相关方法
    bool canBeEnhanced() const override { return true; }
    void enhanceWithEnergy() override;
    bool isEnhanced() const override { return enhanced; }

private:
    void shoot(); // 发射豌豆
    int shootInterval; // 射击间隔
    QGraphicsScene* gameScene; // 游戏场景指针
    QTime lastShootTime;    // 记录上次射击的时间

    // 新增：能量增强相关成员
    bool enhanced;             // 是否已被能量增强
    int originalShootInterval; // 原始射击间隔
    int enhancedShootInterval; // 增强后的射击间隔
    QString originalGifPath;   // 原始GIF路径
    QString enhancedGifPath;   // 增强后的GIF路径
};

#endif // PEASHOOTER_H
