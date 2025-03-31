#include "DanmakuManager.h"
#include <QFontMetrics>

DanmakuManager& DanmakuManager::getInstance()
{
    static DanmakuManager instance;
    return instance;
}

DanmakuManager::DanmakuManager()
{
    m_timer = new QTimer(this);
    m_font = QFont("Microsoft YaHei", 18);

    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_isShowing = false;
        m_text = "";
        m_timer->stop();
    });
}

DanmakuManager::~DanmakuManager()
{
    delete m_timer;
}

void DanmakuManager::showDanmaku(const std::string& text, int durationMs)
{
    m_text = text;
    m_isShowing = true;

    if (m_timer->isActive()) {
        m_timer->stop();
    }

    m_timer->start(durationMs);
}

void DanmakuManager::hideDanmaku()
{
    m_isShowing = false;
    m_text = "";

    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

void DanmakuManager::renderDanmaku(QPainter& painter, int width, int height)
{
    if (!m_isShowing || m_text.empty()) {
        return;
    }

    painter.setFont(m_font);

    QString qText = QString::fromStdString(m_text);
    QFontMetrics metrics(m_font);
    QRect textRect = metrics.boundingRect(qText);

    // 创建背景矩形
    QRect bgRect = textRect.adjusted(-15, -10, 15, 10);
    bgRect.moveCenter(QPoint(width / 2, height / 2));

    // 绘制半透明背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 128));
    painter.drawRoundedRect(bgRect, 8, 8);

    // 绘制文本
    painter.setPen(Qt::white);
    painter.drawText(bgRect, Qt::AlignCenter, qText);
}