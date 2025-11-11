#include "shovel.h"
#include "plant.h"

Shovel::Shovel(){}

QRectF Shovel::boundingRect() const {
    return QRectF(-40, -40, 80, 80);
}

void Shovel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter -> drawPixmap(QRect(-40, -40, 80, 80), QPixmap(":/image/shop/ShovelBank.png"));
    painter->drawPixmap(QRect(-35, -35, 70, 70), QPixmap(":/image/shop/Shovel.png"));
}

void Shovel::mousePressEvent(QGraphicsSceneMouseEvent *event){
    Q_UNUSED(event)
    setCursor(Qt::ArrowCursor);
}

void Shovel::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length()
        < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(event -> widget());
    QMimeData *mime = new QMimeData;
    QImage image(":/image/shop/Shovel.png");
    QImage scaledImage = image.scaled(
        image.width() * 0.5,
        image.height() * 0.5,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    mime->setText("Shovel");
    mime->setImageData(image);
    drag->setMimeData(mime);
    drag->setPixmap(QPixmap::fromImage(scaledImage));
    drag->setHotSpot(QPoint(35 * 0.5, 35 * 0.5));
    drag->exec();
    setCursor(Qt::ArrowCursor);
}

void Shovel::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    setCursor(Qt::ArrowCursor);
}

void Shovel::removePlant(QPointF pos){
    QList<QGraphicsItem *> items = scene()->items(pos);
    foreach (QGraphicsItem *item, items)
        if (item->type() == Plant::Type)
        {
            delete item;
            return;
        }
}
