#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shop.h"
#include <QVector>
#include <QRandomGenerator>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>

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
    this->showBeginStandZombies();

    connect(timer, &QTimer::timeout, scene, &QGraphicsScene::advance);
}

void MainWindow::showBeginStandZombies()
{
    Zombies *zombie = new Zombies("normalZombie", true, scene, this);
    zombie->setPos(900, 0);   // 第一行靠右
    zombiesVector.push_back(zombie);

    zombie = new Zombies("normalZombie", true, scene, this);
    zombie->setPos(900, 100);   // 第二行稍下
    zombiesVector.push_back(zombie);

    zombie = new Zombies("normalZombie", true, scene, this);
    zombie->setPos(900, 200);    // 第三行中间
    zombiesVector.push_back(zombie);

    zombie = new Zombies("normalZombie", true, scene, this);
    zombie->setPos(900, 300);    // 和上面的错开一点
    zombiesVector.push_back(zombie);

    zombie = new Zombies("normalZombie", true, scene, this);
    zombie->setPos(930, 300);    // 更靠下的行
    zombiesVector.push_back(zombie);

    zombie = new Zombies("normalZombie", true, scene, this);
    zombie->setPos(930, 100);    // 稍微靠右一点
    zombiesVector.push_back(zombie);

    zombie = new Zombies("normalZombie", true, scene, this);
    zombie->setPos(900, 400);    // 最下面一行
    zombiesVector.push_back(zombie);
}

void MainWindow::deleteBeginZombie()
{
    for (QVector<Zombies *>::iterator it = this->zombiesVector.begin();
         it != this->zombiesVector.end();
         it++)
    {
        delete *it;
    }
}//删掉开场的僵尸（开始游戏后把那些站着的僵尸删掉）
void MainWindow::outZombies()//开场随机生成僵尸
{

    auto spawnZombie = [=](int count) {
        for (int i = 0; i < count; ++i) {
            int randomY = QRandomGenerator::global()->bounded(5);
            Zombies *zombie = new Zombies("normalZombie", false, this->scene, this);
            zombie->setPos(900, randomY * 100);
        }
    };

    // 定时生成僵尸
    QTimer::singleShot(0, this, [=]() { spawnZombie(1); });
    QTimer::singleShot(20000, this, [=]() { spawnZombie(2); });
    QTimer::singleShot(40000, this, [=]() { spawnZombie(3); });
    QTimer::singleShot(60000, this, [=]() { spawnZombie(2); });
    QTimer::singleShot(80000, this, [=]() { spawnZombie(4); });
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
        timer->stop(); // 暂停主游戏计时器
        qDebug() << "The game has been paused";
    } else {
        pauseButton->setText("Pause");
        pauseButton->setStyleSheet("QPushButton { background-color: lightgreen; font-size: 14px; }");
        // 继续游戏逻辑
        gameStateTimer->start(100); // 恢复状态检查
        timer->start(33); // 恢复主游戏计时器
        qDebug() << "The game continues";
    }
}

// 检查游戏状态
void MainWindow::checkGameState()
{
    // 这里需要与负责Zombie模块的组员协作
    // 你需要从他们那里获取这些信息：

    // 伪代码 - 你需要根据实际实现修改：
    bool zombieReachedLeft = false;   // 是否有僵尸到达最左侧
    bool allZombiesDefeated = false;  // 是否所有僵尸都被消灭

    // 示例：你需要这样获取数据（具体方法要和组员商量）：
    // zombieReachedLeft = zombieManager->hasZombieReachedLeft();
    // allZombiesDefeated = (zombieManager->getAliveZombieCount() == 0);

    // 检查失败条件
    if (zombieReachedLeft && !gameOver) {
        gameOver = true;
        gameStateTimer->stop();
        timer->stop();

        QMessageBox::information(this, "游戏结束", "僵尸吃掉了你的脑子！游戏失败！");
        qDebug() << "游戏失败：僵尸到达左侧";

    }
    // 检查胜利条件
    else if (allZombiesDefeated && !gameOver) {
        gameOver = true;
        gameStateTimer->stop();
        timer->stop();

        QMessageBox::information(this, "游戏胜利", "恭喜你击败了所有僵尸！");
        qDebug() << "游戏胜利：所有僵尸被消灭";
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
}

int MainWindow::ZombiesNum = 0;
