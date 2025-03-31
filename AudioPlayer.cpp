#include "AudioPlayer.h"
#include <QUrl>
#include <QSoundEffect>

#include "ConfigManager.h"

AudioPlayer::AudioPlayer()
{
    _player = new QSoundEffect();
}

AudioPlayer::~AudioPlayer()
{
    if (_player)
    {
        _player->stop();
        delete _player;
        _player = nullptr;
    }
}

void AudioPlayer::play(const std::string& filePath)
{
    _player = new QSoundEffect();
    _player->setSource(QUrl::fromLocalFile(QString::fromStdString(filePath)));
    _player->setVolume(ConfigManager::getInstance().getVolume() / 100.0f);
    _player->play();
}

void AudioPlayer::stop()
{
    if (_player)
    {
        _player->stop();
    }
}

void AudioPlayer::setVolume() const {
    if (_player)
    {
        // QMediaPlayer音量范围是0-1，浮点数
        _player->setVolume(ConfigManager::getInstance().getVolume() / 100.0f);
    }
}
