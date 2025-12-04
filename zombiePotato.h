#ifndef ZOMBIEPOTATO_H
#define ZOMBIEPOTATO_H

#include "plant.h"

class zombiePotato : public Plant{
public:
    zombiePotato();
    void advance(int phase) override;

    // 新增：能量增强相关方法
    bool canBeEnhanced() const override { return false; }

private:
};

#endif // ZOMBIEPOTATO_H
