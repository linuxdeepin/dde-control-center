#include "spinboxwidget.h"

#include <dspinbox.h>

#include <QLabel>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace widgets {

SpinBoxWidget::SpinBoxWidget(QWidget *parent)
    : SettingsItem(parent),
      m_title(new QLabel),
      m_spinBox(new DSpinBox)
{
    m_spinBox->setContextMenuPolicy(Qt::NoContextMenu);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(20, 0, 10, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_spinBox);

    setLayout(mainLayout);
}

void SpinBoxWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

}

}
