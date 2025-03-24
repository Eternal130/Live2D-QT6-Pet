#pragma once

#include <QtWidgets/QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>


class GLCore : public QOpenGLWidget
{
    Q_OBJECT

public:
    GLCore(QWidget *parent = nullptr);
    GLCore(int width, int height, QWidget* parent = nullptr);
    ~GLCore();


    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;


    // 重写函数
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;


private:
    bool isLeftPressed{}; // 鼠标左键是否按下
    bool isRightPressed{};// 鼠标右键是否按下
    QPoint currentPos;  // 当前鼠标位置

};