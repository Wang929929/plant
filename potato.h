#ifndef POTATO_H
#define POTATO_H

#include "plant.h"

class Potato : public Plant{
public:
    Potato();
    void advance(int phase) override;
};

#endif // POTATO_H
