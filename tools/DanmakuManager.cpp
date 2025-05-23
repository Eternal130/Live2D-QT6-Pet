#include "DanmakuManager.h"
#include <QFontMetrics>
#include <QTextLayout>

DanmakuManager &DanmakuManager::getInstance() {
    static DanmakuManager instance;
    return instance;
}

DanmakuManager::DanmakuManager() {
    m_timer = new QTimer(this);
    m_typingTimer = new QTimer(this);
    m_font = QFont("Microsoft YaHei", 18);

    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_isShowing = false;
        m_text = "";
        m_currentCharIndex = 0;
        m_timer->stop();
    });

    // 打字效果定时器
    connect(m_typingTimer, &QTimer::timeout, this, [this]() {
        if (m_currentCharIndex < m_text.length()) {
            m_currentCharIndex++;
        } else {
            m_typingTimer->stop();
        }
    });
}

DanmakuManager::~DanmakuManager() {
    delete m_timer;
    delete m_typingTimer;
}

void DanmakuManager::showDanmaku(const std::string &text, int durationMs) {
    m_text = text;
    m_isShowing = true;
    m_currentCharIndex = 0;

    if (m_timer->isActive()) {
        m_timer->stop();
    }
    if (m_typingTimer->isActive()) {
        m_typingTimer->stop();
    }

    // 计算动态打字速度
    // 预留约40%的时间给用户阅读完整文本
    int textLength = QString::fromStdString(text).length();
    if (textLength > 0) {
        // 整个打字过程占总显示时间的60%
        int typingTotalTime = static_cast<int>(durationMs * 0.6);
        // 计算每字符的打字速度
        m_typingSpeed = std::max(20, typingTotalTime / textLength);
    } else {
        m_typingSpeed = 50; // 默认值
    }

    // 启动打字效果
    m_typingTimer->start(m_typingSpeed);

    // 设置总显示时间
    m_timer->start(durationMs);
}

void DanmakuManager::hideDanmaku() {
    m_isShowing = false;
    m_text = "";
    m_currentCharIndex = 0;

    if (m_timer->isActive()) {
        m_timer->stop();
    }
    if (m_typingTimer->isActive()) {
        m_typingTimer->stop();
    }
}

void DanmakuManager::renderDanmaku(QPainter &painter, int width, int height) {
    if (!m_isShowing || m_text.empty() || m_currentCharIndex == 0) {
        return;
    }

    painter.setFont(m_font);
    QFontMetrics metrics(m_font);

    // 获取当前应显示的文本（按字节计数而非字符）
    int bytePos = 0;
    int charCount = 0;
    QString fullText = QString::fromStdString(m_text);
    QString qText;

    // 按Unicode字符正确截取
    for (int i = 0; i < fullText.length() && charCount < m_currentCharIndex; i++) {
        qText.append(fullText[i]);
        charCount++;
    }

    // 使用Qt的文本布局功能自动换行
    QTextLayout textLayout(qText, m_font);
    m_maxLineWidth = width * 0.3;

    textLayout.beginLayout();
    QTextLine line = textLayout.createLine();

    QStringList lines;
    while (line.isValid()) {
        line.setLineWidth(m_maxLineWidth);
        lines.append(qText.mid(line.textStart(), line.textLength()));
        line = textLayout.createLine();
    }
    textLayout.endLayout();

    // 如果QTextLayout没有正确分行，使用备用方案
    if (lines.isEmpty()) {
        // 备用方案：手动分行
        QString remainingText = qText;
        while (!remainingText.isEmpty()) {
            int breakPos = metrics.horizontalAdvance(remainingText) > m_maxLineWidth
                               ? findBreakPosition(remainingText, metrics, m_maxLineWidth)
                               : remainingText.length();

            lines.append(remainingText.left(breakPos));
            remainingText.remove(0, breakPos);
        }
    }

    // 计算文本整体高度
    int textHeight = lines.size() * metrics.height();

    // 创建背景矩形
    QRect bgRect(0, 0, m_maxLineWidth + 30, textHeight + 20);
    bgRect.moveCenter(QPoint(width / 2, height / 1.5));

    // 绘制半透明背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 128));
    painter.drawRoundedRect(bgRect, 8, 8);

    // 绘制文本行
    painter.setPen(Qt::white);
    QPoint textPos(bgRect.left() + 15, bgRect.top() + 15 + metrics.ascent());

    for (const QString &line: lines) {
        painter.drawText(textPos, line);
        textPos.setY(textPos.y() + metrics.height());
    }
}
