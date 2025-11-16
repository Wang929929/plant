#ifndef ZOMBIES_H
#define ZOMBIES_H

#include <QObject>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QTimer>
#include <QPainter>
#include <QMovie>

class Zombies : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit Zombies(QString name, bool isStand, QGraphicsScene *scene, QObject *parent = nullptr);
    ~Zombies() override;

    // Qt 基础接口
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
    void advance(int phase) override;

    //新增：确保变成公共方法
public:
    bool isAlive()const{return !isDead && healthPoints > 0;}

    // 僵尸行为接口
    void doEating();     // 开始吃植物
    void eatOver();      // 停止吃植物
    void lessHealth(double damage);   // 受到伤害
    void dead();         // 死亡逻辑


    enum { Type = UserType + 2 };
    int type() const override { return Type; }

private:
    void initZombieStats();     // 初始化血量等基础数据
    void setMovie(const QString &path); // 切换动画
    void updateAnimation();     // 检测状态变化并更新动画

private:
    QString m_name;
    bool isStand = false;
    bool isDead = false;
    bool isDelete = false;
    bool isEating = false;
    bool isLeft = false;
    bool touchingPlant;

    int healthPoints = 10;
    QMovie *movie = nullptr;
    QString currentState = ""; // 当前动画状态（walk / eat / die）

signals:
    void zombieDied(Zombies *zombie);
};

#endif // ZOMBIES_H
