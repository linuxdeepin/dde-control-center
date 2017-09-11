#include "shortcutkey.h"
#include "keylabel.h"

#include <QMouseEvent>

ShortcutKey::ShortcutKey(QWidget *parent) : QWidget(parent)
{

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(5);

    setLayout(m_mainLayout);
    setStyleSheet("background: transparent;");
}

void ShortcutKey::setTextList(const QStringList &list)
{
    for (KeyLabel *label : m_list) {
        m_mainLayout->removeWidget(label);
        label->deleteLater();
    }

    m_list.clear();

    for (const QString &key : list) {
        KeyLabel *label = new KeyLabel(key);
        m_list << label;
        m_mainLayout->addWidget(label);
    }
}

void ShortcutKey::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);

    for (KeyLabel *label : m_list)
        label->setEnter(true);
}

void ShortcutKey::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    for (KeyLabel *label : m_list)
        label->setEnter(false);
}
