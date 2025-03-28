#ifndef MENUSETTING_H
#define MENUSETTING_H

#include "ElaScrollPage.h"

class ElaSlider;
class GLCore;
class ElaText;
class ElaScrollPageArea;

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
     * @brief 更新定时器间隔
     * @param fps 新的FPS值
     */
    void updateTimers(int fps);

    /**
     * @brief 设置中央控件和布局
     */
    void setupCentralWidget();

    ElaSlider *_fpsSlider{nullptr}; // FPS滑动条控件
    GLCore *_glCore{nullptr}; // OpenGL核心对象指针
    ElaScrollPageArea *_settingArea{nullptr}; // 设置区域
};

#endif // MENUSETTING_H
