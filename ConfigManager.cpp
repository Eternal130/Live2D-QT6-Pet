#include "ConfigManager.h"
#include "GLCore.h"

ConfigManager::ConfigManager()
    : m_settings("AiriDesktopGril/Settings.ini", QSettings::IniFormat)
{
    loadConfig();
}

void ConfigManager::loadConfig()
{
    m_fps = m_settings.value("Graphics/FPS", 60).toInt();
    w_width = m_settings.value("Graphics/Width", 720).toInt();
    w_height = m_settings.value("Graphics/Height", 480).toInt();
    w_x = m_settings.value("Graphics/X", -1).toInt();
    w_y = m_settings.value("Graphics/Y", -1).toInt();
    GLCore::fps = m_fps;
}

void ConfigManager::saveConfig()
{
    m_settings.setValue("Graphics/FPS", m_fps);
    m_settings.sync();
    m_settings.setValue("Graphics/Width", w_width);
    m_settings.sync();
    m_settings.setValue("Graphics/Height", w_height);
    m_settings.sync();
    m_settings.setValue("Graphics/X", w_x);
    m_settings.sync();
    m_settings.setValue("Graphics/Y", w_y);
    m_settings.sync();
}

void ConfigManager::setFps(int fps)
{
    m_fps = fps;
    GLCore::fps = fps;
    saveConfig();
}
void ConfigManager::setWidth(int width)
{
    w_width = width;
    saveConfig();
}
void ConfigManager::setHeight(int height)
{
    w_height = height;
    saveConfig();
}
void ConfigManager::setX(int x)
{
    w_x = x;
    saveConfig();
}
void ConfigManager::setY(int y)
{
    w_y = y;
    saveConfig();
}