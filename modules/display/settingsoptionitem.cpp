#include "settingsoptionitem.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QPen>
#include <QPainterPath>

namespace dcc {

namespace display {

SettingsOptionItem::SettingsOptionItem(QWidget *parent)
    : QFrame(parent),

      m_title(new QLabel),
      m_icon(new QLabel)
{
    m_icon->setPixmap(QPixmap(":/display/themes/common/icon/list_select.png"));
    m_icon->setVisible(false);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_icon);
    mainLayout->setSpacing(0);

    mainLayout->setContentsMargins(15, 0, 10, 0);

    setFixedHeight(36);

    setLayout(mainLayout);
}

void SettingsOptionItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SettingsOptionItem::setSelected(const bool checked)
{
    m_icon->setVisible(checked);
}

void SettingsOptionItem::mouseReleaseEvent(QMouseEvent *e)
{
    emit clicked();

    QFrame::mouseReleaseEvent(e);
}

} // namespace display

} // namespace dcc

