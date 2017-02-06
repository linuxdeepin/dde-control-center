#include "quickswitchbutton.h"

#include <QDebug>

namespace dcc {

#define SIZE 30

QuickSwitchButton::QuickSwitchButton(const int index, const QString &iconName, QWidget *parent)
    : QLabel(parent),

      m_index(index),
      m_checked(false),
      m_iconName(iconName)
{
    setFixedSize(SIZE, SIZE);
    setPixmap(normalPixmap());
}

void QuickSwitchButton::mouseReleaseEvent(QMouseEvent *e)
{
    QLabel::mouseReleaseEvent(e);

    emit clicked(m_index);
}

void QuickSwitchButton::enterEvent(QEvent *e)
{
    QLabel::enterEvent(e);

    emit hovered(m_index);
}

QPixmap QuickSwitchButton::normalPixmap() const
{
    QString checkedStr = m_checked ? "_on" : "_off";
    return QString(":/frame/themes/%1/icons/%2%3.png").arg(m_themeName).arg(m_iconName).arg(checkedStr);
}

QPixmap QuickSwitchButton::hoverPixmap() const
{
    QString checkedStr = m_checked ? "_on" : "_off";
    return QString(":/frame/themes/%1/icons/%2%3.png").arg(m_themeName).arg(m_iconName).arg(checkedStr);
}

QPixmap QuickSwitchButton::pressedPixmap() const
{
    QString checkedStr = m_checked ? "_on" : "_off";
    return QString(":/frame/themes/%1/icons/%2%3.png").arg(m_themeName).arg(m_iconName).arg(checkedStr);
}

QString QuickSwitchButton::themeName() const
{
    return m_themeName;
}

void QuickSwitchButton::setThemeName(const QString &themeName)
{
    m_themeName = themeName;
    setPixmap(normalPixmap());
}

void QuickSwitchButton::setChecked(const bool checked)
{
    m_checked = checked;
}

}
