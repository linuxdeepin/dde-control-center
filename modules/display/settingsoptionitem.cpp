#include "settingsoptionitem.h"

#include <QHBoxLayout>

namespace dcc {

namespace display {

SettingsOptionItem::SettingsOptionItem(QWidget *parent)
    : QWidget(parent),

      m_title(new QLabel),
      m_icon(new QLabel)
{
    m_icon->setPixmap(QPixmap(":/display/themes/common/icon/select.png"));
    m_icon->setVisible(false);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_icon);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(30, 2, 15, 2);

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

void SettingsOptionItem::mouseReleaseEvent(QMouseEvent *)
{
    emit clicked();
}

} // namespace display

} // namespace dcc

