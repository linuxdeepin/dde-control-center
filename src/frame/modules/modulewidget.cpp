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

#include "modules/modulewidget.h"
#include "modulewidgetheader.h"

#include <QEvent>
#include <QDebug>

using namespace dcc::widgets;

namespace dcc {

static const char *ObjectNameTitle = "ModuleHeaderTitle";
static const char *ObjectNameTemplateIcon = "ModuleHeaderIcon%1";

ModuleWidget::ModuleWidget()
    : QWidget(nullptr)
{
    m_moduleIcon = new QLabel;
    m_moduleIcon->setFixedSize(24, 24);

    m_moduleTitle = new LargeLabel;
    m_moduleTitle->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_moduleTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_moduleTitle->setObjectName(ObjectNameTitle);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(11, 0, 0, 0);
    titleLayout->addWidget(m_moduleIcon);
    titleLayout->setAlignment(m_moduleIcon, Qt::AlignCenter);
    titleLayout->addWidget(m_moduleTitle);

    ModuleWidgetHeader *headerWidget = new ModuleWidgetHeader;
    headerWidget->setLayout(titleLayout);

    m_centralLayout = new QVBoxLayout;
    m_centralLayout->addWidget(headerWidget);
    m_centralLayout->setSpacing(10);
    m_centralLayout->setMargin(0);

    setLayout(m_centralLayout);

    connect(this, &ModuleWidget::objectNameChanged, [this] {
        m_moduleIcon->setObjectName(QString(ObjectNameTemplateIcon).arg(objectName()));
    });
}

const QString ModuleWidget::title() const
{
    return m_moduleTitle->text();
}

void ModuleWidget::setTitle(const QString &title)
{
    m_moduleTitle->setText(title);

    setAccessibleName(title);
}

bool ModuleWidget::event(QEvent *event)
{
    if (event->type() == QEvent::LayoutRequest)
        setFixedHeight(m_centralLayout->sizeHint().height());

    return QWidget::event(event);
}

}
