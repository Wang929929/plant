#include "zombies.h"
#include <QPainter>
#include <QMap>
#include <QDebug>
#include "shop.h"
#include "mainwindow.h"
Zombies::Zombies(QString name,bool isStand,QGraphicsScene * scene,QObject * parent)
{
    MainWindow::ZombiesNum ++;
    this->isDead = false;
    this->isDelete = false;
    this->setParent(parent);
    this->m_name = name;
    this->isStand = isStand;
    this->isEating = false;
    this->isLeft = false;

    //设置碰撞tip标签
    this->setToolTip("zombie");

    this->setCartoonPixmap();
    this->setZValue(93);
    scene->addItem(this);

    //强制刷新
    QTimer * update = new QTimer;
    connect(update,&QTimer::timeout,this,[=](){
        this->update();

    });
    update->start(60);
    if(!this->isStand)
    {
        this->goWalk();
        QMap<QString,int> speed = {{"normalZombie",79}};
        this->cartoonSpeed = speed.value(this->m_name);
    }
    else
    {
        this->cartoonSpeed = 86;
    }
    QMap<QString,int> eat = {{"normalZombie",12}};
    QMap<QString,int> healthMap = {{"normalZombie",10}};
    this->allEatcartoonNum = eat.value(this->m_name);
    this->health = healthMap.value(this->m_name);

    this->loadZombieCartoon();
}
Zombies::~Zombies()
{
    if(this->isStand)
    {
        MainWindow::ZombiesNum --;
    }
}
void Zombies::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(-23,0,this->zombiePixmap);


    //绘制碰撞
    //        painter->setPen(Qt::red); // 设置边框颜色
    //        painter->setBrush(Qt::NoBrush); // 不填充颜色
    //        painter->drawRect(boundingRect()); // 使用boundingRect()获取图形项的边界矩形并绘制
}

QRectF Zombies::boundingRect() const
{
    return QRectF(0,20,130,130);
}
void Zombies::setCartoonPixmap()
{
    QMap<QString,int> zombiePix;
    if(this->isStand)
    {
        zombiePix = {{"normalZombie",20}};
    }
    else
    {
        zombiePix = {{"normalZombie",20}};
    }
    this->allcantoonNum = zombiePix.value(this->m_name);
}
void Zombies::loadZombieCartoon()
{
    this->cartoonTimer = new QTimer(this);
    this->cartoonNum = 1;
    this->deadCartoonNum = 0;

    connect(cartoonTimer, &QTimer::timeout, this, [=]() {

        // 僵尸死亡动画逻辑
        if (this->isDead)
        {
            // 当死亡帧播放完（假设共 9 帧：0~8）
            if (this->deadCartoonNum > 8)
            {
                // MainWindow::ZombiesNum--;
                this->setPos(3000, 1); // 移出屏幕

                QTimer::singleShot(10000, [=]() {
                    if (this->isDelete)
                        return;
                    delete this;
                });
                return;
            }

            // 拼出资源路径，比如 ":/image/zombie/zombie_die/ZombieDie_3.png"
            this->zombiePixmap = QString(":/image/zombie/zombie_die/ZombieDie_%0.png")
                                     .arg(this->deadCartoonNum);

            this->deadCartoonNum++;
            this->update(); // 触发重绘
            return;
        }
        if (this->isEating)
        {
            QString path = QString(":/image/zombie_eat/ZombieAttack_%0.png")
            .arg(this->cartoonNum);
            this->zombiePixmap = path;
            this->cartoonNum++;

            if (this->cartoonNum > this->allcantoonNum)
                this->cartoonNum = 0;
            return;
        }
        if(this->isStand)
        {
            this->zombiePixmap = this->zombiePixmap + "stand/";
        }
        else
        {
            this->zombiePixmap = this->zombiePixmap + "walk/";
        }
        // 3️⃣ 僵尸行走动画
        QString path = QString(":/image/zombie/Zombie_%0.png")
                           .arg(this->cartoonNum);
        this->zombiePixmap = path;
        this->cartoonNum++;
        if(this->cartoonNum == this->allcantoonNum + 1)
        {
            this->cartoonNum = 1;
        }
    });
    cartoonTimer->start(this->cartoonSpeed);
}
void Zombies::goWalk()
{
    this->walkTimer = new QTimer(this);
    connect(walkTimer,&QTimer::timeout,this,[=](){
        this->setPos(this->x()-1, this->y());
    });
    walkTimer->start(126);

}
void Zombies::doEating(){
    //停止走路
    this->walkTimer->stop();
    this->isEating = true;
}
void Zombies::eatOver(){
    this->walkTimer->start(126);
    this->isEating = false;
}
void Zombies::lessHealth(){
    this->health --;
    if(this->health==0){
        this->dead();
    }
}
void Zombies::dead(){
    this->isEating = false;
    this->walkTimer->stop();
    this->isDead=true;
}



