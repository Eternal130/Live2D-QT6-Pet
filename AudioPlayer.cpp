#include "AudioPlayer.h"
#include <fstream>
#include <cstdint>
#include <string>
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

/**
 * 快速获取WAV文件时长(毫秒)
 * 仅读取文件头，不加载整个文件
 */
int getWavDuration(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return 3000; // 默认3秒
    }

    // WAV文件结构参数
    uint32_t sampleRate = 0;
    uint16_t numChannels = 0;
    uint16_t bitsPerSample = 0;
    uint32_t dataSize = 0;

    // 检查RIFF头
    char header[12];
    if (!file.read(header, sizeof(header))) {
        file.close();
        return 3000;
    }

    if (memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "WAVE", 4) != 0) {
        file.close();
        return 3000; // 非有效WAV文件
    }

    // 查找格式块和数据块
    char chunkId[4];
    uint32_t chunkSize;
    bool foundFormat = false;
    bool foundData = false;

    while (!foundFormat || !foundData) {
        if (!file.read(chunkId, sizeof(chunkId)) ||
            !file.read(reinterpret_cast<char*>(&chunkSize), 4)) {
            break;
        }

        if (memcmp(chunkId, "fmt ", 4) == 0) {
            // 跳过格式类型(2字节)
            file.seekg(2, std::ios::cur);

            // 读取通道数
            file.read(reinterpret_cast<char*>(&numChannels), 2);

            // 读取采样率
            file.read(reinterpret_cast<char*>(&sampleRate), 4);

            // 跳过字节率(4字节)和块对齐(2字节)
            file.seekg(6, std::ios::cur);

            // 读取位深度
            file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

            // 跳过剩余的格式数据
            if (chunkSize > 16) {
                file.seekg(chunkSize - 16, std::ios::cur);
            }

            foundFormat = true;
        }
        else if (memcmp(chunkId, "data", 4) == 0) {
            dataSize = chunkSize;
            foundData = true;
        }
        else {
            // 跳过其他块
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    file.close();

    // 计算时长(毫秒)
    if (sampleRate > 0 && numChannels > 0 && bitsPerSample > 0) {
        double bytesPerSample = bitsPerSample / 8.0;
        double bytesPerSecond = sampleRate * numChannels * bytesPerSample;
        return static_cast<int>((dataSize * 1000) / bytesPerSecond);
    }

    return 3000; // 默认3秒
}
void AudioPlayer::play(const std::string& filePath)
{
    _player = new QSoundEffect();
    _player->setSource(QUrl::fromLocalFile(QString::fromStdString(filePath)));
    _player->setVolume(ConfigManager::getInstance().getVolume() / 100.0f);
    _player->play();

    _duration = getWavDuration(filePath);
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

int AudioPlayer::getDuration() const
{
    if (_duration)
    {
        return _duration;
    }
    return 0;
}
