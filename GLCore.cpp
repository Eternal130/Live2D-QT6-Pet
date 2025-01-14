#include "LAppDelegate.hpp" // ����Ҫ���ڵ�һ������Ȼ����ʵ��
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppDefine.hpp"
#include "GLCore.h"
#include <QTimer>
#include <QMouseEvent>




GLCore::GLCore(QWidget *parent)
    : QOpenGLWidget(parent)
{
    
}

GLCore::GLCore(int w, int h, QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFixedSize(w, h);
    
    //this->setAttribute(Qt::WA_DeleteOnClose);       // ���ڹر�ʱ�Զ��ͷ��ڴ�
    //this->setWindowFlag(Qt::FramelessWindowHint); // �����ޱ߿򴰿�
    this->setWindowFlag(Qt::WindowStaysOnTopHint); // ���ô���ʼ���ڶ���
    //this->setWindowFlag(Qt::Tool); // ����Ӧ�ó���ͼ��
    this->setAttribute(Qt::WA_TranslucentBackground); // ���ô��ڱ���͸��

    

    QTimer* timer = new QTimer();
    connect(timer, &QTimer::timeout, [=]() {
        update();
        });
    timer->start((1.0 / 60) * 1000);    // 60FPS



}

GLCore::~GLCore()
{
    
}



void GLCore::mouseMoveEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(event->x(), event->y());

    if (isLeftPressed) {
        this->move(event->pos() - this->currentPos + this->pos());
    }

    

    // �����¼��������ݣ�������¼����ݸ������ڣ�ʵ������϶��ޱ߿򴰿�
    event->ignore();
}

void GLCore::mousePressEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesBegan(event->x(), event->y());

    if (event->button() == Qt::LeftButton) {
        this->isLeftPressed = true;
        this->currentPos = event->pos();
    }
    // TODO: �Ҽ��˵���
    if (event->button() == Qt::RightButton) {
        
        // ���ô��ڴ�С
        //LAppDelegate::GetInstance()->resize(400, 400);
        //this->setFixedSize(400, 400);
        LAppLive2DManager::GetInstance()->LoadModelFromPath("Resources/Mao/", "Mao.model3.json");

        this->isRightPressed = true;
    }


    
    // �����¼���������
    event->ignore();
}

void GLCore::mouseReleaseEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesEnded(event->x(), event->y());

    if (event->button() == Qt::LeftButton) {
        isLeftPressed = false;
    }
    if (event->button() == Qt::RightButton) {
        isRightPressed = false;
    }

    
    // �����¼���������
    event->ignore();
}

void GLCore::initializeGL()
{
    LAppDelegate::GetInstance()->Initialize(this);
    
    // ѡ��ģ��
}

void GLCore::paintGL()
{
    LAppDelegate::GetInstance()->update();
    
}

void GLCore::resizeGL(int w, int h)
{
    LAppDelegate::GetInstance()->resize(w, h);
}