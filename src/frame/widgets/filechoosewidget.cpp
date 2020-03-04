/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "widgets/filechoosewidget.h"

#include <dfilechooseredit.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>

using namespace dcc::widgets;

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace widgets {

FileChooseWidget::FileChooseWidget(QWidget *parent)
    : SettingsItem(parent)
    , m_title(new QLabel)
{
    m_fileChooserEdit = new DFileChooserEdit(this);
    m_title->setFixedWidth(110);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    m_fileChooserEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mainLayout->addWidget(m_fileChooserEdit);
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

