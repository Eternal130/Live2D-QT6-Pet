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
#include <QPainter>
#include <QThread>
#include <QtConcurrent/QtConcurrent>

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

    // ��ȡģ�͵�����Drawable����
    Live2D::Cubism::Framework::CubismModel* cubismModel = model->GetModel();
    const int drawableCount = cubismModel->GetDrawableCount();

    // ��������Drawable�жϵ��λ��
    for (int i = 0; i < drawableCount; i++) {
        // ��ȡ��Drawable��ID
        const Live2D::Cubism::Framework::CubismIdHandle drawableId = cubismModel->GetDrawableId(i);

        // ʹ��IsHit�������õ��Ƿ������Drawable��
        if (model->IsHit(drawableId, x, y)) {
            return true;
        }
    }

    return false;
}
void GLCore::generateModelMask()
{
    // ����һ����Alphaͨ����QImage�����ڸ�
    QImage maskImage(width(), height(), QImage::Format_ARGB32);
    maskImage.fill(Qt::transparent);

    // ��ȡ��ǰģ��
    auto* manager = LAppLive2DManager::GetInstance();
    auto* model = manager->GetModel(0);

    if (!model) {
        qDebug() << "ûģ�ͣ����Ʋ����ڸ�";
        return;
    }

    // �ڴ����ϲ���ģ�͵�ÿ����
    const int step = 5; // ÿ5�����ز���һ��
    const int threadCount = QThread::idealThreadCount();
    QVector<QFuture<void>> futures;

    // �ָ��������߳�
    for (int threadIndex = 0; threadIndex < threadCount; threadIndex++) {
        int startY = threadIndex * height() / threadCount;
        int endY = (threadIndex + 1) * height() / threadCount;

        futures.append(QtConcurrent::run([&maskImage, model, step, startY, endY, this]() {
            for (int y = startY; y < endY; y += step) {
                for (int x = 0; x < width(); x += step) {
                    float modelX = static_cast<float>(x) / width() * 3.0f - 1.5f;
                    float modelY = (1.0f - static_cast<float>(y) / height()) * 2.0f - 1.0f;

                    if (IsModelRendered(model, modelX, modelY)) {
                        for (int dx = 0; dx < step && x + dx < width(); dx++) {
                            for (int dy = 0; dy < step && y + dy < height(); dy++) {
                                maskImage.setPixelColor(x + dx, y + dy, QColor(255, 0, 0, 64));
                            }
                        }
                    }
                }
            }
        }));
    }

    // �ȴ������߳����
    for (auto &future : futures)
        future.waitForFinished();

    // �����ڸ�
    image = maskImage;

    update(); // ���������ڸ�
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
// �����Ƿ����ڸ���
bool GLCore::isPointInMask(const QPoint& point) const
{
    // ����ڸ��Ƿ����
    if (image.isNull()) {
        return false;
    }

    // ��鵱ǰ���Ƿ��ڴ�����
    if (!rect().contains(point)) {
        return false;
    }

    // ��ȡ��ǰλ������
    QColor pixelColor = image.pixelColor(point);

    // ͸���ȴ���0��˵����ǰ�����ڸ���
    return pixelColor.alpha() > 0;
}
/*
 * ��鵱ǰλ���Ƿ���Դ�͸
 * ��ʵ�⣬����ģ���˶���ģ�ͻ�������Ļ�仯�ģ����Ǳ仯�����Ĭ��״̬���󣬶���Ƶ���ػ�������������޴����ܿ���
 * ����ģ���ڸǽ�����һ�Σ�֮���ж��Ƿ�͸ʱʵ�����жϵ�ǰ���Ƿ����ڸ���
 */
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

    if (isPointInMask(localPos)) {
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

    

    // �����¼��������ݣ�������¼����ݸ������ڣ�ʵ������϶��ޱ߿򴰿�
    event->ignore();
}

void GLCore::mousePressEvent(QMouseEvent* event)
{
    int x = qRound(event->position().x());
    int y = qRound(event->position().y());
    // ���ݵ�ǰ��͸״̬������
    if (!isCurrentlyTransparent) {
        // ֪ͨLive2Dģ��
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
    
    // ѡ��ģ��
}

void GLCore::paintGL()
{
    LAppDelegate::GetInstance()->update();
// ��debugģʽ�»���ģ����ײ�ڸ�
#ifdef QT_DEBUG
    if (!image.isNull()) {
        QPainter painter(this);
        painter.drawImage(0, 0, image);
        //������뿴���ڸ���ģ���˶��ı仯������������������generateModelMask();����������ܿ����������ܺ���û�Ż��ڸǻ����й�
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