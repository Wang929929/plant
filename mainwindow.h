#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QPixmap>
#include <QVector>
#include <QPushButton>  // 添加按钮头文件

#include "audiomanager.h"  // 添加音频管理器头文件
#include "map.h"
#include "zombies.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showBeginStandZombies();
    void deleteBeginZombie();
    void outZombies();
    bool checkColliding();
    static int ZombiesNum;

private slots:
    // 添加槽函数
    void toggleMute();      // 静音/取消静音
    void togglePause();     // 暂停/继续
    void checkGameState();  // 检查游戏状态
    void showVolumeMenu();  // 新增这行：显示音量菜单

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QGraphicsScene *scene;
    QGraphicsView *view;

    QVector<Zombies *> zombiesVector;

    // 添加成员变量
    QPushButton *muteButton;    // 静音按钮
    QPushButton *pauseButton;   // 暂停按钮
    QTimer *gameStateTimer;     // 游戏状态检查定时器
    QPushButton *volumeButton;  // 新增：音量调节按钮
    AudioManager *audioManager; // 新增：音频管理器

    // 游戏状态变量
    bool isMuted;
    bool isPaused;
    bool gameOver;
};
#endif // MAINWINDOW_H
