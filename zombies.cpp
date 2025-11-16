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

    // 默认行走动画
    if (m_name == "bucketZombie")
        setMovie(":/image/zombie/BucketZombieWalk.gif");
    if (m_name == "normalZombie")
        setMovie(":/image/zombie/ZombieWalk2.gif");
    if (m_name == "ConeZombie")
        setMovie(":/image/zombie/ConeZombieWalk.gif");
    if (m_name == "footballZombie")
        setMovie(":/image/zombie/FootballZombieWalk.gif");
    currentState = "walk";
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
    if (m_name == "coneZombie") {

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
        movie->stop();
        movie->deleteLater();
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
        QTimer::singleShot(5000, this, [=]() { delete this; }); // 延迟删除
    }
}

/* ===========================
   游戏刷新逻辑
   =========================== */
void Zombies::advance(int phase)
{
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
    QString newState;
    if (isDead)
        newState = "die";
    else if (isEating)
        newState = "eat";
    else
        newState = "walk";

    if (newState != currentState)
    {
        currentState = newState;
        if (m_name == "normalZombie"){
            if (newState == "die")
                setMovie(":/image/zombie/zombie_die/ZombieDie.gif");
            else if (newState == "eat")
                setMovie(":/image/zombie_eat/ZombieAttack.gif");
            else
                setMovie(":/image/zombie/ZombieWalk2.gif");}

        if (m_name == "bucketZombie"){

            if (newState == "die")
                setMovie(":/image/zombie/zombie_die/ZombieDie.gif");
            else if (newState == "eat")
                setMovie(":/image/zombie_eat/BucketZombieAttack.gif");
            else
                setMovie(":/image/zombie/BucketZombieWalk.gif");}
        if (m_name == "ConeZombie"){

            if (newState == "die")
                setMovie(":/image/zombie/zombie_die/ZombieDie.gif");
            else if (newState == "eat")
                setMovie(":/image/zombie_eat/ConeZombieAttack.gif");
            else
                setMovie(":/image/zombie/ConeZombieWalk.gif");}
        if (m_name == "footballZombie"){

            if (newState == "die")
                setMovie(":/image/zombie/zombie_die/FootballZombieDie.gif");
            else if (newState == "eat")
                setMovie(":/image/zombie_eat/FootballZombieAttack.gif");
            else
                setMovie(":/image/zombie/FootballZombieWalk.gif");}
    }
}
