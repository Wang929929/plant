#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shop.h"
#include "shovel.h"
#include "zombies.h" //新增
#include "audiomanager.h" //新增
#include <QVector>
#include <QRandomGenerator>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout> //新增垂直

#include "sun.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置计时器
    timer = new QTimer(this);

    //初始化场景
    scene = new QGraphicsScene(this);
    scene ->setSceneRect(150, 0, 1000,600);
    scene ->setItemIndexMethod(QGraphicsScene::NoIndex); //?？？无索引机制

    //初始化视图
    view = new QGraphicsView(scene, this);
    view->resize(1002,602);
    //初始化背景
    view->setRenderHint(QPainter::Antialiasing); // ???开启渲染抗锯齿
    QPixmap background(":/image/Background.jpg");
    if (background.isNull()) qWarning() << "没有找到图片，请检查图片路径";
    background = background.scaled(1000, 600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    view->setBackgroundBrush(background);
    view->setCacheMode(QGraphicsView::CacheBackground); // 开启背景缓存
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate); // 局部视图更新
    setCentralWidget(view);
    //初始化map
    Map *map = new Map;
    map->setPos(618, 326);
    scene->addItem(map);
    //初始化shop
    Shop *shop = new Shop;
    shop->setPos(300, 15);
    scene->addItem(shop);
    //初始化shovel
    Shovel *shovel = new Shovel;
    shovel->setPos(825, 65);
    scene->addItem(shovel);
    // 【谢嘉翔添加】: 初始化并启动太阳生成定时器
    sunSpawnTimer = new QTimer(this);
    connect(sunSpawnTimer, &QTimer::timeout, this, &MainWindow::spawnSun);
    sunSpawnTimer->start(10000); // 每 10 秒掉落一个

    //创建按钮
    muteButton = new QPushButton("Mute", this);
    pauseButton = new QPushButton("Pause", this);

    // 设置按钮位置和样式（根据界面调整位置）
    muteButton->setGeometry(411, 552, 80, 30);
    pauseButton->setGeometry(511, 552, 80, 30);

    // 设置按钮样式
    muteButton->setStyleSheet("QPushButton { background-color: lightblue; font-size: 14px; }");
    pauseButton->setStyleSheet("QPushButton { background-color: lightgreen; font-size: 14px; }");

    // 连接信号和槽
    connect(muteButton, &QPushButton::clicked, this, &MainWindow::toggleMute);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::togglePause);

    // 初始化状态变量
    isMuted = false;
    isPaused = false;
    gameOver = false;

    // 创建游戏状态检查定时器
    gameStateTimer = new QTimer(this);
    connect(gameStateTimer, &QTimer::timeout, this, &MainWindow::checkGameState);
    gameStateTimer->start(100); // 每100ms检查一次游戏状态


    // 创建音量按钮
    volumeButton = new QPushButton("Volume", this);
    volumeButton->setGeometry(611, 552, 80, 30);  // 放在其他按钮右边
    volumeButton->setStyleSheet("QPushButton { background-color: #FFA500; font-size: 14px; color: white; }");

    // 连接音量按钮信号
    connect(volumeButton, &QPushButton::clicked, this, &MainWindow::showVolumeMenu);

    // 初始化音频管理器
    audioManager = AudioManager::instance();
    audioManager->playBackgroundMusic();  // 开始播放背景音乐
    audioManager->setVolume(50);          // 设置初始音量50%

    timer -> start(33);
    view->show();
    //this->showBeginStandZombies();

    connect(timer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    // // 立即生成几个用于测试的僵尸（可删）!!!!!如果要测试就把下面的commend解除掉
    //for (int i = 0; i < 3; ++i)
    //outZombies();

    // 自动生成僵尸计时器（每 5 秒生成一个）用来测试的，不用管这个
    QTimer *spawnTimer = new QTimer(this);
    connect(spawnTimer, &QTimer::timeout, this, [this]() {
        outZombies();
    });
    spawnTimer->start(5000); // 每5秒生成一个新僵尸
}

void MainWindow::showBeginStandZombies()//开场动画，让我们种植物的时候僵尸出现的的
{
    static const QVector<QPointF> zombiePositions = {
        {900, 20}, {900, 120}, {900, 220},
        {900, 320}, {930, 320}, {930, 120}, {900, 420}
    };

    for (const QPointF &pos : zombiePositions)
    {
        Zombies *zombie = new Zombies("normalZombie", true, scene, this);
        connect(zombie, &Zombies::zombieDied, this, &MainWindow::handleZombieDied);
        zombie->setPos(pos);
        zombiesVector.append(zombie);
    }

    qDebug() << "Initialized" << zombiesVector.size() << "stand zombies.";
}

void MainWindow::deleteBeginZombie()//正式开始的时候调用这个，删掉开场僵尸
{
    for (QVector<Zombies *>::iterator it = this->zombiesVector.begin();
         it != this->zombiesVector.end();
         it++)
    {
        delete *it;
    }
}
void MainWindow::outZombies()//僵尸随机出现，游戏开始
{
    if (gameOver || isPaused)
        return;

    // 最大僵尸数量
    if (ZombiesNum >= 40)
        return;

    // 以概率生成，降低出现频率
    if (QRandomGenerator::global()->bounded(100) < 20) // 50% 概率不生成
        return;

    // 随机生成普通僵尸或铁桶僵尸
    QString type;
    int randValue = QRandomGenerator::global()->bounded(100); // 0~99
    if (randValue < 70)             // 0~69 → 普通僵尸（50%）
        type = "normalZombie";
    else if (randValue < 85)        // 60~84 → 路障僵尸（30%）
        type = "ConeZombie";
    else if (randValue < 95)                          // 85~94 → 铁桶僵尸（20%）
        type = "bucketZombie";
    else
        type ="footballZombie";

    // 生成随机行走僵尸
    int randomY = QRandomGenerator::global()->bounded(5) * 100 + 20;
    Zombies *zombie = new Zombies(type, false, scene, this);
    connect(zombie, &Zombies::zombieDied, this, &MainWindow::handleZombieDied);
    zombie->setPos(900, randomY);
    zombiesVector.append(zombie);

    qDebug() << "Spawned" << type << "at y =" << randomY << ", total zombies:" << ZombiesNum;
}

// 在析构函数后实现功能
// 静音/取消静音功能
void MainWindow::toggleMute()
{
    isMuted = !isMuted;

    if (isMuted) {
        muteButton->setText("Unmute");
        muteButton->setStyleSheet("QPushButton { background-color: orange; font-size: 14px; }");
        // 这里添加静音游戏音效的代码
        // 使用音频管理器静音
        audioManager->setVolume(0);
        qDebug() << "The game has been muted";
        // 如果需要：audioManager->setMute(true);
    } else {
        muteButton->setText("Mute");
        muteButton->setStyleSheet("QPushButton { background-color: lightblue; font-size: 14px; }");
        // 这里添加取消静音的代码
        // 使用音频管理器取消静音
        audioManager->setVolume(50);
        qDebug() << "The game has been unmuted";
        // 如果需要：audioManager->setMute(false);
    }
}

// 暂停/继续功能
void MainWindow::togglePause()
{
    isPaused = !isPaused;

    if (isPaused) {
        pauseButton->setText("Resume");
        pauseButton->setStyleSheet("QPushButton { background-color: red; font-size: 14px; }");
        // 暂停游戏逻辑
        gameStateTimer->stop(); // 暂停状态检查
        timer->stop();          // 暂停主游戏计时器
        sunSpawnTimer->stop();  //【谢嘉翔】暂停太阳生成
        qDebug() << "The game has been paused";
    } else {
        pauseButton->setText("Pause");
        pauseButton->setStyleSheet("QPushButton { background-color: lightgreen; font-size: 14px; }");
        // 继续游戏逻辑
        gameStateTimer->start(100); // 恢复状态检查
        timer->start(33);           // 恢复主游戏计时器
        sunSpawnTimer->start();     //【谢嘉翔】恢复太阳生成
        qDebug() << "The game continues";
    }
}

// 检查游戏状态
void MainWindow::checkGameState()
{
    ++timeCount;
    if (gameOver) return; // 已经结束就不再判断

    bool zombieReachedLeft = false;
    bool allZombiesDefeated = timeCount >= 300? true : false;  // 假设全部死亡，若发现活的就设为 false

    // 清理无效指针并检查僵尸状态
    for (int i = zombiesVector.size() - 1; i >= 0; --i)
    {
        Zombies *zombie = zombiesVector[i];
        if (!zombie) {
            zombiesVector.remove(i);
            continue;
        }

        // 使用新的 isAlive() 方法
        if (zombie->isAlive())
        {
            allZombiesDefeated = false;

            // 如果僵尸活着且到达最左边界，判定为失败
            if (zombie->x() <= 150)
            {
                zombieReachedLeft = true;
                break;
            }
        }
    }

    // 游戏失败条件
    if (zombieReachedLeft)
    {
        gameOver = true;
        gameStateTimer->stop();
        timer->stop();

        // 新增：移除原来的弹窗，改用图片显示
        // QMessageBox::information(this, "游戏结束", "僵尸吃掉了你的脑子！");

        // 新增：用图片显示
        showGameOverImage(false); //false表示失败

        qDebug() << "游戏失败：僵尸到达左侧";
    }

    // 游戏胜利条件
    else if (allZombiesDefeated && ZombiesNum > 0 && zombiesVector.isEmpty())  // 防止刚开始没僵尸就胜利且确保vector中没有僵尸
    {
        gameOver = true;
        gameStateTimer->stop();
        timer->stop();

        // 播放胜利音效
        if (audioManager) {
            audioManager->playVictorySound();
        }

        QMessageBox::information(this, "游戏胜利", "恭喜你击败了所有僵尸！");
        qDebug() << "游戏胜利：所有僵尸被消灭";
    }
}

// 新增：游戏结束时，显示“僵尸吃掉了你的脑子”图片
void MainWindow::showGameOverImage(bool isWin)
{
    // 创建覆盖全屏的半透明层
    QWidget *overlay = new QWidget(this);  // 修正：QWidget *overlay
    overlay->setGeometry(this->rect());
    overlay->setStyleSheet("background-color: rgba(0, 0, 0, 150);"); // 半透明黑色背景

    // 创建图片标签
    QLabel *imageLabel = new QLabel(overlay);  // 修正：QLabel *imageLabel
    imageLabel->setAlignment(Qt::AlignCenter);

    if (isWin) {  // 修正：isWin（不是!swfin）
        // 胜利图片（如果你有的话）
        QPixmap winPixmap(":/image/game_win.png");  // 修正：QPixmap winPixmap
        imageLabel->setPixmap(winPixmap.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));  // 修正：400, 300
    } else {
        // 失败图片 - "僵尸吃掉了你的脑子"
        QPixmap losePixmap(":/image/zombie_eat_brain.png"); // 修正：QPixmap losePixmap
        if (losePixmap.isNull()) {  // 修正：losePixmap.isNull()
            qDebug() << "失败图片未找到，请检查路径";
            // 备用方案：显示文字
            imageLabel->setText("💀 僵尸吃掉了你的脑子！ 💀");  // 修正：正确的文字
            imageLabel->setStyleSheet("QLabel { color: red; font-size: 36px; font-weight: bold; background: transparent; }");
        } else {
            imageLabel->setPixmap(losePixmap.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));  // 修正：400, 300
        }
    }

    // 创建重新开始按钮
    QPushButton *restartButton = new QPushButton("重新开始", overlay);  // 修正：QPushButton *restartButton
    restartButton->setStyleSheet("QPushButton { font-size: 20px; padding: 10px 20px; background-color: #4CAF50; color: white; border-radius: 5px; }");
    restartButton->setFixedSize(120, 50);

    // 布局
    QVBoxLayout *layout = new QVBoxLayout(overlay);  // 修正：QVBoxLayout *layout
    layout->addWidget(imageLabel, 0, Qt::AlignCenter);  // 修正：imageLabel
    layout->addWidget(restartButton, 0, Qt::AlignCenter);
    layout->setAlignment(Qt::AlignCenter);

    overlay->setLayout(layout);
    overlay->show();

    // 连接重新开始按钮（这个要在函数内部）
    connect(restartButton, &QPushButton::clicked, this, [this, overlay]() {
        overlay->deleteLater(); //删除覆盖层
        // 新增：重新开始游戏的按钮
        restartGame();
        qDebug() << "重新开始游戏";
    });
}

// 新增：重新开始游戏的函数
void MainWindow::restartGame()
{
    qDebug() << "=== 重新开始游戏 ===";

    // 重置状态变量
    gameOver = false;
    isPaused = false;
    isMuted = false;

    // 重置UI
    muteButton->setText("Mute");
    muteButton->setStyleSheet("QPushButton { background-color: lightblue; }");
    pauseButton->setText("Pause");
    pauseButton->setStyleSheet("QPushButton { background-color: lightgreen; }");

    // 停止定时器
    gameStateTimer->stop();
    timer->stop();

    // 清理所有僵尸
    for (Zombies *zombie : zombiesVector) {
        if (zombie) {
            scene->removeItem(zombie);
            delete zombie;
        }
    }
    zombiesVector.clear();
    MainWindow::ZombiesNum = 0;

    // 清理所有太阳（新增）
    QList<QGraphicsItem*> allItems = scene->items();
    for (QGraphicsItem* item : allItems) {
        Sun* sun = dynamic_cast<Sun*>(item);
        if (sun) {
            scene->removeItem(item);
            delete item;
        }
    }

    // 新增：重置商店太阳数
    QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem* item : items) {
        Shop* foundShop = dynamic_cast<Shop*>(item);
        if (foundShop) {
            foundShop->resetSun(200);  // 重置为初始200太阳
            qDebug() << "商店太阳数已重置";
            break;
        }
    }

    // 新增：清理所有植物
    QList<QGraphicsItem*> sceneItems = scene->items();
    for (QGraphicsItem* item : sceneItems) {
        Plant* plant = dynamic_cast<Plant*>(item);
        if (plant) {
            scene->removeItem(item);
            delete item;
            qDebug() << "删除植物";
        }
    }

    // 重新开始游戏循环
    gameStateTimer->start(100);
    timer->start(33);

    // 重置音频
    if (audioManager) {
        audioManager->setVolume(50);
        audioManager->playBackgroundMusic();
    }

    qDebug() << "游戏已重置，清理了僵尸、太阳和植物，等待玩家点击开始按钮";
}

// 新增：显示音量菜单
void MainWindow::showVolumeMenu()
{
    QMenu volumeMenu(this);

    // 添加音量选项
    QAction *lowVolume = volumeMenu.addAction("🔈 Low (25%)");
    QAction *mediumVolume = volumeMenu.addAction("🔉 Medium (50%)");
    QAction *highVolume = volumeMenu.addAction("🔊 High (100%)");
    QAction *muteAction = volumeMenu.addAction("🔇 Mute (0%)");

    // 显示菜单
    QAction *selectedAction = volumeMenu.exec(volumeButton->mapToGlobal(QPoint(0, volumeButton->height())));

    if (selectedAction) {
        if (selectedAction == lowVolume) {
            audioManager->setVolume(25);
            volumeButton->setText("Vol:Low");
            isMuted = false;
            muteButton->setText("Mute");
        } else if (selectedAction == mediumVolume) {
            audioManager->setVolume(50);
            volumeButton->setText("Vol:Mid");
            isMuted = false;
            muteButton->setText("Mute");
        } else if (selectedAction == highVolume) {
            audioManager->setVolume(100);
            volumeButton->setText("Vol:High");
            isMuted = false;
            muteButton->setText("Mute");
        } else if (selectedAction == muteAction) {
            audioManager->setVolume(0);
            volumeButton->setText("Vol:Mute");
            isMuted = true;
            muteButton->setText("Unmute");
        }
    }
}

// 【谢嘉翔添加】: 实现生成太阳的槽函数
void MainWindow::spawnSun(){
    if (gameOver || isPaused)
        return;

    Sun *sun = new Sun();
    scene->addItem(sun);
}

// 处理僵尸死亡信号
void MainWindow::handleZombieDied(Zombies *zombie){
    // 从zombiesVector中移除僵尸
    int index = zombiesVector.indexOf(zombie);
    if (index != -1) {
        zombiesVector.remove(index);
        qDebug() << "Removed zombie from vector. Total zombies:" << zombiesVector.size();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
    delete scene;
    delete view;

    delete muteButton;
    delete pauseButton;
    delete volumeButton;
    delete gameStateTimer;
    delete sunSpawnTimer;//【谢嘉翔】
}

int MainWindow::ZombiesNum = 0;
