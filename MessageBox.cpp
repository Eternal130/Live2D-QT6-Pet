#include "MessageBox.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ElaImageCard.h"
#include "ElaText.h"
#include "ElaPushButton.h"

MessageBox::MessageBox(const QString& message, QWidget* parent)
    : ElaWidget(parent)
{
    setIsFixedSize(true);
    setFixedSize(300, 150);
    setWindowModality(Qt::ApplicationModal);
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);

    // 创建消息文本
    m_messageText = new ElaText(message, this);
    m_messageText->setWordWrap(true);
    m_messageText->setTextPixelSize(14);

    // 创建按钮
    m_confirmButton = new ElaPushButton("确认", this);
    connect(m_confirmButton, &ElaPushButton::clicked, this, [=]() {
        this->hide();
    });


    // 布局
    QHBoxLayout* iconTextLayout = new QHBoxLayout();
    iconTextLayout->addSpacing(15);
    iconTextLayout->addWidget(m_messageText, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_confirmButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 30, 20, 20);
    mainLayout->setSpacing(20);
    mainLayout->addLayout(iconTextLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

MessageBox::~MessageBox()
{
}

void MessageBox::setConfirmButtonText(const QString& text)
{
    m_confirmButton->setText(text);
}

void MessageBox::setMessageText(const QString& text)
{
    m_messageText->setText(text);
}