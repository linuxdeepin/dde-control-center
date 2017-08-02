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
    m_spinBox->setStyleSheet("DSpinBox {"
                             "background-color: transparent;"
                             "}"
                             ""
                             "QWidget {"
                             "border: none;"
                             "background-color: transparent;"
                             "}"
                             ""
                             "QAbstractSpinBox::up-button {"
                             "border: none;"
                             "background: transparent;"
                             "}"
                             ""
                             "QAbstractSpinBox::up-arrow {"
                             "border: none;"
                             "background: transparent;"
                             "}"
                             ""
                             "QAbstractSpinBox::down-button {"
                             "border: none;"
                             "background: transparent;"
                             "}"
                             ""
                             "QAbstractSpinBox::down-arrow {"
                             "border: none;"
                             "background: transparent;"
                             "}");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(20, 7, 10, 7);
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
