#ifndef SUN_H
#define SUN_H

#include <QGraphicsItem>
#include <QRectF>
#include <QPointF>
#include <QMovie>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

class Sun : public QGraphicsItem{
public:
    Sun(QPointF pos);
    ~Sun() override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void advance(int phase) override;
private:
    QMovie *movie;
    QPointF position;
    int counter;
    int time;
};

#endif // SUN_H
