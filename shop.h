#ifndef SHOP_H
#define SHOP_H

#include <QString>
#include <QPointF>
#include <QGraphicsObject>
#include <QList>
#include <QGraphicsScene>
#include <QMap>
#include <QPixmap>

#include "plant.h"
#include "card.h"
#include "peashooter.h"
#include "sunflower.h"

// 向前声明，用于鼠标事件
class QGraphicsSceneMouseEvent;

class Shop : public QGraphicsObject{
    Q_OBJECT

public:
    Shop();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void addPlant(QString s, QPointF pos);
    void addSun(int amount);

private:
    // 辅助函数：根据点击的局部坐标返回卡片名称
    QString cardAt(const QPointF &pos) const;

    // ... (其他成员变量保持不变) ...
    int m_sun;
};

#endif // SHOP_H

