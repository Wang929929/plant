#include "audiomanager.h"
#include <QAudioOutput>  // 添加这个头文件

AudioManager* AudioManager::m_instance = nullptr;

AudioManager* AudioManager::instance()
{
    if (!m_instance) {
        m_instance = new AudioManager();
    }
    return m_instance;
}

AudioManager::AudioManager(QObject *parent) : QObject(parent)
{
    // 初始化背景音乐 - 新API
    backgroundMusic = new QMediaPlayer(this);
    QAudioOutput *audioOutput = new QAudioOutput(this);
    backgroundMusic->setAudioOutput(audioOutput);
    backgroundMusic->setSource(QUrl("qrc:/sounds/background_music.MP3"));
    audioOutput->setVolume(0.5);  // 0.0 - 1.0范围

    // 初始化胜利音效
    victorySound = new QSoundEffect(this);
    victorySound->setSource(QUrl("qrc:/sounds/victory.wav"));
    victorySound->setVolume(0.5f);

    currentVolume = 50;
}

void AudioManager::setVolume(int volume)
{
    currentVolume = volume;
    // 设置背景音乐音量
    if (backgroundMusic->audioOutput()) {
        backgroundMusic->audioOutput()->setVolume(volume / 100.0);
    }
    // 设置音效音量
    victorySound->setVolume(volume / 100.0f);
}

void AudioManager::playBackgroundMusic()
{
    if (backgroundMusic->playbackState() != QMediaPlayer::PlayingState) {
        backgroundMusic->play();
    }
}

void AudioManager::playVictorySound()
{
    victorySound->play();
}

void AudioManager::stopAll()
{
    backgroundMusic->stop();
    victorySound->stop();
}
