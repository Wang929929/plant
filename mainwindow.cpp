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

    // 新增：游戏启动时立即开始计时
    gameStartTime = QTime::currentTime();
    totalPausedTime = 0;
    isTiming = true;

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
    
    connect(timer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    spawnInterval = 6000;      // 初始 6 秒
    minSpawnInterval = 1000;   // 最快 1 秒

    spawnTimer = new QTimer(this);
    spawnTimer->setInterval(spawnInterval);

    connect(spawnTimer, &QTimer::timeout, this, [this]() {
        outZombies();
    });

    spawnTimer->start();


    // ====== 刷新加速计时器（每 20 秒加速一次）======
    QTimer *speedUpTimer = new QTimer(this);

    connect(speedUpTimer, &QTimer::timeout, this, [this]() {

        if (spawnInterval > minSpawnInterval) {
            spawnInterval -= 500; // 每次减少 0.5 秒
            spawnTimer->setInterval(spawnInterval);
        }

        qDebug() << "[Spawn Speed Up] 当前僵尸刷新间隔(毫秒):" << spawnInterval;
    });

    speedUpTimer->start(20000); // 每 20 秒加速一次
    // ======（新增）僵尸强度随时间增长 ======
    zombieHealthBonus = 0;

    QTimer *zombieBoostTimer = new QTimer(this);
    connect(zombieBoostTimer, &QTimer::timeout, this, [=]() {
        zombieHealthBonus += 20;  // 每30秒 +5 血，可自己调
        qDebug() << "[Zombie Boost] 当前僵尸血量加成：" << zombieHealthBonus;
    });

    zombieBoostTimer->start(30000); // 每30秒触发一次
}

void MainWindow::outZombies()//僵尸随机出现，游戏开始
{
    if (gameOver || isPaused)
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

// 暂停/继续功能
void MainWindow::togglePause()
{
    isPaused = !isPaused;

    if (isPaused) {
        pauseButton->setText("Resume");
        pauseButton->setStyleSheet("QPushButton { background-color: red; font-size: 14px; }");

        // 记录暂停开始时间，并停止计时
        pauseStartTime = QTime::currentTime();
        isTiming = false;  // 停止计时

        gameStateTimer->stop();
        timer->stop();
        if (sunSpawnTimer) sunSpawnTimer->stop();

        qDebug() << "游戏已暂停，计时停止";
    } else {
        pauseButton->setText("Pause");
        pauseButton->setStyleSheet("QPushButton { background-color: lightgreen; font-size: 14px; }");

        // 计算暂停时长并累计
        int pauseDuration = pauseStartTime.msecsTo(QTime::currentTime());
        totalPausedTime += pauseDuration;
        isTiming = true;  // 恢复计时

        qDebug() << "暂停时长:" << pauseDuration << "ms, 累计暂停:" << totalPausedTime << "ms";

        gameStateTimer->start(100);
        timer->start(33);
        if (sunSpawnTimer) sunSpawnTimer->start(10000);

        qDebug() << "游戏继续，计时恢复";
    }
}

void MainWindow::checkGameState()
{
    if (gameOver) return;

    bool zombieReachedLeft = false;

    // 检查是否有僵尸到达左边
    for (int i = zombiesVector.size() - 1; i >= 0; --i)
    {
        Zombies *zombie = zombiesVector[i];
        if (!zombie) {
            zombiesVector.remove(i);
            continue;
        }

        if (zombie->isAlive() && zombie->x() <= 150)
        {
            zombieReachedLeft = true;
            break;
        }
    }

    // 游戏失败条件
    if (zombieReachedLeft)
    {
        gameOver = true;
        gameStateTimer->stop();
        timer->stop();
        showGameOverImage(false);
        qDebug() << "游戏失败：僵尸到达左侧";
    }

    // 游戏胜利条件：真正的3分钟游戏时间（排除暂停时间）
    else if (isTiming)  // 只在计时状态下检查胜利
    {
        // 计算实际游戏时间 = 当前时间 - 游戏开始时间 - 累计暂停时间
        int actualPlayTime = gameStartTime.msecsTo(QTime::currentTime()) - totalPausedTime;

        // 每30秒显示一次剩余时间（调试用）
        static int lastDebugTime = 0;
        if (actualPlayTime - lastDebugTime >= 30000) { // 每30秒
            int remaining = (180000 - actualPlayTime) / 1000;
            if (remaining > 0) {
                qDebug() << "剩余时间:" << remaining << "秒";
            }
            lastDebugTime = actualPlayTime;
        }

        // 胜利条件：实际游戏时间达到3分钟（180,000毫秒）
        if (actualPlayTime >= 180000)  // 3分钟 = 180,000毫秒
        {
            gameOver = true;
            gameStateTimer->stop();
            timer->stop();

            if (audioManager) {
                audioManager->playVictorySound();
            }

            int totalSeconds = actualPlayTime / 1000;
            int pauseSeconds = totalPausedTime / 1000;

            QMessageBox::information(this, "游戏胜利",
                                     QString("恭喜你坚持了3分钟！\n"
                                             "总用时: %1分%2秒\n"
                                             "实际游戏时间: 3分钟\n"
                                             "暂停时间: %3秒")
                                         .arg(totalSeconds / 60)
                                         .arg(totalSeconds % 60)
                                         .arg(pauseSeconds));

            qDebug() << "🎉 游戏胜利！总用时:" << totalSeconds << "秒，暂停:" << pauseSeconds << "秒";
        }
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

    // 重置计时相关变量
    gameStartTime = QTime::currentTime();
    totalPausedTime = 0;
    isTiming = true;

    // 重置音频
    if (audioManager) {
        audioManager->setVolume(50);
        audioManager->playBackgroundMusic();
    }

    qDebug() << "游戏已重置，清理了僵尸、太阳和植物，计时重新开始，等待玩家点击开始按钮";
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
int MainWindow::zombieHealthBonus = 0;
