#include "energy.h"
#include "plant.h"
#include "zombies.h"
#include "shop.h"
#include <QDebug>

Energy::Energy() : isDragging(false){
    counter = 0;
    cool = 33;  //能量冷却用时
}

QRectF Energy::boundingRect() const {
    return QRectF(-40, -40, 80, 80);
}

void Energy::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    // 绘制能量背景框
    painter->drawPixmap(QRect(-30, -30, 80, 80), QPixmap(":/image/shop/ShovelBank.png")); // 使用铲子相同的背景框
    // 绘制能量图标
    painter->drawPixmap(QRect(-25, -25, 70, 70), QPixmap(":/image/shop/Energy.png"));
    if (counter < cool)
    {
        QBrush brush(QColor(0, 0, 0, 100));
        painter->setPen(Qt::NoPen);
        painter->setBrush(brush);
        painter->drawRect(QRectF(-30, -30, 80, 80 * (1 - qreal(counter) / cool)));
    }
}

void Energy::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if (counter < cool)
        event->setAccepted(false);
    else event -> accept();
    if (event->button() == Qt::LeftButton) {
        setCursor(Qt::ClosedHandCursor);
        isDragging = true;  // 接受事件，阻止传播
    } else {
        event->ignore();
    }
}

void Energy::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if (!isDragging) {
        event->ignore();
        return;
    }

    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length()
        < QApplication::startDragDistance()) {
        return;
    }

    QDrag *drag = new QDrag(event->widget());
    QMimeData *mime = new QMimeData;
    QImage image(":/image/shop/Energy.png");
    QImage scaledImage = image.scaled(
        image.width() * 0.5,
        image.height() * 0.5,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    mime->setText("Energy");
    mime->setImageData(image);
    drag->setMimeData(mime);
    drag->setPixmap(QPixmap::fromImage(scaledImage));
    drag->setHotSpot(QPoint(35 * 0.5, 35 * 0.5));
    drag->exec();

    setCursor(Qt::ArrowCursor);
    event->accept();  // 接受事件，阻止传播
}

void Energy::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (isDragging) {
        // 先标记事件为已接受，防止传播
        event->accept();
        isDragging = false;
    } else {
        event->ignore();
    }
    setCursor(Qt::ArrowCursor);
}

void Energy::advance(int phase){
    if (!phase)
        return;
    update();
    if (counter < cool)
        ++counter;
}

void Energy::applyEnergyToItem(QPointF pos){
    QList<QGraphicsItem *> items = scene()->items(pos);
    foreach (QGraphicsItem *item, items) {
        if (item->type() == Plant::Type) {
            Plant *plant = qgraphicsitem_cast<Plant *>(item);

            // 检查植物是否可以增强
            if (!plant->canBeEnhanced()) {
                qDebug() << "This plant cannot be enhanced!";
                return;
            }

            // 检查植物是否已经增强过
            if (plant->isEnhanced()) {
                qDebug() << "Plant already enhanced! Cannot enhance again.";
                return;
            }

            plant->enhanceWithEnergy();  // 调用植物的增强方法
            counter = 0;
            return;
        }
    }

    // 对僵尸使用能量
    QPointF zombiePos;
    zombiePos.setX(pos.x()-1);
    zombiePos.setY(pos.y());
    QList<QGraphicsItem *> zombieItems = scene()->items(zombiePos);
    foreach (QGraphicsItem *item, zombieItems) {
        if (item->type() == Zombies::Type){
            qDebug() << "Applying energy to zombie";
            Zombies* zombie = qgraphicsitem_cast<Zombies *>(item);
            zombie -> dead();
            QPointF correctPos;
            correctPos.setX((int(pos.x()) - 249) / 82 * 82 + 290);
            correctPos.setY((int(pos.y()) - 81) / 98 * 98 + 130);
            qDebug() << "Planting zombiepotato at" << correctPos;
            Shop *shop = qgraphicsitem_cast<Shop *>(scene()->items(QPointF(300, 15))[0]);
            shop->addPlant("zombiePotato", correctPos);
            return;
        }
    }
}

