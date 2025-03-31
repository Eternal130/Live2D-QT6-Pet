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
    m_volume = m_settings.value("Audio/Volume", 100).toInt();
    m_isAutoStart = m_settings.value("Application/AutoStart", false).toBool();
    w_width = m_settings.value("Graphics/Width", 720).toInt();
    w_height = m_settings.value("Graphics/Height", 480).toInt();
    w_x = m_settings.value("Graphics/X", -1).toInt();
    w_y = m_settings.value("Graphics/Y", -1).toInt();
    m_name = m_settings.value("Model/Name", "a").value<QString>();
    f_name = m_settings.value("Model/FolderName", "a").value<QString>();
    GLCore::fps = m_fps;
}

void ConfigManager::saveConfig()
{
    m_settings.setValue("Graphics/FPS", m_fps);
    m_settings.setValue("Audio/Volume", m_volume);
    m_settings.setValue("Application/AutoStart", m_isAutoStart);
    m_settings.setValue("Graphics/Width", w_width);
    m_settings.setValue("Graphics/Height", w_height);
    m_settings.setValue("Graphics/X", w_x);
    m_settings.setValue("Graphics/Y", w_y);
    m_settings.setValue("Model/Name", m_name);
    m_settings.setValue("Model/FolderName", f_name);
    m_settings.sync();
}

void ConfigManager::setFps(int fps)
{
    m_fps = fps;
    GLCore::fps = fps;
    saveConfig();
}
void ConfigManager::setVolume(int volume)
{
    m_volume = volume;
    saveConfig();
}
void ConfigManager::setAutoStart(bool autoStart)
{
    m_isAutoStart = autoStart;
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
void ConfigManager::setName(const QString &name)
{
    m_name = name;
    saveConfig();
}
void ConfigManager::setFName(const QString &name)
{
    f_name = name;
    saveConfig();
}