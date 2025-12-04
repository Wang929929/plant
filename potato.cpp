#include "potato.h"
#include <QDebug>

Potato::Potato() : enhanced(false)
{
    // 修改：设置生命值（增强前和增强后）。原本代码：hp = 4000;
    originalHp = 4000;     // 原始生命值
    enhancedHp = 8000;     // 增强后生命值翻倍
    hp = originalHp;

    // 修改：设置GIF路径和初始动画。原本代码：setMovie(":/image/plant/Potato.gif");
    originalGifPath = ":/image/plant/Potato.gif";
    enhancedGifPath = ":/image/plant/PotatoEnhanced.gif";
    setMovie(originalGifPath);
}

void Potato::advance(int phase)
{
    if (!phase)
        return;
    update();
    if (hp <= 0)
        delete this;
}

// 新增：能量增强方法-生命值翻倍
void Potato::enhanceWithEnergy()
{
    if (enhanced) {
        return;
    }

    hp = enhancedHp;  // 应用增强效果-生命值翻倍
    setMovie(enhancedGifPath);  // 更换GIF动画
    enhanced = true;
}
