#include "useravatar.h"

UserAvatar::UserAvatar(QWidget *parent, bool deleteable) : QLabel(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignCenter);

    m_iconLabel = new QLabel();
    m_iconLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(m_iconLabel);
    setLayout(mainLayout);

    if (deleteable)
        initDeleteButton();
}

void UserAvatar::setIcon(const QString &iconPath, const QSize &size)
{
    if (size.isEmpty())
        m_iconLabel->setFixedSize(NORMAL_ICON_SIZE, NORMAL_ICON_SIZE);
    else
        m_iconLabel->setFixedSize(size);

    m_iconLabel->setPixmap(QPixmap(iconPath).scaled(m_iconLabel->size()));

    m_iconPath = iconPath;
}

void UserAvatar::setIsNormal(bool normal)
{
    if (normal)
        m_iconLabel->setFixedSize(NORMAL_ICON_SIZE, NORMAL_ICON_SIZE);
    else
        m_iconLabel->setFixedSize(BIG_ICON_SIZE, BIG_ICON_SIZE);

    m_iconLabel->setPixmap(QPixmap(m_iconPath).scaled(m_iconLabel->size()));
}

QString UserAvatar::iconPath() const
{
    return m_iconPath;
}

void UserAvatar::mousePressEvent(QMouseEvent *e)
{
    emit mousePress();
    QLabel::mousePressEvent(e);
}

void UserAvatar::enterEvent(QEvent *)
{
    if (m_deleteButton)
        m_deleteButton->show();
}

void UserAvatar::leaveEvent(QEvent *)
{
    if (m_deleteButton)
        m_deleteButton->hide();
}

void UserAvatar::initDeleteButton()
{
    m_deleteButton = new AvatarDeleteButton(this);
    m_deleteButton->hide();
    m_deleteButton->raise();
    m_deleteButton->setFixedSize(24, 24); //image's size
    m_deleteButton->move(width()  - m_deleteButton->width(), 0);
    connect(m_deleteButton, &AvatarDeleteButton::clicked, this, &UserAvatar::requestDelete);
}


AvatarDeleteButton::AvatarDeleteButton(QWidget *parent) : DImageButton(parent)
{

}
