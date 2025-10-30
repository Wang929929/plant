#include "map.h"

Map::Map(){
    dragOver=false;
    setAcceptDrops(true);
}

//植物矩阵边界判定，需要根据地图大小调整
QRectF Map::boundingRect() const{
    return QRectF(-369, -235, 738, 470);
}

//空函数paint，避免报错，可根据后续时间和需要进一步优化
void Map::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

void Map::dragEnterEvent(QGraphicsSceneDragDropEvent *event){
    if (event -> mimeData() -> hasText()){
        event -> setAccepted(true);
        dragOver = true;
        update();
    }
    else event -> setAccepted(false);
}

void Map::dragLeaveEvent(QGraphicsSceneDragDropEvent *event){
    Q_UNUSED(event);
    dragOver = false;
    update();
}

void Map::dropEvent(QGraphicsSceneDragDropEvent *event){
    dragOver = false;
    //网格坐标对齐，需要根据图片和网格大小调整数据
    if (event -> mimeData() -> hasText()){
        QString s = event -> mimeData() -> text();
        QPointF pos = mapToScene(event -> pos());
        pos.setX((int(pos.x()) - 249) / 82 * 82 + 290);
        pos.setY((int(pos.y()) - 81) / 98 * 98 + 130);

        //商店查找，具体坐标需要依据情况而定
        Shop *shop = qgraphicsitem_cast<Shop *>(scene()->items(QPointF(300, 15))[0]);
        shop->addPlant(s, pos); //shop的植物添加方法
    }

}
