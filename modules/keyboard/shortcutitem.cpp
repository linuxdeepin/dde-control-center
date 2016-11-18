#include "shortcutitem.h"
#include <QLabel>

using namespace dcc;

ShortcutItem::ShortcutItem(QFrame *parent)
    :SettingsItem(parent),
      m_hlayout(new QHBoxLayout())
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);
    m_hlayout->setMargin(0);
    m_hlayout->setSpacing(5);

    m_title = new QLabel();
    m_title->setText("");
    m_title->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);
    layout->addLayout(m_hlayout);
    layout->setAlignment(m_hlayout, Qt::AlignRight);
    setLayout(layout);

    setFixedHeight(36);
}

void ShortcutItem::setShortcutString(QString shortcut)
{
    QLayoutItem *child;
    while ((child = m_hlayout->takeAt(0)) != 0) {
        child->widget()->deleteLater();
        delete child;
    }
    shortcut = shortcut.replace("<", "");
    shortcut = shortcut.replace(">", "-");
    QStringList shortcuts = shortcut.split("-");

    for(int i = 0 ; i < shortcuts.count(); i++)
    {
        QLabel* label = new QLabel();
        label->setStyleSheet("background-color: white; border: 3px; transparent: 0.8;");
        label->setText(shortcuts.at(i));
        label->setAlignment(Qt::AlignCenter);
        m_hlayout->addWidget(label);
        m_hlayout->setAlignment(label, Qt::AlignRight);
    }
}

void ShortcutItem::setTitle(const QString &title)
{
    m_title->setText(title);
}
