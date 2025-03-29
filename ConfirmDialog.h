#ifndef ELACONFIRMDIALOG_H
#define ELACONFIRMDIALOG_H

#include "ElaWidget.h"

class ElaPushButton;
class ElaText;

class ConfirmDialog : public ElaWidget
{
    Q_OBJECT

public:
    explicit ConfirmDialog(const QString& title = nullptr, const QString& message = nullptr, QWidget* parent = nullptr);
    ~ConfirmDialog();

    // 设置按钮文本
    void setConfirmButtonText(const QString& text);
    void setCancelButtonText(const QString& text);

    void setMessageText(const QString &text);

private:
    ElaText* m_messageText;
    ElaPushButton* m_confirmButton;

public:
    [[nodiscard]] ElaPushButton * m_confirm_button() const {
        return m_confirmButton;
    }

    [[nodiscard]] ElaPushButton * m_cancel_button() const {
        return m_cancelButton;
    }

private:
    ElaPushButton* m_cancelButton;
};

#endif // ELACONFIRMDIALOG_H