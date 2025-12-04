#ifndef SUNFLOWER_H
#define SUNFLOWER_H

#include "plant.h"

class SunFlower : public Plant{
public:
    SunFlower();
    void advance(int phase) override;

    // 新增：能量增强相关方法
    bool canBeEnhanced() const override { return true; }
    void enhanceWithEnergy() override;
    bool isEnhanced() const override { return enhanced; }

private:
    // 新增：能量增强相关成员
    bool enhanced;                 // 是否已被能量增强
    int originalTime;              // 原始生产阳光间隔
    int enhancedTime;              // 增强后生产阳光间隔
    QString originalGifPath;       // 原始GIF路径
    QString enhancedGifPath;       // 增强后的GIF路径
};

#endif // SUNFLOWER_H

