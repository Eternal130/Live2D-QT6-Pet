#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "ElaWidget.h"

class ElaPushButton;
class ElaText;

class MessageBox : public ElaWidget {
    Q_OBJECT

public:
    explicit MessageBox(const QString &message = nullptr, QWidget *parent = nullptr);

    ~MessageBox();

    // 设置按钮文本
    void setConfirmButtonText(const QString &text);

    void setMessageText(const QString &text);

    [[nodiscard]] ElaPushButton *m_confirm_button() const {
        return m_confirmButton;
    }

private:
    ElaText *m_messageText;
    ElaPushButton *m_confirmButton;
};

#endif //MESSAGEBOX_H
