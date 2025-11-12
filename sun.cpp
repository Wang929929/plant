#include "sun.h"
#include "shop.h"

#include <QRandomGenerator>

Sun::Sun(QPointF pos){
    double randomX = QRandomGenerator::global()->bounded(-15, 15);
    double randomY = QRandomGenerator::global()->bounded(15, 45);
    position = QPointF(pos.x() + randomX, pos.y() + randomY);
    setPos(QPointF(position.x(), pos.y()));
    counter = 0;
    time = int(10.0 * 1000 / 33);
    movie = new QMovie(":/image/Sun.gif");
    movie->start();
    setAcceptedMouseButtons(Qt::LeftButton);
}

// 【谢嘉翔添加】: 随机掉落太阳的构造函数
Sun::Sun(){
    int randomX = QRandomGenerator::global()->bounded(150, 900);// X 坐标范围
    int randomY_dest = QRandomGenerator::global()->bounded(100, 500);// Y 坐标
    position = QPointF(randomX, randomY_dest);// 设置最终目标位置
    setPos(randomX, 0);// 设置初始位置（在屏幕顶端）

    // 初始化其他变量
    counter = 0;
    time = int(10.0 * 1000 / 33);
    movie = new QMovie(":/image/Sun.gif");
    movie->start();
    setAcceptedMouseButtons(Qt::LeftButton);
}

Sun::~Sun(){
    delete movie;
}

QRectF Sun::boundingRect() const
{
    return QRectF(-35, -35, 70, 70);
}

void Sun::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->drawImage(boundingRect(), movie->currentImage());
}

void Sun::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    Shop *shop = qgraphicsitem_cast<Shop *>(scene()->items(QPointF(300, 15))[0]);
    shop->addSun(25);
    scene()->removeItem(this);
    delete this;
    event -> setAccepted(true);
}

void Sun::advance(int phase)
{
    if (!phase)
        return;
    update();
    if (y() < position.y())
        setY(y() + 2);//谢嘉翔修改参数。原为30
}
