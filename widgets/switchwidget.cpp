#include "switchwidget.h"

#include <QHBoxLayout>

using namespace dcc;

DWIDGET_USE_NAMESPACE

SwitchWidget::SwitchWidget(QFrame *parent)
    : SettingsItem(parent)
{

    m_title = new QLabel;
    m_title->setText("title");

    m_switchBtn = new DSwitchButton;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_switchBtn);

    setLayout(mainLayout);

    connect(m_switchBtn, &DSwitchButton::checkedChanged, this, &SwitchWidget::checkedChanegd);
}

void SwitchWidget::setChecked(const bool checked)
{
    m_switchBtn->setChecked(checked);
}

void SwitchWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}
