#pragma once

#include <QtWidgets/QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QSystemTrayIcon>
#include "ElaMenu.h"


class GLCore : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit GLCore(QWidget *parent = nullptr);
    GLCore(int width, int height, QWidget* parent = nullptr);

    void setupTrayIcon();

    ~GLCore() override;
    static int fps;//刷新率
    QTimer* renderTimer{}; // 渲染定时器

    void generateModelMask();

    void scanAndLoadModels();

    void updateEyeTracking();


    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void wheelEvent(QWheelEvent *event);


    // 重写函数
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    // 检查鼠标位置是否需要穿透
    void checkMouseOverTransparentPixel();

    // 设置窗口鼠标穿透属性
    void setWindowTransparent(bool transparent);

    bool isPointInMask(const QPoint &point) const;

private:
    bool isLeftPressed{}; // 鼠标左键是否按下
    bool isRightPressed{};// 鼠标右键是否按下
    bool isMiddlePressed{}; // 鼠标中键是否按下
    QPoint currentPos;  // 当前鼠标位置
    QTimer* transparencyCheckTimer{}; // 检查透明度的定时器
    QTimer* eyeTrackingTimer{};  // 视线追踪定时器
    bool isCurrentlyTransparent{false}; // 当前窗口是否穿透
    QImage image;// 存储模型碰撞遮盖
    ElaMenu* _homeMenu{nullptr};// 右键菜单
    ElaMenu* _selectModelMenu; // 模型选择菜单
    QAction* _menu;// 设置菜单
    QAction* _close;// 关闭程序
    QSystemTrayIcon* _trayIcon;// 任务栏图标
};