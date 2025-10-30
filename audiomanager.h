#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QSoundEffect>

class AudioManager : public QObject
{
    Q_OBJECT
public:
    static AudioManager* instance();

    void setVolume(int volume);  // 0-100
    void playBackgroundMusic();
    void playVictorySound();
    void stopAll();

private:
    AudioManager(QObject *parent = nullptr);
    static AudioManager* m_instance;

    QMediaPlayer *backgroundMusic;
    QSoundEffect *victorySound;
    int currentVolume;
};

#endif // AUDIOMANAGER_H
