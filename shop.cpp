#include "shop.h"
#include "plant.h"
#include "card.h"
#include "peashooter.h"
#include "sunflower.h"
#include "potato.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QList>
#include <QGraphicsObject>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <QHash>



Shop::Shop() : m_sun(200){
    // 搭建图框(共8个）
    Card *card = nullptr;
    for (int i = 0; i < Card::name.size(); ++i)
    {
        card = new Card(Card::name[i]);
        card->setParentItem(this);
        card->setPos(70 + 65 * i, 50);
    }
    /*
    std::vector<std::string> card_names = {"Peashooter","Sunflower","Wallnut","PotatoMine","CherryBomb",
                     "SnowPea","Chomper","Repeater"};
    int max_num = sizeof(card_names);
*/


}

// boundingRect 和 paint 函数保持不变)
QRectF Shop::boundingRect() const{
    return QRectF(0, 0, 800, 100);
}

void Shop::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();

    // 绘制太阳币
    //QRectF sunIconRect(-70, 20, 70, 50);
    QRectF sunIconRect(-70, 0, 650, 100);// 太阳币图标的位置 (x, y, w, h)
    QRectF sunTextRect(-40, 60, 50, 50); // 太阳币文本的位置 (x, y, w, h)

    // 绘制图标
    QPixmap sunIcon(":/image/shop/Shop.png");
    if (!sunIcon.isNull()) {
        painter->drawPixmap(sunIconRect, sunIcon, sunIcon.rect());
    } else {
        // 如果图片加载失败，画一个黄色矩形作为备用
        painter->setBrush(Qt::yellow);
        painter->drawRect(sunIconRect);
    }
    // 绘制文本
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 16, QFont::Bold));
    painter->drawText(sunTextRect, Qt::AlignCenter, QString::number(m_sun));
    painter->restore();
}

// 添加植物
void Shop::addPlant(QString s, QPointF pos){
    if (!scene()) {
        qDebug() << "Planting failed: Shop has no scene.";
        return;
    }

    QList<QGraphicsItem *> items = scene()->items(pos);
    foreach (QGraphicsItem *item, items) {
        if (item->type() == Plant::Type) {
            qDebug() << "Planting failed: Spot occupied.";
            return;
        }
    }


    Plant *plant = nullptr;
    switch (Card::index[s]){
    case 0:
        plant = new SunFlower; break;
    case 1:
        plant = new Peashooter; break;
    case 2:
        plant = new Potato; break;
    }

    if (plant) {
        int cost = Card::cost[Card::index[s]];
        plant->setPos(pos);
        scene()->addItem(plant);
        m_sun -= cost;
        qDebug() << "Planted" << s << "for" << cost << "sun. Remaining:" << m_sun;

        QList<QGraphicsItem *> child = childItems();
        foreach (QGraphicsItem *item, child)
        {
            Card *card = qgraphicsitem_cast<Card *>(item);
            if (card->text == s)
                card->counter = 0;
        }

        update();

    } else {
        qDebug() << "Planting failed: Plant object creation failed for" << s;
    }
}

//增加太阳币
void Shop::addSun(int amount){
    m_sun += amount;
    qDebug() << "Added" << amount << "sun. Total:" << m_sun;
    update(); // 重绘
}

int Shop::getSun(){
    return m_sun;
}


