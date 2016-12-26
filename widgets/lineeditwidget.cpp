#include "lineeditwidget.h"

#include <QVBoxLayout>

namespace dcc {
namespace widgets {

LineEditWidget::LineEditWidget(QFrame *parent)
    : SettingsItem(parent),

      m_title(new QLabel),
      m_edit(new QLineEdit)
{
    m_title->setFixedWidth(140);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_edit);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
    setObjectName("LineEditWidget");
    setFixedHeight(36);
}

void LineEditWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

void LineEditWidget::setText(const QString &text)
{
    m_edit->setText(text);
}

}
}
