// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/filechoosewidget.h"
#include "window/utils.h"

#include <dfilechooseredit.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace widgets {

FileChooseWidget::FileChooseWidget(QWidget *parent)
    : SettingsItem(parent)
    , m_title(new QLabel)
{
    m_fileChooserEdit = new DFileChooserEdit(this);
    m_title->setMinimumWidth(LeftTitleWitdh);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title, 3, Qt::AlignVCenter);
    m_fileChooserEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mainLayout->addWidget(m_fileChooserEdit, 7, Qt::AlignVCenter);
    setLayout(mainLayout);
    setObjectName("FileChooseWidget");
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
}
}

