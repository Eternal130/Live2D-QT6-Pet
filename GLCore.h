#pragma once

// 标准库头文件（如果有）

// Cubism头文件
#include "LAppModel.hpp"

// Qt库头文件
#include <QtWidgets/QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QSystemTrayIcon>

// 项目头文件
#include "ElaMenu.h"

/**
 * @brief OpenGL渲染核心类，负责窗口显示、模型渲染和用户交互
 */
class GLCore : public QOpenGLWidget {
    Q_OBJECT

public:
    //---------------------------------------------------------------------
    // 构造与析构
    //---------------------------------------------------------------------
    explicit GLCore(QWidget *parent = nullptr);

    GLCore(int width, int height, QWidget *parent = nullptr);

    ~GLCore() override;

    //---------------------------------------------------------------------
    // 初始化与设置
    //---------------------------------------------------------------------
    void initWindowAttributes();

    void initTimers();

    void releaseTimers();

    void initMenus();

    void setupTrayIcon();

    void scanAndLoadModels();

    //---------------------------------------------------------------------
    // 模型相关
    //---------------------------------------------------------------------
    void generateModelMask();

    void updateEyeTracking();

    int getImageWidth() const { return image.width(); }

    int getImageHeight() const { return image.height(); }

    //---------------------------------------------------------------------
    // 窗口穿透相关
    //---------------------------------------------------------------------
    void checkMouseOverTransparentPixel();

    void setWindowTransparent(bool transparent);

    bool isPointInMask(const QPoint &point) const;

    //---------------------------------------------------------------------
    // 缩放相关
    //---------------------------------------------------------------------
    void handleScaling(bool zoomIn);

    //---------------------------------------------------------------------
    // OpenGL重写函数
    //---------------------------------------------------------------------
    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int w, int h) override;

    //---------------------------------------------------------------------
    // 事件处理函数
    //---------------------------------------------------------------------
    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    //---------------------------------------------------------------------
    // 静态成员变量
    //---------------------------------------------------------------------
    static int fps; // 刷新率

    //---------------------------------------------------------------------
    // 公共成员变量
    //---------------------------------------------------------------------
    QTimer *eyeTrackingTimer{}; // 视线追踪定时器
    QTimer *renderTimer{}; // 渲染定时器
private:
    //---------------------------------------------------------------------
    // 定时器相关
    //---------------------------------------------------------------------
    QTimer *transparencyCheckTimer{}; // 检查透明度的定时器

    //---------------------------------------------------------------------
    // 鼠标交互相关
    //---------------------------------------------------------------------
    bool isLeftPressed{}; // 鼠标左键是否按下
    bool isRightPressed{}; // 鼠标右键是否按下
    bool isMiddlePressed{}; // 鼠标中键是否按下
    QPoint currentPos; // 当前鼠标位置
    bool isCurrentlyTransparent{false}; // 当前窗口是否穿透

    //---------------------------------------------------------------------
    // 模型与遮罩相关
    //---------------------------------------------------------------------
    QImage image; // 存储模型碰撞遮盖

    //---------------------------------------------------------------------
    // 菜单与UI相关
    //---------------------------------------------------------------------
    ElaMenu *_homeMenu{nullptr}; // 右键菜单
    ElaMenu *_selectModelMenu; // 模型选择菜单
    QAction *_menu; // 设置菜单
    QAction *_close; // 关闭程序
    QSystemTrayIcon *_trayIcon; // 任务栏图标
};
