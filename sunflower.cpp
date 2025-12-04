#include "sunflower.h"
#include "sun.h"
#include <QDebug>

SunFlower::SunFlower() : enhanced(false)
{
    hp = 300;

    // 修改：设置生产阳光间隔（增强前和增强后）。原本代码：time = int(10.0 * 1000 / 33);
    originalTime = int(10.0 * 1000 / 33);
    enhancedTime = int(5.0 * 1000 / 33);   // 增强后用时减半
    time = originalTime;

    // 修改：设置GIF路径和初始动画。原本代码：setMovie(":/image/plant/SunFlower.gif");
    originalGifPath = ":/image/plant/SunFlower.gif";
    enhancedGifPath = ":/image/plant/SunFlowerEnhanced.gif";
    setMovie(originalGifPath);
}

void SunFlower::advance(int phase)
{
    if (!phase)
        return;
    update();
    if (hp <= 0)
        delete this;
    else if (++counter >= time)
    {
        counter = 0;
        scene()->addItem(new Sun(pos()));
    }
}

// 新增：能量增强方法-生产阳光速度加倍
void SunFlower::enhanceWithEnergy()
{
    if (enhanced) {
        return;
    }

    time = enhancedTime;  // 应用增强效果-生产速度加倍
    setMovie(enhancedGifPath);  // 更换GIF动画
    enhanced = true;
}
