#include "LAppDelegate.hpp" // 必须要放在第一个，不然包老实的
#include "LAppView.hpp"
#include "LAppPal.hpp"
#include "LAppLive2DManager.hpp"
#include "GLCore.h"

#include "LAppModel.hpp"
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>
#include <QPainter>
#include <QtConcurrent/QtConcurrent>

// Windows API
#include <windows.h>

#include "ConfigManager.h"
#include "menu.h"


int GLCore::fps = 60; // 初始化静态成员变量
GLCore::GLCore(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

GLCore::GLCore(int width, int height, QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFixedSize(width, height);
    
    this->setAttribute(Qt::WA_DeleteOnClose);       // 窗口关闭时自动释放内存
    this->setWindowFlag(Qt::FramelessWindowHint); // 设置无边框窗口
    this->setWindowFlag(Qt::WindowStaysOnTopHint); // 设置窗口始终在顶部
    this->setWindowFlag(Qt::Tool); // 隐藏应用程序图标
    this->setAttribute(Qt::WA_TranslucentBackground); // 设置窗口背景透明


    // 设置鼠标追踪
    this->setMouseTracking(true);

    renderTimer = new QTimer();
    connect(renderTimer, &QTimer::timeout, [=]() {
        update();
        });
    renderTimer->start(1.0 / ConfigManager::getInstance().getFps() * 1000);    // 60FPS


    // 透明度检查定时器 - 每100ms检查一次
    transparencyCheckTimer = new QTimer(this);
    connect(transparencyCheckTimer, &QTimer::timeout, this, &GLCore::checkMouseOverTransparentPixel);
    transparencyCheckTimer->start(100); // 低频率检查以提高性能
    // 视线追踪计时器
    eyeTrackingTimer = new QTimer(this);
    connect(eyeTrackingTimer, &QTimer::timeout, [this]() {
        updateEyeTracking();
    });
    eyeTrackingTimer->start(1.0 / ConfigManager::getInstance().getFps() * 1000); // 60fps
    QTimer::singleShot(500, this, &GLCore::generateModelMask);// 生成模型遮罩
    _homeMenu = new ElaMenu(this);
    _selectModelMenu = _homeMenu->addMenu(ElaIconType::Cubes, "切换模型");
    _menu = _homeMenu->addElaIconAction(ElaIconType::GearComplex, "菜单");
    connect(_menu, &QAction::triggered, [this]() {
        Menu* menu = new Menu(this);
        menu->moveToCenter();
        menu->setAttribute(Qt::WA_DeleteOnClose);  // 窗口关闭时自动释放内存
        menu->show();
            });
    // 关闭程序
    _close = _homeMenu->addElaIconAction(ElaIconType::X, "关闭");
    //感觉这样关会有问题，但对qt不熟，只能这样了
    connect(_close, &QAction::triggered, [this]() {
        this->close();
        });
    scanAndLoadModels();  // 调用新函数扫描并加载模型
    setupTrayIcon();
}

void GLCore::setupTrayIcon()
{
    // 创建系统托盘图标
    _trayIcon = new QSystemTrayIcon(this);

    // 设置图标
    QIcon icon("Resources/icon_gear.png");
    _trayIcon->setIcon(icon);
    _trayIcon->setToolTip("QT Pet");

    // 设置托盘图标的上下文菜单为_homeMenu
    _trayIcon->setContextMenu(_homeMenu);

    // 显示托盘图标
    _trayIcon->show();
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
    // 创建一个带Alpha通道的QImage用于遮盖
    QImage maskImage(width(), height(), QImage::Format_ARGB32);
    maskImage.fill(Qt::transparent);

    // 获取当前模型
    auto* manager = LAppLive2DManager::GetInstance();
    auto* model = manager->GetModel(0);

    if (!model) {
        qDebug() << "没模型，绘制不了遮盖";
        return;
    }

    // 在窗口上采样模型的每个点
    const int step = 5; // 每5个像素采样一次
    const int threadCount = QThread::idealThreadCount();
    QVector<QFuture<void>> futures;

    // 分割工作给多个线程
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

    // 等待所有线程完成
    for (auto &future : futures)
        future.waitForFinished();

    // 保存遮盖
    image = maskImage;

    update(); // 触发绘制遮盖
}

void GLCore::scanAndLoadModels()
{
    // 获取Resources目录
    QDir resourcesDir("Resources");
    if (!resourcesDir.exists()) {
        qDebug() << "Resources目录不存在!";
        return;
    }

    // 获取所有子目录
    QStringList subdirs = resourcesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // 检查每个子目录中的model3.json文件
    for (const QString& subdir : subdirs) {
        QDir modelDir(resourcesDir.filePath(subdir));
        QStringList jsonFiles = modelDir.entryList(QStringList() << "*.model3.json", QDir::Files);

        // 检查是否只有一个model3.json文件
        if (jsonFiles.count() == 1) {
            QString modelName = subdir;
            QString jsonFileName = jsonFiles.first();

            // 添加菜单选项
            QAction* modelAction = _selectModelMenu->addAction(modelName);

            // 连接到加载模型的函数
            connect(modelAction, &QAction::triggered, [this, modelName, jsonFileName]() {
                QString modelPath = "Resources/" + modelName + "/";
                LAppLive2DManager::GetInstance()->LoadModelFromPath(modelPath.toStdString().c_str(),
                                                                  jsonFileName.toStdString().c_str());
                ConfigManager::getInstance().setName(modelName);
                generateModelMask();
            });
        }
    }
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
// 检查点是否在遮盖内
bool GLCore::isPointInMask(const QPoint& point) const
{
    // 检查遮盖是否存在
    if (image.isNull()) {
        return false;
    }

    // 检查当前点是否在窗口内
    if (!rect().contains(point)) {
        return false;
    }

    // 获取当前位置像素
    QColor pixelColor = image.pixelColor(point);

    // 透明度大于0，说明当前点在遮盖上
    return pixelColor.alpha() > 0;
}
/*
 * 检查当前位置是否可以穿透
 * 经实测，随着模型运动，模型绘制区域是会变化的，但是变化相对于默认状态不大，而且频繁重绘绘制区域会带来巨大性能开销
 * 所以模型遮盖仅绘制一次，之后判断是否穿透时实际是判断当前点是否在遮盖上
 */
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
            _homeMenu->popup(event->globalPosition().toPoint());
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
            ConfigManager::getInstance().setX(event->pos().x() - this->currentPos.x() + this->pos().x());
            ConfigManager::getInstance().setY(event->pos().y() - this->currentPos.y() + this->pos().y());
        }

        event->accept();
    } else {
        event->ignore();
    }
}
void GLCore::wheelEvent(QWheelEvent* event)
{
    // 检查是否按住Ctrl键
    if (event->modifiers() & Qt::ControlModifier) {
        // 防止过于频繁的滚轮事件处理
        static QElapsedTimer debounceTimer;
        if (!debounceTimer.isValid()) {
            debounceTimer.start();
        } else if (debounceTimer.elapsed() < 50) { // 50ms防抖
            event->accept();
            return;
        }
        debounceTimer.restart();

        // 获取当前窗口信息
        QSize currentSize = size();
        QPoint currentPos = pos();

        // 使用更精确的浮点数计算
        double centerX = currentPos.x() + currentSize.width() / 2.0;
        double centerY = currentPos.y() + currentSize.height() / 2.0;

        // 平滑缩放因子
        double scaleFactor = event->angleDelta().y() > 0 ? 1.05 : 0.95;

        // 精确计算新尺寸
        int newWidth = qMax(100, (int)(currentSize.width() * scaleFactor));
        int newHeight = qMax(100, (int)(currentSize.height() * scaleFactor));

        // 精确计算新位置
        int newX = qRound(centerX - newWidth / 2.0);
        int newY = qRound(centerY - newHeight / 2.0);

        // 设置窗口的几何形状
        // 缩放时会明显抖动，实在是不知道怎么修了，只能这样用了
        setFixedSize(newWidth, newHeight);
        move(newX, newY);
        // 存储新的窗口位置和大小到配置文件
        ConfigManager::getInstance().setHeight(newHeight);
        ConfigManager::getInstance().setWidth(newWidth);
        ConfigManager::getInstance().setX(newX);
        ConfigManager::getInstance().setY(newY);

        // 更新模型遮罩
        if (!image.isNull()) {
            image = image.scaled(newWidth, newHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
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
    if (!image.isNull()) {
        QPainter painter(this);
        painter.drawImage(0, 0, image);
        //如果你想看看遮盖随模型运动的变化情况，可以在这里加上generateModelMask();不过这样会很卡，不过可能和我没优化遮盖绘制有关
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