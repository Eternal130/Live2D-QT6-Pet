#ifndef DANMAKUMANAGER_H
#define DANMAKUMANAGER_H

#include <string>
#include <QTimer>
#include <QPainter>

class DanmakuManager : public QObject {
    Q_OBJECT

public:
    static DanmakuManager &getInstance();

    // 显示弹幕
    void showDanmaku(const std::string &text, int durationMs = 3000);

    // 隐藏弹幕
    void hideDanmaku();

    // 渲染弹幕
    void renderDanmaku(QPainter &painter, int width, int height);

    // 是否正在显示弹幕
    bool isShowing() const { return m_isShowing; }

private:
    DanmakuManager();

    ~DanmakuManager();

    std::string m_text;
    bool m_isShowing{false};
    QTimer *m_timer{nullptr};
    QFont m_font;
    QTimer *m_typingTimer = nullptr; // 打字效果定时器
    int m_currentCharIndex = 0; // 当前显示到的字符位置
    int m_typingSpeed = 50; // 字符显示速度(ms)
    int m_maxLineWidth = 0; // 最大行宽
    int findBreakPosition(const QString &text, const QFontMetrics &metrics, int maxWidth) {
        int pos = 0;
        int lastGoodPos = 0;

        while (pos < text.length()) {
            // 尝试找一个合适的断点
            int nextPos = pos + 1;
            if (metrics.horizontalAdvance(text.left(nextPos)) <= maxWidth) {
                lastGoodPos = pos;
                pos = nextPos;
            } else {
                return lastGoodPos > 0 ? lastGoodPos + 1 : 1; // 至少返回1个字符
            }
        }

        return pos;
    }
};


#endif //DANMAKUMANAGER_H
