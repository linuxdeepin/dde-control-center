#include "switchwidget.h"

#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

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

SwitchWidget::SwitchWidget(QWidget *widget, QFrame *parent):
    SettingsItem(parent)
{
    m_switchBtn = new DSwitchButton;

    QHBoxLayout *mainLayout = new QHBoxLayout;

    mainLayout->addWidget(widget, 0, Qt::AlignLeft);
    mainLayout->addStretch();
    mainLayout->addWidget(m_switchBtn);
    mainLayout->setSpacing(0);

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

    setAccessibleName(title);
}

}
}
