#ifndef POTATO_H
#define POTATO_H

#include "plant.h"

class Potato : public Plant{
public:
    Potato();
    void advance(int phase) override;

    // 新增：能量增强相关方法
    bool canBeEnhanced() const override { return true; }
    void enhanceWithEnergy() override;
    bool isEnhanced() const override { return enhanced; }

private:
    // 新增：能量增强相关成员
    bool enhanced;                 // 是否已被能量增强
    int originalHp;                // 原始生命值
    int enhancedHp;                // 增强后生命值
    QString originalGifPath;       // 原始GIF路径
    QString enhancedGifPath;       // 增强后GIF路径
};

#endif // POTATO_H
