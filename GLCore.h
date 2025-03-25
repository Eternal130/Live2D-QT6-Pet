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


    // ��д����
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    // ������λ���Ƿ���Ҫ��͸
    void checkMouseOverTransparentPixel();

    // ���ô�����괩͸����
    void setWindowTransparent(bool transparent);

private:
    bool isLeftPressed{}; // �������Ƿ���
    bool isRightPressed{};// ����Ҽ��Ƿ���
    QPoint currentPos;  // ��ǰ���λ��
    QTimer* renderTimer{}; // ��Ⱦ��ʱ��
    QTimer* transparencyCheckTimer{}; // ���͸���ȵĶ�ʱ��
    QTimer* eyeTrackingTimer{};  // ����׷�ٶ�ʱ��
    bool isCurrentlyTransparent{false}; // ��ǰ�����Ƿ�͸

};