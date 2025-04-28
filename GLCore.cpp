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

//---------------------------------------------------------------------
// 静态成员初始化
//---------------------------------------------------------------------
int GLCore::fps = 60;
bool firstGL = true;

//---------------------------------------------------------------------
// 构造与析构
//---------------------------------------------------------------------
GLCore::GLCore(QWidget *parent)
    : QOpenGLWidget(parent)
{
}

GLCore::GLCore(int width, int height, QWidget *parent)
    : QOpenGLWidget(parent)
{
    // 设置窗口大小
    setFixedSize(width, height);

    // 初始化窗口属性
    initWindowAttributes();

    // 初始化定时器
    initTimers();

    // 初始化菜单
    initMenus();

    // 加载模型
    scanAndLoadModels();

    // 设置系统托盘
    setupTrayIcon();

    // 启动时根据配置设置窗口穿透
    setWindowTransparent(ConfigManager::getInstance().getAlwaysTransparent());
}

GLCore::~GLCore()
{
    // 停止并释放所有定时器
    releaseTimers();
    _trayIcon->hide();
    delete _trayIcon;
}

//---------------------------------------------------------------------
// 初始化相关函数
//---------------------------------------------------------------------
void GLCore::initWindowAttributes()
{
    this->setAttribute(Qt::WA_DeleteOnClose);       // 窗口关闭时自动释放内存
    this->setWindowFlag(Qt::FramelessWindowHint);   // 设置无边框窗口
    this->setWindowFlag(Qt::WindowStaysOnTopHint);  // 设置窗口始终在顶部
    this->setWindowFlag(Qt::Tool);                  // 隐藏应用程序图标
    this->setAttribute(Qt::WA_TranslucentBackground); // 设置窗口背景透明

    // 启用鼠标追踪
    this->setMouseTracking(true);
}

void GLCore::initTimers()
{
    // 渲染定时器 - 控制画面刷新
    renderTimer = new QTimer(this);
    connect(renderTimer, &QTimer::timeout, this, [this]() {
        update();
    });
    renderTimer->start(1000.0 / ConfigManager::getInstance().getFps());

    // 透明度检查定时器 - 控制鼠标穿透
    transparencyCheckTimer = new QTimer(this);
    connect(transparencyCheckTimer, &QTimer::timeout, this, &GLCore::checkMouseOverTransparentPixel);
    transparencyCheckTimer->start(100); // 低频率检查以提高性能

    // 视线追踪定时器 - 控制模型眼睛跟随鼠标
    eyeTrackingTimer = new QTimer(this);
    connect(eyeTrackingTimer, &QTimer::timeout, this, &GLCore::updateEyeTracking);
    eyeTrackingTimer->start(1000.0 / ConfigManager::getInstance().getFps());
}

void GLCore::releaseTimers()
{
    if (renderTimer) {
        renderTimer->stop();
        delete renderTimer;
        renderTimer = nullptr;
    }

    if (transparencyCheckTimer) {
        transparencyCheckTimer->stop();
        delete transparencyCheckTimer;
        transparencyCheckTimer = nullptr;
    }

    if (eyeTrackingTimer) {
        eyeTrackingTimer->stop();
        delete eyeTrackingTimer;
        eyeTrackingTimer = nullptr;
    }
}

void GLCore::initMenus()
{
    // 创建主菜单
    _homeMenu = new ElaMenu(this);

    // 添加模型选择子菜单
    _selectModelMenu = _homeMenu->addMenu(ElaIconType::Cubes, "切换模型");

    // 添加设置菜单项
    _menu = _homeMenu->addElaIconAction(ElaIconType::GearComplex, "菜单");
    connect(_menu, &QAction::triggered, [this]() {
        Menu* menu = new Menu(this);
        menu->moveToCenter();
        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->show();
    });

    // 添加关闭程序菜单项
    _close = _homeMenu->addElaIconAction(ElaIconType::X, "关闭");
    connect(_close, &QAction::triggered, [this]() {
        QCoreApplication::exit(0);
    });
}

void GLCore::setupTrayIcon()
{
    // 创建系统托盘图标
    _trayIcon = new QSystemTrayIcon(this);

    // 设置图标
    QIcon icon("Resources/icon_gear.png");
    _trayIcon->setIcon(icon);
    _trayIcon->setToolTip("QT Pet");

    // 设置托盘图标的上下文菜单
    _trayIcon->setContextMenu(_homeMenu);

    // 显示托盘图标
    _trayIcon->show();
}

//---------------------------------------------------------------------
// OpenGL相关函数
//---------------------------------------------------------------------
void GLCore::initializeGL()
{
    LAppDelegate::GetInstance()->Initialize(this);
}

void GLCore::paintGL()
{
    LAppDelegate::GetInstance()->update();
    if (firstGL) {
        firstGL = false;
        generateModelMask();
    }

}

void GLCore::resizeGL(int w, int h)
{
    LAppDelegate::GetInstance()->resize(w, h);
}

//---------------------------------------------------------------------
// 模型管理相关函数
//---------------------------------------------------------------------
void GLCore::scanAndLoadModels()
{
    // 清空现有菜单项
    _selectModelMenu->clear();
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
                ConfigManager::getInstance().setFName(modelName);
                // 模型名称应该是文件名去掉.model3.json
                QString modelNameWithoutExtension = jsonFileName;
                modelNameWithoutExtension.chop(12); // 去掉.model3.json的长度
                ConfigManager::getInstance().setName(modelNameWithoutExtension);
                generateModelMask();
            });
        }
    }
}

/**
 * @brief 生成模型遮罩，和模型绘制并没有关系，目前只是用来保存模型实际大小，用于控制字幕显示宽度。
 * 仅在程序初始化以及模型更换后调用一次。
 */
void GLCore::generateModelMask()
{
    // 获取当前模型
    auto* manager = LAppLive2DManager::GetInstance();
    auto* model = manager->GetModel(0);

    if (!model) {
        qDebug() << "无可用模型，无法生成遮罩";
        return;
    }
    // 在UI线程中一次性获取整个帧缓冲区
    QImage frameBuffer = this->grabFramebuffer();
    // 将帧缓冲区缩放为窗口的逻辑大小
    frameBuffer = frameBuffer.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // 创建与帧缓冲区相同大小的遮罩图像
    QImage maskImage(frameBuffer.width(), frameBuffer.height(), QImage::Format_ARGB32);
    maskImage.fill(Qt::transparent);
    // 使用多线程处理图像
    const int threadCount = QThread::idealThreadCount();
    QVector<QFuture<void>> futures;

    for (int threadIndex = 0; threadIndex < threadCount; threadIndex++) {
        int startY = threadIndex * frameBuffer.height() / threadCount;
        int endY = (threadIndex + 1) * frameBuffer.height() / threadCount;

        futures.append(QtConcurrent::run([&frameBuffer, startY, endY]() {
            for (int y = startY; y < endY; y++) {
                for (int x = 0; x < frameBuffer.width(); x++) {
                    QColor pixelColor = frameBuffer.pixelColor(x, y);
                    if (pixelColor.alpha() > 10) {
                        frameBuffer.setPixelColor(x, y, QColor(255, 0, 0, 64));
                    }
                }
            }
        }));
    }

    // 等待所有线程完成
    for (auto &future : futures)
        future.waitForFinished();

    // 保存遮罩
    image = frameBuffer.copy();

}

void GLCore::updateEyeTracking()
{
    // 获取全局鼠标位置并转换为本地坐标
    QPoint globalPos = QCursor::pos();
    QPoint localPos = mapFromGlobal(globalPos);

    // 更新Live2D模型视线方向
    LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(localPos.x(), localPos.y());
}

void GLCore::setWindowTransparent(bool transparent)
{
    // 使用Windows API设置/取消穿透
    HWND hwnd = (HWND)this->winId();

    if (ConfigManager::getInstance().getAlwaysTransparent()) {
        // 如果始终点击穿透，设置窗口穿透，然后返回
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
        return;
    }
    if (isCurrentlyTransparent == transparent)
        return; // 状态未变，不需要更新

    isCurrentlyTransparent = transparent;


    if (transparent) {
        // 设置WS_EX_TRANSPARENT使窗口对鼠标点击透明
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    } else {
        // 移除WS_EX_TRANSPARENT标志
        SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
    }
}

bool GLCore::isPointInModel(const QPoint& point)
{
    // 检查当前点是否在窗口内
    if (!rect().contains(point)) {
        return false;
    }

    QImage frameBuffer = this->grabFramebuffer();
    // 将帧缓冲区缩放为窗口的逻辑大小
    frameBuffer = frameBuffer.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // 获取当前位置像素
    QColor pixelColor = frameBuffer.pixelColor(point);

    // 透明度大于10，说明当前点在模型上
    return pixelColor.alpha() > 10;
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

    // 根据当前点像素透明度决定是否穿透
    setWindowTransparent(!isPointInModel(localPos));
}

//---------------------------------------------------------------------
// 事件处理函数
//---------------------------------------------------------------------
void GLCore::mouseMoveEvent(QMouseEvent* event)
{
    if (isMiddlePressed) {
        this->move(event->pos() - this->currentPos + this->pos());
    }

    // 允许事件继续传递
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
        else if (event->button() == Qt::MiddleButton) {
            this->isMiddlePressed = true;
            this->currentPos = event->pos();
        }
        else if (event->button() == Qt::RightButton) {
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
        else if (event->button() == Qt::RightButton) {
            isRightPressed = false;
        }
        else if (event->button() == Qt::MiddleButton) {
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

        // 处理缩放
        handleScaling(event->angleDelta().y() > 0);

        event->accept();
    } else {
        event->ignore();
    }
}

void GLCore::handleScaling(bool zoomIn)
{
    // 获取当前窗口信息
    QSize currentSize = size();
    QPoint currentPos = pos();

    // 使用精确的浮点数计算
    double centerX = currentPos.x() + currentSize.width() / 2.0;
    double centerY = currentPos.y() + currentSize.height() / 2.0;

    // 缩放因子
    double scaleFactor = zoomIn ? 1.05 : 0.95;

    // 计算新尺寸
    int newWidth = qMax(100, (int)(currentSize.width() * scaleFactor));
    int newHeight = qMax(100, (int)(currentSize.height() * scaleFactor));

    // 计算新位置
    int newX = qRound(centerX - newWidth / 2.0);
    int newY = qRound(centerY - newHeight / 2.0);

    // 设置窗口的几何形状
    setFixedSize(newWidth, newHeight);
    move(newX, newY);

    // 存储新的窗口位置和大小到配置
    ConfigManager::getInstance().setHeight(newHeight);
    ConfigManager::getInstance().setWidth(newWidth);
    ConfigManager::getInstance().setX(newX);
    ConfigManager::getInstance().setY(newY);

    // 更新模型遮罩
    if (!image.isNull()) {
        image = image.scaled(newWidth, newHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
}