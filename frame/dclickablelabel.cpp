#include "dclickablelabel.h"
#include "constants.h"

DClickablePictureLabel::DClickablePictureLabel(QWidget *parent, const QString &normalPic, const QString &hoverPic, const QString &pressPic)
    : QLabel(parent)
{
    if (!normalPic.isEmpty())
        m_normalPic = QPixmap(DCC::IconPath + normalPic);
    if (!hoverPic.isEmpty())
        m_hoverPic = QPixmap(DCC::IconPath + hoverPic);
    if (!pressPic.isEmpty())
        m_pressPic = QPixmap(DCC::IconPath + pressPic);

    changeState();
}

DClickablePictureLabel::~DClickablePictureLabel()
{
}

void DClickablePictureLabel::enterEvent(QEvent *event)
{
    m_state = Hover;
    changeState();

    QLabel::enterEvent(event);
}

void DClickablePictureLabel::leaveEvent(QEvent *event)
{
    m_state = Normal;
    changeState();

    QLabel::leaveEvent(event);
}

void DClickablePictureLabel::mousePressEvent(QMouseEvent *event)
{
    m_state = Press;
    changeState();

    QLabel::mousePressEvent(event);
}

void DClickablePictureLabel::mouseReleaseEvent(QMouseEvent *event)
{
    m_state = Hover;
    changeState();

    emit clicked();

    QLabel::mouseReleaseEvent(event);
}

void DClickablePictureLabel::changeState()
{
    switch (m_state) {
    case Hover:     if (!m_hoverPic.isNull()) setPixmap(m_hoverPic);      break;
    case Press:     if (!m_pressPic.isNull()) setPixmap(m_pressPic);      break;
    default:        if (!m_normalPic.isNull()) setPixmap(m_normalPic);    break;
    }

    emit stateChanged();
}

