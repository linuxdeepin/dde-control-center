#include "filechoosewidget.h"

#include <dfilechooseredit.h>

#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

FileChooseWidget::FileChooseWidget(QWidget *parent)
    : SettingsItem(parent)
    , m_fileChooserEdit(new DFileChooserEdit(this))
    , m_title(new QLabel)
{
    m_title->setFixedWidth(110);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    m_fileChooserEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_fileChooserEdit->setFileMode(QFileDialog::Directory);
    mainLayout->addWidget(m_fileChooserEdit);
    setLayout(mainLayout);
}

void FileChooseWidget::setTitle(const QString &title)
{
    m_title->setText(title);
    setAccessibleName(title);
    m_title->setWordWrap(true);
}

void FileChooseWidget::setIsErr(const bool err)
{
    m_fileChooserEdit->setAlert(err);
}
