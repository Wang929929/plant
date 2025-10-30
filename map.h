#ifndef MAP_H
#define MAP_H

#include "plant.h"
#include "shop.h"
// #include "shovel.h" 如果有时间，完成植物移除功能

#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsItem>
#include <QPainter>
#include <QMimeData>
#include <QPointF>

class Map : public QGraphicsItem{
public:
    Map();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
private:
    bool dragOver;
    Shop *shop; //商店对象
};

#endif // MAP_H
