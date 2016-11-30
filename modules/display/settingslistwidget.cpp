#include "settingslistwidget.h"

#include <QVBoxLayout>

SettingsListWidget::SettingsListWidget(QWidget *parent)
    : QWidget(parent),

      m_title(new QLabel),
      m_listLayout(new QVBoxLayout)
{
    m_title->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    m_listLayout->setSpacing(0);
    m_listLayout->setMargin(0);

    QWidget *listWidget = new QWidget;
    listWidget->setObjectName("SettingsListWidget");
    listWidget->setStyleSheet("QWidget #SettingsListWidget {"
                              "border: 1px solid red;"
                              "}");
    listWidget->setLayout(m_listLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(listWidget);

    setLayout(mainLayout);
}

void SettingsListWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SettingsListWidget::appendOption(const QString &option)
{
    SettingsOptionItem *item = new SettingsOptionItem;
    item->setTitle(option);

    m_listLayout->addWidget(item);
}

void SettingsListWidget::setSelectedIndex(const int selected)
{
    for (int i(0); i != m_listLayout->count(); ++i)
    {
        SettingsOptionItem *item = qobject_cast<SettingsOptionItem *>(m_listLayout->itemAt(i)->widget());
        item->setSelected(selected == i);
    }
}
