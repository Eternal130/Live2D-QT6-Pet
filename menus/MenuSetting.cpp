#include "MenuSetting.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>

#include "../config/ConfigManager.h"
#include "ElaScrollPageArea.h"
#include "ElaSlider.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "ElaToggleSwitch.h"
#include "../GLCore.h"

/**
 * @brief MenuSetting构造函数
 * @param glCore 应用的OpenGL核心实例
 * @param parent 父控件指针
 */
MenuSetting::MenuSetting(GLCore *glCore, QWidget *parent)
    : ElaScrollPage(parent)
      , _glCore(glCore) {
    // 初始化主题变更连接
    initThemeConnection();

    // 初始化FPS设置组件
    createFpsSettingUI();

    // 初始化音量设置组件
    createVolumeSettingUI();

    // 初始化透明度检查时间间隔设置组件
    createCheckTimeSettingUI();

    // 初始化点击穿透设置组件
    createTransparentSettingUI();

    // 初始化开机启动设置组件
    createAutoStartSettingUI();

    // 初始化中央布局
    setupCentralWidget();
}

/**
 * @brief MenuSetting析构函数
 */
MenuSetting::~MenuSetting() {
    // 析构函数中不需要手动释放QObject树中的对象
}

/**
 * @brief 初始化主题变更连接
 */
void MenuSetting::initThemeConnection() {
    connect(eTheme, &ElaTheme::themeModeChanged, this, [this]() {
        if (!parentWidget()) {
            update();
        }
    });
}

/**
 * @brief 创建FPS设置相关组件
 */
void MenuSetting::createFpsSettingUI() {
    // 创建FPS设置区域
    ElaScrollPageArea *settingArea = new ElaScrollPageArea(this);
    QHBoxLayout *settingLayout = new QHBoxLayout(settingArea);

    // 创建FPS显示文本
    ElaText *settingText = new ElaText(QString("FPS (10-144)  : %1").arg(GLCore::fps), this);
    settingText->setWordWrap(false);
    settingText->setTextPixelSize(15);

    // 创建FPS滑动条
    _fpsSlider = new ElaSlider(this);
    _fpsSlider->setMinimum(10);
    _fpsSlider->setMaximum(144);
    _fpsSlider->setValue(GLCore::fps);

    // 添加到布局
    settingLayout->addWidget(settingText);
    settingLayout->addWidget(_fpsSlider);

    // 连接滑动条的值变化信号
    connectFpsSliderSignals(settingText);

    // 保存创建的区域到类成员变量
    _fpsSettingArea = settingArea;
}

/**
 * @brief 连接FPS滑动条的信号
 * @param settingText FPS显示文本控件
 */
void MenuSetting::connectFpsSliderSignals(ElaText *settingText) {
    // 滑动条值变化时更新UI和定时器
    connect(_fpsSlider, &ElaSlider::valueChanged, this, [this, settingText](int value) {
        GLCore::fps = value;
        settingText->setText(QString("FPS (10-144)  : %1").arg(GLCore::fps));
        updateTimers(value);
    });

    // 滑动条释放时保存设置
    connect(_fpsSlider, &ElaSlider::sliderReleased, this, [] {
        ConfigManager::getInstance().setFps(GLCore::fps);
    });
}

/**
 * @brief 创建音量设置相关组件
 */
void MenuSetting::createVolumeSettingUI() {
    // 创建音量设置区域
    ElaScrollPageArea *settingArea = new ElaScrollPageArea(this);
    QHBoxLayout *settingLayout = new QHBoxLayout(settingArea);

    // 获取当前音量值
    int currentVolume = ConfigManager::getInstance().getVolume();

    // 创建音量显示文本
    ElaText *settingText = new ElaText(QString("音量 (0-100): %1").arg(currentVolume), this);
    settingText->setWordWrap(false);
    settingText->setTextPixelSize(15);

    // 创建音量滑动条
    _volumeSlider = new ElaSlider(this);
    _volumeSlider->setMinimum(0);
    _volumeSlider->setMaximum(100);
    _volumeSlider->setValue(currentVolume);

    // 添加到布局
    settingLayout->addWidget(settingText);
    settingLayout->addWidget(_volumeSlider);

    // 连接滑动条的值变化信号
    connectVolumeSliderSignals(settingText);

    // 保存创建的区域到类成员变量
    _volumeSettingArea = settingArea;
}

/**
 * @brief 连接音量滑动条的信号
 * @param settingText 音量显示文本控件
 */
void MenuSetting::connectVolumeSliderSignals(ElaText *settingText) {
    // 滑动条值变化时更新UI
    connect(_volumeSlider, &ElaSlider::valueChanged, this, [settingText](int value) {
        settingText->setText(QString("音量 (0-100): %1").arg(value));
    });

    // 滑动条释放时保存设置
    connect(_volumeSlider, &ElaSlider::sliderReleased, this, [this] {
        ConfigManager::getInstance().setVolume(_volumeSlider->value());
        AudioPlayer::getInstance().setVolume();
    });
}

/**
 * @brief 创建透明度检查时间间隔设置相关UI组件
 */
void MenuSetting::createCheckTimeSettingUI() {
    // 创建透明度检查时间间隔设置区域
    ElaScrollPageArea *settingArea = new ElaScrollPageArea(this);
    QHBoxLayout *settingLayout = new QHBoxLayout(settingArea);

    // 获取当前透明度检查时间间隔值
    int currentCheckTime = ConfigManager::getInstance().getCheckTime();

    // 创建透明度检查时间间隔显示文本
    ElaText *settingText = new ElaText(QString("透明度检查时间间隔 (10-1000ms): %1").arg(currentCheckTime), this);
    settingText->setWordWrap(false);
    settingText->setTextPixelSize(15);

    // 创建透明度检查时间间隔滑动条
    _checkTimeSlider = new ElaSlider(this);
    _checkTimeSlider->setMinimum(10);
    _checkTimeSlider->setMaximum(1000);
    _checkTimeSlider->setSingleStep(10);
    _checkTimeSlider->setPageStep(10);
    _checkTimeSlider->setValue(currentCheckTime);

    // 添加到布局
    settingLayout->addWidget(settingText);
    settingLayout->addWidget(_checkTimeSlider);

    // 连接滑动条的值变化信号
    connectCheckTimeSliderSignals(settingText);

    // 保存创建的区域到类成员变量
    _checkTimeSettingArea = settingArea;
}

/**
 * @brief 连接透明度检查时间间隔滑动条的信号
 * @param settingText 显示文本控件
 */
void MenuSetting::connectCheckTimeSliderSignals(ElaText *settingText) {
    // 滑动条值变化时更新UI和计时器
    connect(_checkTimeSlider, &ElaSlider::valueChanged, this, [settingText, this](int value) {
        GLCore::transparencyCheckTime = value;
        settingText->setText(QString("透明度检查时间间隔 (10-1000ms): %1").arg(value));
        if (_glCore && _glCore->transparencyCheckTimer) {
            _glCore->transparencyCheckTimer->setInterval(value);
        }
    });

    // 滑动条释放时保存设置
    connect(_checkTimeSlider, &ElaSlider::sliderReleased, this, [this] {
        ConfigManager::getInstance().setCheckTime(GLCore::transparencyCheckTime);
    });
}

/**
 * @brief 创建开机启动设置相关UI组件
 */
void MenuSetting::createAutoStartSettingUI() {
    // 创建开机启动设置区域
    ElaScrollPageArea *settingArea = new ElaScrollPageArea(this);
    QHBoxLayout *settingLayout = new QHBoxLayout(settingArea);

    // 创建开机启动文本
    ElaText *settingText = new ElaText("开机自动启动", this);
    settingText->setWordWrap(false);
    settingText->setTextPixelSize(15);

    // 创建开机启动开关
    _autoStartSwitchButton = new ElaToggleSwitch(this);

    // 添加到布局
    settingLayout->addWidget(settingText);
    settingLayout->addStretch();
    settingLayout->addWidget(_autoStartSwitchButton);

    // 连接开关信号
    connectAutoStartSwitchSignals();

    // 保存创建的区域到类成员变量
    _autoStartArea = settingArea;
}

/**
 * @brief 创建窗口穿透设置相关UI组件
 */
void MenuSetting::createTransparentSettingUI() {
    // 创建区域
    ElaScrollPageArea *settingArea = new ElaScrollPageArea(this);
    QHBoxLayout *settingLayout = new QHBoxLayout(settingArea);

    // 创建文本
    ElaText *settingText = new ElaText("窗口始终穿透", this);
    settingText->setWordWrap(false);
    settingText->setTextPixelSize(15);

    // 创建开关
    _toggleTransparent = new ElaToggleSwitch(this);

    // 添加到布局
    settingLayout->addWidget(settingText);
    settingLayout->addStretch();
    settingLayout->addWidget(_toggleTransparent);

    // 获取当前状态并设置
    bool currentTransparent = ConfigManager::getInstance().getAlwaysTransparent();
    _toggleTransparent->setIsToggled(currentTransparent);

    // 连接信号
    connect(_toggleTransparent, &ElaToggleSwitch::toggled, this, [this](bool checked) {
        ConfigManager::getInstance().setAlwaysTransparent(checked);
    });

    _transparentArea = settingArea;
}

/**
 * @brief 连接开机启动开关信号
 */
void MenuSetting::connectAutoStartSwitchSignals() {
    // 获取当前状态并设置开关
    bool currentAutoStart = ConfigManager::getInstance().isAutoStart();
    _autoStartSwitchButton->setIsToggled(currentAutoStart);
    // 开关状态变化时更新设置
    connect(_autoStartSwitchButton, &ElaToggleSwitch::toggled, this, [this](bool checked) {
        // 保存到配置
        ConfigManager::getInstance().setAutoStart(checked);
        QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                           QSettings::NativeFormat);
        QString application_name = QCoreApplication::applicationName();
        QString application_path = QCoreApplication::applicationFilePath();
        if (checked) {
            application_path.replace(".exe", ".lnk");
            if (!QFile::exists(application_path)) {
                QFile::link(QCoreApplication::applicationFilePath(), application_name + ".lnk");
            }
            QString strAppPath = QDir::toNativeSeparators(application_path);
            settings.setValue(application_name, strAppPath);

            qDebug() << "开机启动已启用!";
        } else {
            settings.remove(application_name);
            qDebug() << "开机启动已禁用!";
        }
    });
}

/**
 * @brief 更新定时器间隔
 * @param fps 新的FPS值
 */
void MenuSetting::updateTimers(int fps) {
    if (_glCore && _glCore->renderTimer) {
        _glCore->renderTimer->setInterval((1.0 / fps) * 1000);
        _glCore->eyeTrackingTimer->setInterval((1.0 / fps) * 1000);
    }
}

/**
 * @brief 设置中央控件和布局
 */
void MenuSetting::setupCentralWidget() {
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("设置");

    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(_fpsSettingArea);
    centerLayout->addSpacing(15);
    centerLayout->addWidget(_volumeSettingArea);
    centerLayout->addWidget(_checkTimeSettingArea);
    centerLayout->addWidget(_transparentArea);
    centerLayout->addWidget(_autoStartArea);

    addCentralWidget(centralWidget, true, true, 0);
}
