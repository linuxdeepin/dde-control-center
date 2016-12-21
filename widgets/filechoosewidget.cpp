#include "filechoosewidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>

using namespace dcc::widgets;

DWIDGET_USE_NAMESPACE

FileChooseWidget::FileChooseWidget(QWidget *parent)
    : SettingsItem(parent),

      m_title(new QLabel),
      m_edit(new QLineEdit),
      m_btn(new DImageButton)
{
    m_title->setFixedWidth(140);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_edit);
    mainLayout->addWidget(m_btn);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
    setFixedHeight(36);

    connect(m_btn, &DImageButton::clicked, this, &FileChooseWidget::chooseFile);
}

void FileChooseWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

void FileChooseWidget::chooseFile()
{
    QFileDialog fd;
    fd.setModal(true);

    if (fd.exec() == QFileDialog::Accepted)
        m_edit->setText(fd.selectedFiles().first());
}
