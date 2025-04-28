#ifndef MENUSETTING_H
#define MENUSETTING_H

#include "ElaScrollPage.h"

class ElaSlider;
class GLCore;
class ElaText;
class ElaScrollPageArea;
class ElaToggleSwitch;

/**
 * @brief 设置菜单类，提供应用程序设置界面
 */
class MenuSetting : public ElaScrollPage {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param glCore OpenGL核心对象指针
     * @param parent 父控件指针
     */
    Q_INVOKABLE explicit MenuSetting(GLCore *glCore = nullptr, QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MenuSetting() override;

private:
    /**
     * @brief 初始化主题变更连接
     */
    void initThemeConnection();

    /**
     * @brief 创建FPS设置相关UI组件
     */
    void createFpsSettingUI();

    /**
     * @brief 连接FPS滑动条信号
     * @param settingText FPS显示文本控件
     */
    void connectFpsSliderSignals(ElaText *settingText);

    /**
     * @brief 创建音量设置相关UI组件
     */
    void createVolumeSettingUI();

    /**
     * @brief 连接音量滑动条信号
     * @param settingText 音量显示文本控件
     */
    void connectVolumeSliderSignals(ElaText *settingText);

    /**
     * @brief 创建透明度检查时间间隔设置相关UI组件
     */
    void createCheckTimeSettingUI();

    /**
     * @brief 连接透明度检查时间间隔滑动条信号
     * @param settingText 显示文本控件
     */
    void connectCheckTimeSliderSignals(ElaText *settingText);

    /**
     * @brief 创建开机启动设置相关UI组件
     */
    void createAutoStartSettingUI();

    /**
     * @brief 创建窗口穿透设置相关UI组件
     */
    void createTransparentSettingUI();

    /**
     * @brief 连接开机启动开关信号
     */
    void connectAutoStartSwitchSignals();

    /**
     * @brief 更新定时器间隔
     * @param fps 新的FPS值
     */
    void updateTimers(int fps);

    /**
     * @brief 设置中央控件和布局
     */
    void setupCentralWidget();

    GLCore *_glCore{nullptr}; // OpenGL核心对象指针
    ElaSlider *_fpsSlider{nullptr}; // FPS滑动条控件
    ElaScrollPageArea *_fpsSettingArea{nullptr}; // FPS设置区域
    ElaSlider *_volumeSlider{nullptr};         // 音量滑动条控件
    ElaScrollPageArea *_volumeSettingArea{nullptr}; // 音量设置区域
    ElaSlider* _checkTimeSlider{nullptr}; // 透明度检查时间间隔控件
    ElaScrollPageArea* _checkTimeSettingArea{nullptr}; // 透明度检查时间间隔设置区域
    ElaToggleSwitch* _toggleTransparent{nullptr}; // 窗口穿透开关
    ElaScrollPageArea* _transparentArea{nullptr}; // 云母效果开关区域
    ElaToggleSwitch* _autoStartSwitchButton{nullptr}; // 开机启动开关
    ElaScrollPageArea* _autoStartArea{nullptr}; // 开机启动区域
};

#endif // MENUSETTING_H
