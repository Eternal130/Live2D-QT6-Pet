#include "LAppDelegate.hpp" // ����Ҫ���ڵ�һ������Ȼ����ʵ��
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"
#include "GLCore.h"

#include <LAppModel.hpp>
#include <QTimer>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>

// Windows API
#include <windows.h>



GLCore::GLCore(QWidget *parent)
    : QOpenGLWidget(parent)
{
    
}

GLCore::GLCore(int width, int height, QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFixedSize(width, height);
    
    //this->setAttribute(Qt::WA_DeleteOnClose);       // ���ڹر�ʱ�Զ��ͷ��ڴ�
    this->setWindowFlag(Qt::FramelessWindowHint); // �����ޱ߿򴰿�
    this->setWindowFlag(Qt::WindowStaysOnTopHint); // ���ô���ʼ���ڶ���
    //this->setWindowFlag(Qt::Tool); // ����Ӧ�ó���ͼ��
    this->setAttribute(Qt::WA_TranslucentBackground); // ���ô��ڱ���͸��


    // �������׷��
    this->setMouseTracking(true);

    renderTimer = new QTimer();
    connect(renderTimer, &QTimer::timeout, [=]() {
        update();
        });
    renderTimer->start((1.0 / 60) * 1000);    // 60FPS


    // ͸���ȼ�鶨ʱ�� - ÿ100ms���һ��
    transparencyCheckTimer = new QTimer(this);
    connect(transparencyCheckTimer, &QTimer::timeout, this, &GLCore::checkMouseOverTransparentPixel);
    transparencyCheckTimer->start(100); // ��Ƶ�ʼ�����������
    // ����׷�ټ�ʱ��
    eyeTrackingTimer = new QTimer(this);
    connect(eyeTrackingTimer, &QTimer::timeout, [this]() {
        updateEyeTracking();
    });
    eyeTrackingTimer->start((1.0 / 60) * 1000); // 60fps
}

GLCore::~GLCore()
{
    if (renderTimer) {
        renderTimer->stop();
        delete renderTimer;
    }
    if (transparencyCheckTimer) {
        transparencyCheckTimer->stop();
        delete transparencyCheckTimer;
    }
    if (eyeTrackingTimer) {
        eyeTrackingTimer->stop();
        delete eyeTrackingTimer;
    }
}


// ����׷��
void GLCore::updateEyeTracking()
{
    // ���ȫ�����λ��
    QPoint globalPos = QCursor::pos();
    QPoint localPos = mapFromGlobal(globalPos);

    // ����live2dģ��
    LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(
        localPos.x(),
        localPos.y()
    );
}
void GLCore::setWindowTransparent(bool transparent)
{
    if (isCurrentlyTransparent == transparent)
        return; // ״̬δ�䣬����Ҫ����

    isCurrentlyTransparent = transparent;

    // ʹ��Windows API����/ȡ����͸
    HWND hwnd = (HWND)this->winId();

    if (transparent) {
        // ����WS_EX_TRANSPARENTʹ���ڶ������͸��
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    } else {
        // �Ƴ�WS_EX_TRANSPARENT��־
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
    }
}


void GLCore::checkMouseOverTransparentPixel()
{
    // ��ȡȫ�����λ��
    QPoint globalPos = QCursor::pos();
    QPoint localPos = mapFromGlobal(globalPos);
    // �����Ƿ��ڴ�����
    if (!rect().contains(localPos)) {
        setWindowTransparent(true); // ����ڴ�����ʱӦ��͸
        return;
    }

    auto* manager = LAppLive2DManager::GetInstance();
    auto* model = manager->GetModel(0);

    if (model && model->HitTest("*",
            static_cast<float>(localPos.x()) / width() * 3.0f - 1.5f,
            (1.0f - static_cast<float>(localPos.y()) / height()) * 2.0f - 1.0f)) {
        setWindowTransparent(false);
        return;
            }
    setWindowTransparent(true);
}

void GLCore::mouseMoveEvent(QMouseEvent* event)
{

    if (isLeftPressed) {
        this->move(event->pos() - this->currentPos + this->pos());
    }

    

    // �����¼��������ݣ�������¼����ݸ������ڣ�ʵ������϶��ޱ߿򴰿�
    event->ignore();
}

void GLCore::mousePressEvent(QMouseEvent* event)
{
    int x = qRound(event->position().x());
    int y = qRound(event->position().y());
    qDebug() << isCurrentlyTransparent;
    // ���ݵ�ǰ��͸״̬������
    if (!isCurrentlyTransparent) {
        // ֪ͨLive2Dģ��
        LAppDelegate::GetInstance()->GetView()->OnTouchesBegan(x, y);

        if (event->button() == Qt::LeftButton) {
            this->isLeftPressed = true;
            this->currentPos = event->pos();
        }
        if (event->button() == Qt::RightButton) {
            LAppLive2DManager::GetInstance()->LoadModelFromPath("Resources/Mao/", "Mao.model3.json");
            this->isRightPressed = true;
        }

        event->accept();
    } else {
        event->ignore();
    }
}

void GLCore::mouseReleaseEvent(QMouseEvent* event)
{
    if (!isCurrentlyTransparent) {
        int x = qRound(event->position().x());
        int y = qRound(event->position().y());

        LAppDelegate::GetInstance()->GetView()->OnTouchesEnded(x, y);

        if (event->button() == Qt::LeftButton) {
            isLeftPressed = false;
        }
        if (event->button() == Qt::RightButton) {
            isRightPressed = false;
        }

        event->accept();
    } else {
        event->ignore();
    }
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