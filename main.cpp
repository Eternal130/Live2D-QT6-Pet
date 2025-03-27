#include <QtWidgets/QApplication>

#include "ConfigManager.h"
#include "GLCore.h"
#include "ElaApplication.h"

int main(int argc, char *argv[])
{
    ConfigManager::getInstance().loadConfig();
    QApplication a(argc, argv);
    GLCore w(ConfigManager::getInstance().getWidth(), ConfigManager::getInstance().getHeight());
    eApp->init();
    //如果x和y都是-1，说明是第一次启动或者没有改过窗口位置，那么就不用移动窗口，默认是居中
    if (!(ConfigManager::getInstance().getX() == -1 && ConfigManager::getInstance().getY() == -1))
        w.move(ConfigManager::getInstance().getX(), ConfigManager::getInstance().getY());
    w.show();
    return a.exec();
}
