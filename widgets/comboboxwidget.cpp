#include "comboboxwidget.h"

#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>

using namespace dcc::widgets;

ComboBoxWidget::ComboBoxWidget(QWidget *parent)
    : SettingsItem(parent),

      m_title(new QLabel),
      m_comboBox(new QComboBox)
{
    m_title->setFixedWidth(140);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_comboBox);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
    setFixedHeight(36);
}

void ComboBoxWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}
