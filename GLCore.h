#pragma once

#include <QtWidgets/QWidget>
#include <QOpenGLWidget>


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


    // ��д����
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;


private:
    bool isLeftPressed; // �������Ƿ���
    bool isRightPressed;// ����Ҽ��Ƿ���
    QPoint currentPos;  // ��ǰ���λ��

};