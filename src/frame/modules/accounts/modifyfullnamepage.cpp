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

#include "modifyfullnamepage.h"
#include "user.h"
#include "widgets/settingsgroup.h"
#include "widgets/lineeditwidget.h"

#include <QPushButton>

using namespace dcc::widgets;

namespace dcc {
namespace accounts {

ModifyFullNamePage::ModifyFullNamePage(User *u, QWidget *parent)
    : ContentWidget(parent),

      m_user(u)
{
    m_fullnameWidget = new LineEditWidget;
    m_fullnameWidget->setTitle(tr("Fullname:"));
    m_fullnameWidget->textEdit()->setText(m_user->fullname());

    QPushButton *confirmBtn = new QPushButton;
    confirmBtn->setText(tr("OK"));

    SettingsGroup *grp = new SettingsGroup;
    grp->appendItem(m_fullnameWidget);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(grp);
    centralLayout->addWidget(confirmBtn);
    centralLayout->setSpacing(10);
    centralLayout->setContentsMargins(0, 10, 0, 0);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Fullname Settings"));

    connect(m_user, &User::fullnameChanged, this, &ModifyFullNamePage::onFullnameChanged);
    connect(confirmBtn, &QPushButton::clicked, this, [=] { Q_EMIT requestSetFullname(m_user, m_fullnameWidget->textEdit()->text()); });
}

void ModifyFullNamePage::onFullnameChanged(const QString &fn)
{
    m_fullnameWidget->textEdit()->setText(fn);
}

}

}
