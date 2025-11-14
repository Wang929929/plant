#include "fire.h"
#include <QPainter>
#include <QGraphicsScene>

FireItem::FireItem(QGraphicsItem* parent)
    : QGraphicsItem(parent),
    m_movie(new QMovie(":/image/plant/Fire.gif")),
    m_animating(false) {

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
}

FireItem::~FireItem() {
    stopAnimation();
    delete m_movie;
}

QRectF FireItem::boundingRect() const {
    if (m_movie && m_movie->isValid()) {
        return QRectF(0, 0, m_movie->frameRect().width(), m_movie->frameRect().height());
    }
    return QRectF(0, 0, 80, 80); // 默认大小
}

void FireItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_currentPixmap.isNull()) {
        painter->drawPixmap(0, 0, m_currentPixmap);
    }
}

void FireItem::advance(int phase) {
    if (!phase || !m_animating)
        return;

    if (m_movie && m_movie->isValid()) {
        m_currentPixmap = m_movie->currentPixmap();
        update(); // 触发重绘

        // 检查动画是否结束
        if (m_movie->currentFrameNumber() == m_movie->frameCount() - 1) {
            // 动画结束，删除自己
            if (scene()) {
                scene()->removeItem(this);
                delete this;
                return;
            }
        }
    }
}

void FireItem::startAnimation() {
    if (m_movie && m_movie->isValid()) {
        m_movie->start();
        m_animating = true;
    }
}

void FireItem::stopAnimation() {
    m_animating = false;
    if (m_movie) {
        m_movie->stop();
    }
}
