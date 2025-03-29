#include "ConfirmDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaImageCard.h"
#include "ElaText.h"
#include "ElaPushButton.h"

ConfirmDialog::ConfirmDialog(const QString& title, const QString& message, QWidget* parent)
    : ElaWidget(parent)
{
    // 设置窗口属性
    setWindowTitle(title);
    setIsFixedSize(true);
    setFixedSize(400, 200);
    setWindowModality(Qt::ApplicationModal);
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);

    // 创建消息文本
    m_messageText = new ElaText(message, this);
    m_messageText->setWordWrap(true);
    m_messageText->setTextPixelSize(14);

    // 创建按钮
    m_confirmButton = new ElaPushButton("删除", this);
    m_cancelButton = new ElaPushButton("取消", this);


    // 布局
    QHBoxLayout* iconTextLayout = new QHBoxLayout();
    iconTextLayout->addSpacing(15);
    iconTextLayout->addWidget(m_messageText, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_confirmButton);
    buttonLayout->addWidget(m_cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 30, 20, 20);
    mainLayout->setSpacing(20);
    mainLayout->addLayout(iconTextLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

ConfirmDialog::~ConfirmDialog()
{
}

void ConfirmDialog::setConfirmButtonText(const QString& text)
{
    m_confirmButton->setText(text);
}

void ConfirmDialog::setCancelButtonText(const QString& text)
{
    m_cancelButton->setText(text);
}
void ConfirmDialog::setMessageText(const QString& text)
{
    m_messageText->setText(text);
}