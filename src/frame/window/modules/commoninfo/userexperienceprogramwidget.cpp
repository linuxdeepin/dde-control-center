/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "userexperienceprogramwidget.h"
#include "window/modules/commoninfo/commoninfomodel.h"

#include "widgets/switchwidget.h"
#include "widgets/labels/tipslabel.h"

#include <DTipLabel>

#include <QVBoxLayout>
#include <QTimer>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace commoninfo;
DWIDGET_USE_NAMESPACE

UserExperienceProgramWidget::UserExperienceProgramWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vBoxLayout = new QVBoxLayout;

    m_joinUeProgram = new SwitchWidget();
    //~ contents_path /commoninfo/User Experience Program
    m_joinUeProgram->setTitle(tr("Join User Experience Program"));

    DTipLabel *tipLabel = new DTipLabel(tr("Please refer to Article 10~13 in the End User License Agreement for the terms of User Experience Program."));
    tipLabel->setWordWrap(true);
    tipLabel->setContentsMargins(16, 0, 10, 0);

    vBoxLayout->setMargin(0);
    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(m_joinUeProgram);
    vBoxLayout->addSpacing(8);
    vBoxLayout->addWidget(tipLabel);
    vBoxLayout->addStretch();

    setLayout(vBoxLayout);

    connect(m_joinUeProgram, &SwitchWidget::checkedChanged, this, [this](bool state) {
        QTimer::singleShot(0, this, [ = ] {
            this->enableUeProgram(state);
        });
    });
}

void UserExperienceProgramWidget::setModel(CommonInfoModel *model)
{
    connect(model, &CommonInfoModel::ueProgramChanged, m_joinUeProgram, &SwitchWidget::setChecked);
}

void UserExperienceProgramWidget::setDefaultUeProgram(const bool enabled)
{
    m_joinUeProgram->setChecked(enabled);
}
