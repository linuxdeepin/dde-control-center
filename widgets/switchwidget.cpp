#include "switchwidget.h"

#include <QHBoxLayout>

using namespace dcc;

DWIDGET_USE_NAMESPACE

SwitchWidget::SwitchWidget(QFrame *parent)
    : SettingsItem(parent)
{

    m_title = new QLabel;
    m_title->setStyleSheet("color: white;"
                           "margin: 0 0 0 8px;");
    m_title->setText("title");

    m_switchBtn = new DSwitchButton;

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_switchBtn);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(5, 0, 5, 0);

    setFixedHeight(36);
    setLayout(mainLayout);

    connect(m_switchBtn, &DSwitchButton::checkedChanged, this, &SwitchWidget::checkedChanegd);
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
