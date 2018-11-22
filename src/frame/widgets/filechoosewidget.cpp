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

#include "filechoosewidget.h"

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
    : SettingsItem(parent),

      m_title(new QLabel),
      m_edit(new QLineEdit),
      m_btn(new DImageButton)
{
    m_title->setFixedWidth(140);
    m_edit->setReadOnly(true);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addWidget(m_edit);
    mainLayout->addWidget(m_btn);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(20, 0, 0, 0);

    setLayout(mainLayout);
    setFixedHeight(36);
    setObjectName("FileChooseWidget");

    connect(m_btn, &DImageButton::clicked, this, &FileChooseWidget::chooseFile);
}

void FileChooseWidget::setTitle(const QString &title)
{
    m_title->setText(title);

    setAccessibleName(title);
}

void FileChooseWidget::chooseFile()
{
    Q_EMIT requestFrameKeepAutoHide(false);

    QFileDialog fd;
    fd.setModal(true);

    if (fd.exec() == QFileDialog::Accepted) {
        m_edit->setText(fd.selectedFiles().first());
    }

    Q_EMIT requestFrameKeepAutoHide(true);
}

}

}

