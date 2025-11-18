#include "zombies.h"
#include "mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QMap>
#include "plant.h"

Zombies::Zombies(QString name, bool isStand, QGraphicsScene *scene, QObject *parent)
    : m_name(name), isStand(isStand)
{
    MainWindow::ZombiesNum++;
    setParent(parent);
    setToolTip("zombie");
    setZValue(93);
    scene->addItem(this);

    initZombieStats();
    bool readyToDelete = false;

    // 默认行走动画（使用统一的路径拼接规则）
    QString fileName = m_name + "Walk.gif";
    QString path = QString(":/image/%1/%2").arg(m_name).arg(fileName);

    setMovie(path);

    // 设置初始状态
    currentState = "Walk";
}
Zombies::~Zombies()
{
    MainWindow::ZombiesNum--;
    if (movie)
        delete movie;
}

void Zombies::initZombieStats()
{
    static const QMap<QString, int> health = {
        {"normalZombie", 10},
        {"bucketZombie", 15},// 铁桶僵尸血量15
        {"ConeZombie", 12},
        {"footballZombie", 12}
    };
    healthPoints = health.value(m_name, 10); // 默认10
}

void Zombies::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (movie)
        painter->drawPixmap(-23, 0, movie->currentPixmap());
}

QRectF Zombies::boundingRect() const
{
    if (m_name == "ConeZombie") {

        return QRectF(90, 60, 40, 80);
    }
    else if (m_name == "footballZombie"){
        return QRectF(15, 60, 40, 80);
    }
    else {
        // 普通僵尸、铁桶僵尸等
        return QRectF(55, 70, 40, 80);
    }
}

void Zombies::setMovie(const QString &path)
{
    if (movie) {
        delete movie;
        movie = nullptr;
    }

    movie = new QMovie(path);
    connect(movie, &QMovie::frameChanged, this, [=]() { update(); });
    movie->start();
}

/* ===========================
   行为逻辑
   =========================== */
void Zombies::doEating()
{
    if (!isEating && !isDead)
        isEating = true;
}

void Zombies::eatOver()
{
    if (isEating)
        isEating = false;

}

void Zombies::lessHealth(double damage)
{
    healthPoints -= damage;
    if (healthPoints <= 0)
        dead();
}


void Zombies::dead()
{
    if (!isDead) {
        isDead = true;
        isEating = false;
        emit zombieDied(this); // 发射僵尸死亡信号

        // 切换到死亡动画（你的 updateAnimation 会自动处理）
        updateAnimation();
    }
}

/* ===========================
   游戏刷新逻辑
   =========================== */
void Zombies::advance(int phase)
{
    if (readyToDelete) {
        if (scene()) scene()->removeItem(this);
        delete this;
        return;
    }

    if (isDead) {
        if (movie && movie->currentFrameNumber() == movie->frameCount() - 1) {
            readyToDelete = true;
        }
        return;
    }
    if (!phase)
        return;

    updateAnimation();

    if (isDead)
        return;

    QList<QGraphicsItem *> items = collidingItems();
    bool touchingPlant = false;

    for (QGraphicsItem *item : items)
    {
        if (item->type() == Plant::Type)
        {
            Plant *plant = dynamic_cast<Plant *>(item);

            if (plant && plant->hp > 0)
            {
                touchingPlant = true;
                doEating();

                // 攻击力：足球僵尸更强
                double damage = (m_name == "footballZombie") ? 1.5 : 1;
                plant->hp -= damage;
                break;
            }
        }
    }

    // 没有植物可吃就继续往前走
    if (!touchingPlant && !isStand)
    {
        eatOver();
        updateAnimation();

        // 移动速度：足球僵尸更快
        double speed = (m_name == "footballZombie") ? 1.0 : 0.5;
        setPos(x() - speed, y());
    }
}

void Zombies::updateAnimation()
{
    // 状态统一成首字母大写：Walk / Attack / Die
    QString newState;
    if (isDead)
        newState = "Die";
    else if (isEating)
        newState = "Attack";
    else
        newState = "Walk";

    if (newState == currentState)
        return;

    currentState = newState;


    QString fileName = m_name + newState + ".gif";

    QString path = QString(":/image/%1/%2")
                       .arg(m_name)
                       .arg(fileName);

    setMovie(path);
}
