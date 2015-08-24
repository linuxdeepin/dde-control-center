#include "useravatar.h"

UserAvatar::UserAvatar(QWidget *parent, bool deleteable) : QPushButton(parent)
{
    setCheckable(true);

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

//    m_iconLabel->setPixmap(QPixmap(iconPath).scaled(m_iconLabel->size()));

    m_iconPath = iconPath;

    repaint();
}

void UserAvatar::setIsNormal(bool normal)
{
    m_isNormal = normal;

    if (normal)
        m_iconLabel->setFixedSize(NORMAL_ICON_SIZE, NORMAL_ICON_SIZE);
    else
        m_iconLabel->setFixedSize(BIG_ICON_SIZE, BIG_ICON_SIZE);

//    m_iconLabel->setPixmap(QPixmap(m_iconPath).scaled(m_iconLabel->size()));
}

QString UserAvatar::iconPath() const
{
    return m_iconPath;
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

void UserAvatar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPainterPath path;
    int iconSize = m_isNormal ? NORMAL_ICON_SIZE : BIG_ICON_SIZE;
    QRect ellipseRec((width() -iconSize) / 2.0, (height() - iconSize) / 2.0, iconSize, iconSize);
    path.addEllipse(ellipseRec);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setClipPath(path);

    painter.drawPixmap(ellipseRec, QPixmap(m_iconPath));

    QColor penColor = m_selected ? m_borderSelectedColor : m_borderColor;

    QPen pen;
    pen.setColor(penColor);
    pen.setWidth(m_borderWidth);
    painter.setPen(pen);
    painter.drawPath(path);
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

void UserAvatar::setSelected(bool selected)
{
    m_selected = selected;

    repaint();
}


int UserAvatar::borderWidth() const
{
    return m_borderWidth;
}

void UserAvatar::setBorderWidth(int borderWidth)
{
    m_borderWidth = borderWidth;
}

QColor UserAvatar::borderSelectedColor() const
{
    return m_borderSelectedColor;
}

void UserAvatar::setBorderSelectedColor(const QColor &borderSelectedColor)
{
    m_borderSelectedColor = borderSelectedColor;
}

QColor UserAvatar::borderColor() const
{
    return m_borderColor;
}

void UserAvatar::setBorderColor(const QColor &borderColor)
{
    m_borderColor = borderColor;
}



AvatarDeleteButton::AvatarDeleteButton(QWidget *parent) : DImageButton(parent)
{

}
