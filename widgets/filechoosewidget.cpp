#include "filechoosewidget.h"

#include <QHBoxLayout>
#include <QLabel>

using namespace dcc::widgets;

DWIDGET_USE_NAMESPACE

FileChooseWidget::FileChooseWidget(QWidget *parent)
    : SettingsItem(parent),

      m_title(new QLabel),
      m_edit(new DFileChooserEdit)
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_edit);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
}

void FileChooseWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}
