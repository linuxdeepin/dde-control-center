#include "switchwidget.h"

#include <QHBoxLayout>

#include "labels/normallabel.h"

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

SwitchWidget::SwitchWidget(QFrame *parent)
    : SwitchWidget(new NormalLabel, parent)
{

}

SwitchWidget::SwitchWidget(const QString &title, QFrame *parent) :
    SwitchWidget(new NormalLabel(title), parent)
{

}

SwitchWidget::SwitchWidget(QWidget *widget, QFrame *parent):
    SettingsItem(parent),
    m_leftWidget(widget),
    m_switchBtn(new DSwitchButton)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(20, 0, 10, 0);

    mainLayout->addWidget(m_leftWidget, 0, Qt::AlignVCenter);
    mainLayout->addStretch();
    mainLayout->addWidget(m_switchBtn, 0, Qt::AlignVCenter);

    setFixedHeight(36);
    setLayout(mainLayout);

    connect(m_switchBtn, &DSwitchButton::checkedChanged, this, &SwitchWidget::checkedChanged);
}

void SwitchWidget::setChecked(const bool checked)
{
    m_switchBtn->blockSignals(true);
    m_switchBtn->setChecked(checked);
    m_switchBtn->blockSignals(false);
}

void SwitchWidget::setTitle(const QString &title)
{
    QLabel *label = qobject_cast<QLabel*>(m_leftWidget);
    if (label) {
        label->setText(title);
    }

    setAccessibleName(title);
}

bool SwitchWidget::checked() const
{
    return m_switchBtn->checked();
}

}
}
