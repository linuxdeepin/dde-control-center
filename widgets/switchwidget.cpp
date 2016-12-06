#include "switchwidget.h"

#include <QHBoxLayout>

using namespace dcc;

DWIDGET_USE_NAMESPACE

SwitchWidget::SwitchWidget(QFrame *parent)
    : SettingsItem(parent)
{
    m_title = new NormalLabel;
    m_title->setStyleSheet("color: white;");
    m_title->setText("title");

    m_switchBtn = new DSwitchButton;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_switchBtn);
    mainLayout->setSpacing(0);

    setFixedHeight(36);
    setLayout(mainLayout);

    connect(m_switchBtn, &DSwitchButton::checkedChanged, this, &SwitchWidget::checkedChanegd);
}

SwitchWidget::SwitchWidget(const QString &title, QFrame *parent) :
    SwitchWidget(parent)
{
    setTitle(title);
}

void SwitchWidget::setChecked(const bool checked)
{
    m_switchBtn->blockSignals(true);
    m_switchBtn->setChecked(checked);
    m_switchBtn->blockSignals(false);
}

void SwitchWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}
