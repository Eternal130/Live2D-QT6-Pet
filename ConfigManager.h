#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QSettings>

class ConfigManager {
public:
    static ConfigManager &getInstance() {
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

    QString getName() const { return m_name; }
    QString getFName() const { return f_name; }

    void setName(const QString &name);

    void setFName(const QString &name);

private:
    ConfigManager();

    ~ConfigManager() = default;

    QSettings m_settings;
    int m_fps;// fps，顾名思义
    int w_width;// 窗口宽度
    int w_height;// 窗口高度
    int w_x;// 窗口X坐标
    int w_y;// 窗口Y坐标
    QString m_name;// 模型名称，是xxx.model3.json中的xxx
    QString f_name;// 模型文件夹名称，是xxx.model3.json所在的文件夹名称，本来这两个应该是一样的，但是我发现我导出的模型就有的不一样，所以直接分开了
};


#endif //CONFIGMANAGER_H
