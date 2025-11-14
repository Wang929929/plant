#ifndef FIREITEM_H
#define FIREITEM_H

#include <QGraphicsItem>
#include <QMovie>

class FireItem : public QGraphicsItem {
public:
    FireItem(QGraphicsItem* parent = nullptr);
    ~FireItem();

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void advance(int phase) override;  // 使用advance来更新动画

    void startAnimation();
    void stopAnimation();

private:
    QMovie* m_movie;
    QPixmap m_currentPixmap;
    bool m_animating;
};

#endif // FIREITEM_H
