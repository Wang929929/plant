#ifndef ENERGY_H
#define ENERGY_H

#include <QGraphicsItem>
#include <QString>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QWidget>
#include <QPainter>

class Energy : public QGraphicsItem{
public:
    int counter;
    int cool;
    Energy();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void advance(int phase) override;
    void applyEnergyToItem(QPointF pos);  // 应用能量到植物

private:
    bool isDragging;  // 是否正在拖动
};

#endif // ENERGY_H
