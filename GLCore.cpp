#include "LAppDelegate.hpp" // 必须要放在第一个，不然包老实的
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

GLCore::GLCore(int width, int height, QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFixedSize(width, height);
    
    //this->setAttribute(Qt::WA_DeleteOnClose);       // 窗口关闭时自动释放内存
    //this->setWindowFlag(Qt::FramelessWindowHint); // 设置无边框窗口
    this->setWindowFlag(Qt::WindowStaysOnTopHint); // 设置窗口始终在顶部
    //this->setWindowFlag(Qt::Tool); // 隐藏应用程序图标
    this->setAttribute(Qt::WA_TranslucentBackground); // 设置窗口背景透明

    

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
    LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(
    qRound(event->position().x()),
    qRound(event->position().y())
);

    if (isLeftPressed) {
        this->move(event->pos() - this->currentPos + this->pos());
    }

    

    // 允许事件继续传递，将鼠标事件传递给主窗口，实现鼠标拖动无边框窗口
    event->ignore();
}

void GLCore::mousePressEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesBegan(
    qRound(event->position().x()),
    qRound(event->position().y())
);

    if (event->button() == Qt::LeftButton) {
        this->isLeftPressed = true;
        this->currentPos = event->pos();
    }
    // TODO: 右键菜单等
    if (event->button() == Qt::RightButton) {
        
        // 设置窗口大小
        //LAppDelegate::GetInstance()->resize(400, 400);
        //this->setFixedSize(400, 400);
        LAppLive2DManager::GetInstance()->LoadModelFromPath("Resources/Mao/", "Mao.model3.json");

        this->isRightPressed = true;
    }


    
    // 允许事件继续传递
    event->ignore();
}

void GLCore::mouseReleaseEvent(QMouseEvent* event)
{
    LAppDelegate::GetInstance()->GetView()->OnTouchesEnded(
    qRound(event->position().x()),
    qRound(event->position().y())
);

    if (event->button() == Qt::LeftButton) {
        isLeftPressed = false;
    }
    if (event->button() == Qt::RightButton) {
        isRightPressed = false;
    }

    
    // 允许事件继续传递
    event->ignore();
}

void GLCore::initializeGL()
{
    LAppDelegate::GetInstance()->Initialize(this);
    
    // 选择模型
}

void GLCore::paintGL()
{
    LAppDelegate::GetInstance()->update();
    
}

void GLCore::resizeGL(int w, int h)
{
    LAppDelegate::GetInstance()->resize(w, h);
}