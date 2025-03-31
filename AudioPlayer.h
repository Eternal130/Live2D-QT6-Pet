#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QSoundEffect>
#include <string>

class AudioPlayer
{
public:
    AudioPlayer();
    ~AudioPlayer();
    static AudioPlayer& getInstance() {
        static AudioPlayer instance;
        return instance;
    }
    // 播放音频文件
    void play(const std::string& filePath);

    // 获取音频时长
    int getDuration() const;

    // 停止播放
    void stop();

    // 设置音量
    void setVolume() const;

private:
    // 禁止复制
    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;
    QSoundEffect* _player{nullptr};
    int _duration{0}; // 音频时长
};

#endif //AUDIOPLAYER_H
