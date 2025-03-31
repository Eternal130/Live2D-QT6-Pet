#ifndef DANMAKUMANAGER_H
#define DANMAKUMANAGER_H

#include <string>
#include <QObject>
#include <QTimer>
#include <QPainter>

class DanmakuManager : public QObject
{
    Q_OBJECT
public:
    static DanmakuManager& getInstance();

    // 显示弹幕
    void showDanmaku(const std::string& text, int durationMs = 3000);

    // 隐藏弹幕
    void hideDanmaku();

    // 渲染弹幕
    void renderDanmaku(QPainter& painter, int width, int height);

    // 是否正在显示弹幕
    bool isShowing() const { return m_isShowing; }

private:
    DanmakuManager();
    ~DanmakuManager();

    std::string m_text;
    bool m_isShowing{false};
    QTimer* m_timer{nullptr};
    QFont m_font;
};



#endif //DANMAKUMANAGER_H
