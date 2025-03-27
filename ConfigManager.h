#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QSettings>
class ConfigManager {
    public:
        static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    void loadConfig();
    void saveConfig();

    // FPS相关配置
    int getFps() const { return m_fps; }
    void setFps(int fps);
    int getWidth() const { return w_width; }
    void setWidth(int width);
    int getHeight() const { return w_height; }
    void setHeight(int height);
    int getX() const { return w_x; }
    void setX(int x);
    int getY() const { return w_y; }
    void setY(int y);

private:
    ConfigManager();
    ~ConfigManager() = default;

    QSettings m_settings;
    int m_fps;
    int w_width;
    int w_height;
    int w_x;
    int w_y;
};



#endif //CONFIGMANAGER_H
