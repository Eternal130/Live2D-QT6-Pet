#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QSoundEffect>
#include <string>

class AudioPlayer
{
public:
    AudioPlayer();
    ~AudioPlayer();

    // 播放音频文件
    void play(const std::string& filePath);

    // 停止播放
    void stop();

    // 设置音量 (0.0-1.0)
    void setVolume(float volume);

private:
    QSoundEffect* _player;
};

#endif //AUDIOPLAYER_H
