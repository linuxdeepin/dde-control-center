#include "usericon.h"

UserIcon::UserIcon(QWidget *parent) : QLabel(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignCenter);

    m_iconLabel = new QLabel();
    m_iconLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(m_iconLabel);
    setLayout(mainLayout);
}

void UserIcon::setIcon(const QString &iconPath, const QSize &size)
{
    if (size.isEmpty())
        m_iconLabel->setFixedSize(NORMAL_ICON_SIZE, NORMAL_ICON_SIZE);
    else
        m_iconLabel->setFixedSize(size);

    m_iconLabel->setPixmap(QPixmap(iconPath).scaled(m_iconLabel->size()));

    m_iconPath = iconPath;
}

void UserIcon::setIsNormal(bool normal)
{
    if (normal)
        m_iconLabel->setFixedSize(NORMAL_ICON_SIZE, NORMAL_ICON_SIZE);
    else
        m_iconLabel->setFixedSize(BIG_ICON_SIZE, BIG_ICON_SIZE);

    m_iconLabel->setPixmap(QPixmap(m_iconPath).scaled(m_iconLabel->size()));
}
QString UserIcon::iconPath() const
{
    return m_iconPath;
}

void UserIcon::mousePressEvent(QMouseEvent *e)
{
    emit mousePress();
    QLabel::mousePressEvent(e);
}


