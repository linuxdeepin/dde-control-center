#include "useravatar.h"

#include "dthememanager.h"

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

    D_THEME_INIT_WIDGET(UserAvatar);
}

void UserAvatar::setIcon(const QString &iconPath, const QSize &size)
{
    if (size.isEmpty())
        m_iconLabel->setFixedSize(NORMAL_ICON_SIZE, NORMAL_ICON_SIZE);
    else
        m_iconLabel->setFixedSize(size);

    m_iconPath = iconPath;

    repaint();
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

void UserAvatar::paintEvent(QPaintEvent *)
{
    int iconSize = NORMAL_ICON_SIZE;
    switch (m_avatarSize){
    case AvatarSmallSize:
        iconSize = SMALL_ICON_SIZE;
        break;
    case AvatarLargeSize:
        iconSize = LARGE_ICON_SIZE;
        break;
    default:
        break;
    }

    QPainter painter(this);
    QRect ellipseRec((width() -iconSize) / 2.0, (height() - iconSize) / 2.0, iconSize, iconSize);
    QPainterPath path;
    path.addEllipse(ellipseRec);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setClipPath(path);

    QImage tmpImg(m_iconPath);
    painter.drawImage(ellipseRec, this->isEnabled() ? tmpImg : imageToGray(tmpImg));

    QColor penColor = m_selected ? m_borderSelectedColor : m_borderColor;

    QPen pen;
    pen.setColor(penColor);
    pen.setWidth(m_borderWidth);
    painter.setPen(pen);
    painter.drawPath(path);
    painter.end();
}

QImage UserAvatar::imageToGray(const QImage &image)
{
    int height = image.height();
    int width = image.width();
    QImage targetImg(width, height, QImage::Format_Indexed8);
    targetImg.setColorCount(256);
    for(int i = 0; i < 256; i++)
        targetImg.setColor(i, qRgb(i, i, i));

    switch(image.format())
    {
    case QImage::Format_Indexed8:
        for(int i = 0; i < height; i ++)
        {
            const uchar *pSrc = (uchar *)image.constScanLine(i);
            uchar *pDest = (uchar *)targetImg.scanLine(i);
            memcpy(pDest, pSrc, width);
        }
        break;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        for(int i = 0; i < height; i ++)
        {
            const QRgb *pSrc = (QRgb *)image.constScanLine(i);
            uchar *pDest = (uchar *)targetImg.scanLine(i);

            for( int j = 0; j < width; j ++)
            {
                 pDest[j] = qGray(pSrc[j]);
            }
        }
        break;
    default:
        break;
    }
    return targetImg;
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

void UserAvatar::setAvatarSize(const AvatarSize &avatarSize)
{
    int tmpSize = NORMAL_ICON_SIZE;
    switch (avatarSize){
    case AvatarSmallSize:
        tmpSize = SMALL_ICON_SIZE;
        break;
    case AvatarLargeSize:
        tmpSize = LARGE_ICON_SIZE;
        break;
    default:
        break;
    }
    m_iconLabel->setFixedSize(tmpSize, tmpSize);

    m_avatarSize = avatarSize;
}

void UserAvatar::setDisabled(bool disable)
{
    setEnabled(!disable);
    repaint();
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
