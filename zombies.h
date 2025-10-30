#ifndef ZOMBIES_H
#define ZOMBIES_H

#include <QObject>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QTimer>

class Zombies : public QGraphicsObject
{
    Q_OBJECT
public:
    Zombies(QString name,bool isStand,QGraphicsScene * scene,QObject * parents = nullptr);
    ~Zombies();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) override;
    QRectF boundingRect() const override;

    //设置僵尸动画
    void loadZombieCartoon();
    //设置僵尸动画图片数量
    void setCartoonPixmap();
    //僵尸行走
    void goWalk();
    //僵尸吃植物
    void doEating();
    //吃完植物
    void eatOver();
    //减少生命值
    void lessHealth();
    //死亡动画
    void dead();

    //是否死亡
    bool isDead;
private:
    QString m_name;
    bool isStand;
    //判断是否在吃植物
    bool isEating;
    //僵尸是否在最左边
    bool isLeft;
    //僵尸图片路径
    QString zombiePixmap;
    //僵尸移动速度
    int cartoonSpeed;
    //当前僵尸数量
    int cartoonNum;
    //总共僵尸数量（打完就赢）
    int allcantoonNum;
    //僵尸吃植物动画
    int allEatcartoonNum;
    //僵尸生命
    int health;


    //走路定时器
    QTimer * walkTimer;
    //已经死了
    bool isDelete;
    //记录死亡动画数量 当前数量
    int deadCartoonNum;
    //动画计时器
    QTimer * cartoonTimer;

signals:

};

#endif // ZOMBIES_H

