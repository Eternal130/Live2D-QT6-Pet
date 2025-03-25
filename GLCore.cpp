#include "LAppDelegate.hpp" // 必须要放在第一个，不然包老实的
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"
#include "GLCore.h"

#include <LAppModel.hpp>
#include <QTimer>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>
#include <QPainter>

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
    
    //this->setAttribute(Qt::WA_DeleteOnClose);       // 窗口关闭时自动释放内存
    this->setWindowFlag(Qt::FramelessWindowHint); // 设置无边框窗口
    this->setWindowFlag(Qt::WindowStaysOnTopHint); // 设置窗口始终在顶部
    //this->setWindowFlag(Qt::Tool); // 隐藏应用程序图标
    this->setAttribute(Qt::WA_TranslucentBackground); // 设置窗口背景透明


    // 设置鼠标追踪
    this->setMouseTracking(true);

    renderTimer = new QTimer();
    connect(renderTimer, &QTimer::timeout, [=]() {
        update();
        });
    renderTimer->start((1.0 / 60) * 1000);    // 60FPS


    // 透明度检查定时器 - 每100ms检查一次
    transparencyCheckTimer = new QTimer(this);
    connect(transparencyCheckTimer, &QTimer::timeout, this, &GLCore::checkMouseOverTransparentPixel);
    transparencyCheckTimer->start(100); // 低频率检查以提高性能
    // 视线追踪计时器
    eyeTrackingTimer = new QTimer(this);
    connect(eyeTrackingTimer, &QTimer::timeout, [this]() {
        updateEyeTracking();
    });
    eyeTrackingTimer->start((1.0 / 60) * 1000); // 60fps
    QTimer::singleShot(500, this, &GLCore::generateModelMask);
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
bool IsModelRendered(LAppModel* model, float x, float y)
{
    if (!model || !model->GetModel()) return false;

    // 获取模型的所有Drawable数量
    Live2D::Cubism::Framework::CubismModel* cubismModel = model->GetModel();
    const int drawableCount = cubismModel->GetDrawableCount();

    // 遍历所有Drawable判断点击位置
    for (int i = 0; i < drawableCount; i++) {
        // 获取该Drawable的ID
        const Live2D::Cubism::Framework::CubismIdHandle drawableId = cubismModel->GetDrawableId(i);

        // 使用IsHit方法检查该点是否在这个Drawable内
        if (model->IsHit(drawableId, x, y)) {
            return true;
        }
    }

    return false;
}
void GLCore::generateModelMask()
{
    // 创建一个透明的pixmap
    QPixmap mask(width(), height());
    mask.fill(Qt::transparent);

    QPainter painter(&mask);
    painter.setPen(QColor(255, 0, 0, 128)); // 半透明红色
    painter.setBrush(QColor(255, 0, 0, 64)); // 非常透明的红色

    // 获取当前模型
    auto* manager = LAppLive2DManager::GetInstance();
    auto* model = manager->GetModel(0);

    if (!model) {
        qDebug() << "没模型，绘制不了遮盖";
        return;
    }

    // 在窗口上采样模型的每个点
    const int step = 5; // 每5个像素采样一次
    for (int x = 0; x < width(); x += step) {
        for (int y = 0; y < height(); y += step) {
            // 转换为cubism坐标
            float modelX = static_cast<float>(x) / width() * 3.0f - 1.5f;
            float modelY = (1.0f - static_cast<float>(y) / height()) * 2.0f - 1.0f;

            // 检查当前点是否在模型上
            // if (model->HitTest("*", modelX, modelY)) {
            //     painter.drawRect(x, y, step, step);
            // }
            if (IsModelRendered(model, modelX, modelY)) {
                painter.drawRect(x, y, step, step);
            }
        }
    }

    modelMask = mask;

    update(); // 触发绘制遮盖
}
// 视线追踪
void GLCore::updateEyeTracking()
{
    // 获得全局鼠标位置
    QPoint globalPos = QCursor::pos();
    QPoint localPos = mapFromGlobal(globalPos);

    // 更新live2d模型
    LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(
        localPos.x(),
        localPos.y()
    );
}
void GLCore::setWindowTransparent(bool transparent)
{
    if (isCurrentlyTransparent == transparent)
        return; // 状态未变，不需要更新

    isCurrentlyTransparent = transparent;

    // 使用Windows API设置/取消穿透
    HWND hwnd = (HWND)this->winId();

    if (transparent) {
        // 设置WS_EX_TRANSPARENT使窗口对鼠标点击透明
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    } else {
        // 移除WS_EX_TRANSPARENT标志
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
    }
}


void GLCore::checkMouseOverTransparentPixel()
{
    // 获取全局鼠标位置
    QPoint globalPos = QCursor::pos();
    QPoint localPos = mapFromGlobal(globalPos);
    // 检查点是否在窗口内
    if (!rect().contains(localPos)) {
        setWindowTransparent(true); // 鼠标在窗口外时应穿透
        return;
    }

    auto* manager = LAppLive2DManager::GetInstance();
    auto* model = manager->GetModel(0);

    if (model && IsModelRendered(model,
            static_cast<float>(localPos.x()) / width() * 3.0f - 1.5f,
            (1.0f - static_cast<float>(localPos.y()) / height()) * 2.0f - 1.0f)) {
        setWindowTransparent(false);
        return;
            }
    setWindowTransparent(true);
}

void GLCore::mouseMoveEvent(QMouseEvent* event)
{

    if (isMiddlePressed) {
        this->move(event->pos() - this->currentPos + this->pos());
    }

    

    // 允许事件继续传递，将鼠标事件传递给主窗口，实现鼠标拖动无边框窗口
    event->ignore();
}

void GLCore::mousePressEvent(QMouseEvent* event)
{
    int x = qRound(event->position().x());
    int y = qRound(event->position().y());
    // 根据当前穿透状态处理点击
    if (!isCurrentlyTransparent) {
        // 通知Live2D模型
        LAppDelegate::GetInstance()->GetView()->OnTouchesBegan(x, y);

        if (event->button() == Qt::LeftButton) {
            this->isLeftPressed = true;
        }
        if (event->button() == Qt::MiddleButton) {
            this->isMiddlePressed = true;
            this->currentPos = event->pos();
        }
        if (event->button() == Qt::RightButton) {
            LAppLive2DManager::GetInstance()->LoadModelFromPath("Resources/Mao/", "Mao.model3.json");
            generateModelMask();
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
        if (event->button() == Qt::MiddleButton) {
            isMiddlePressed = false;
        }

        event->accept();
    } else {
        event->ignore();
    }
}

void GLCore::initializeGL()
{
    LAppDelegate::GetInstance()->Initialize(this);
    
    // 选择模型
}

void GLCore::paintGL()
{
    LAppDelegate::GetInstance()->update();
// 在debug模式下绘制模型碰撞遮盖
#ifdef QT_DEBUG
    if (!modelMask.isNull()) {
        QPainter painter(this);
        painter.drawPixmap(0, 0, modelMask);
    } else {
        generateModelMask();
    }
#else

#endif
}

void GLCore::resizeGL(int w, int h)
{
    LAppDelegate::GetInstance()->resize(w, h);
}