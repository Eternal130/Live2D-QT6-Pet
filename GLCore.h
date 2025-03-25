#pragma once

#include <QtWidgets/QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>


class GLCore : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit GLCore(QWidget *parent = nullptr);
    GLCore(int width, int height, QWidget* parent = nullptr);
    ~GLCore() override;

    void updateEyeTracking();


    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;


    // 重写函数
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    // 检查鼠标位置是否需要穿透
    void checkMouseOverTransparentPixel();

    // 设置窗口鼠标穿透属性
    void setWindowTransparent(bool transparent);

private:
    bool isLeftPressed{}; // 鼠标左键是否按下
    bool isRightPressed{};// 鼠标右键是否按下
    QPoint currentPos;  // 当前鼠标位置
    QTimer* renderTimer{}; // 渲染定时器
    QTimer* transparencyCheckTimer{}; // 检查透明度的定时器
    QTimer* eyeTrackingTimer{};  // 视线追踪定时器
    bool isCurrentlyTransparent{false}; // 当前窗口是否穿透

};