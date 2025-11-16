#ifndef PEA_H
#define PEA_H

#include <QGraphicsPixmapItem>

class Pea : public QGraphicsPixmapItem
{
public:
    Pea(QGraphicsItem* parent = nullptr);
    ~Pea() = default;

    void setDamage(int dmg) { damage = dmg; }
    int getDamage() const { return damage; }

    void advance(int phase) override;

private:
    int damage = 10;
    int speed = 5;
};

#endif // PEA_H
