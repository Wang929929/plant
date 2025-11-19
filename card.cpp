#include "card.h"
#include "shop.h"

const QMap<QString, int> Card::index = {{"SunFlower", 0}, {"Peashooter", 1}, {"Potato", 2}, {"Pepper", 3}};
const QVector<QString> Card::name = {"SunFlower", "Peashooter", "Potato", "Pepper"};
const QVector<int> Card::cost = {50, 100, 50, 125};
const QVector<int> Card::cool = {227, 227, 227, 227};

Card::Card(QString s){
    text = s;
    counter = 0;
}

QRectF Card::boundingRect() const
{
    return QRectF(-50, -30, 100, 60);
}

void Card::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->scale(0.6, 0.58);
    painter->drawPixmap(QRect(-50, -70, 100, 140), QPixmap(":/image/cards/Card.png"));
    painter->drawPixmap(QRect(-35, -42, 70, 70), QPixmap(":/image/cards/" + text + ".png"));
    QFont font;
    font.setPointSizeF(15);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(-30, 60, QString("%1").arg(cost[index[text]], 3));
    if (counter < cool[index[text]])
    {
        QBrush brush(QColor(0, 0, 0, 100));
        painter->setBrush(brush);

        painter->drawRect(QRectF(-48, -68, 98, 132 * (1 - qreal(counter) / cool[index[text]])));
    }
}

void Card::advance(int phase)
{
    if (!phase)
        return;
    update();
    if (counter < cool[index[text]])
        ++counter;
}

void Card::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    if (counter < cool[index[text]])
        event->setAccepted(false);
    Shop *shop = qgraphicsitem_cast<Shop *>(scene()->items(QPointF(300, 15))[0]);
    if (shop->getSun() < cost[index[text]])
        event->setAccepted(false);
    setCursor(Qt::ArrowCursor); //光标形状设置
}

void Card::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length()
        < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(event -> widget());
    QMimeData *mime = new QMimeData;
    QImage image(":/image/cards/" + text + ".png");
    mime->setText(text);
    mime->setImageData(image);
    drag->setMimeData(mime);
    drag->setPixmap(QPixmap::fromImage(image));
    drag->setHotSpot(QPoint(35, 35));
    drag->exec();
    setCursor(Qt::ArrowCursor);
}

void Card::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    setCursor(Qt::ArrowCursor);
}
