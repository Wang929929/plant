#ifndef PLANT_H
#define PLANT_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QMovie>

class Plant : public QGraphicsItem{
public:
    int hp;
    int state;
    enum { Type = UserType + 1 };//植物类型
    Plant();
    ~Plant() override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const override;
    int type() const override;
    void setMovie(QString path);

    // 能量增强相关方法
    virtual bool canBeEnhanced() const { return false; }  // 默认植物不能被增强
    virtual void enhanceWithEnergy() { qDebug() << "This plant cannot be enhanced."; }
    virtual bool isEnhanced() const { return false; }

protected:
    QMovie *movie;
    int atk;
    int counter;
    int time;
};

#endif // PLANT_H
