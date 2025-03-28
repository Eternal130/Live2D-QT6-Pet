#include "MenuSetting.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>

#include "ConfigManager.h"
#include "ElaScrollPageArea.h"
#include "ElaSlider.h"
#include "ElaText.h"
#include "ElaTheme.h"
#include "GLCore.h"

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
    _settingArea = settingArea;
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
    centerLayout->addWidget(_settingArea);

    addCentralWidget(centralWidget, true, true, 0);
}
