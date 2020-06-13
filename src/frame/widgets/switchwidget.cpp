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

#include "widgets/switchwidget.h"

#include <QHBoxLayout>

#include <QDebug>
#include <QMouseEvent>

#include "widgets/labels/normallabel.h"

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

SwitchWidget::SwitchWidget(const QString &title, QWidget *parent)
    : SwitchWidget(parent, new NormalLabel(title))
{
    m_switchBtn->setAccessibleName(title);
}

SwitchWidget::SwitchWidget(QWidget *parent, QWidget *widget)
    : SettingsItem(parent)
    , m_leftWidget(widget)
    , m_switchBtn(new DSwitchButton)
{
    if (!m_leftWidget)
        m_leftWidget = new QLabel();

    QHBoxLayout *lableLayout = new QHBoxLayout;
    lableLayout->addWidget(m_leftWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);

    mainLayout->addLayout(lableLayout, 0);
    mainLayout->addWidget(m_switchBtn, 0, Qt::AlignVCenter);

    m_leftWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    setLayout(mainLayout);

    connect(m_switchBtn, &DSwitchButton::toggled, this, &SwitchWidget::checkedChanged);
}

void SwitchWidget::setChecked(const bool checked)
{
    m_switchBtn->blockSignals(true);
    m_switchBtn->setChecked(checked);
    m_switchBtn->blockSignals(false);
}

QString SwitchWidget::title() const
{
    QLabel *label = qobject_cast<QLabel *>(m_leftWidget);
    if (label) {
        return label->text();
    }

   return QString();
}

void SwitchWidget::setTitle(const QString &title)
{
    QLabel *label = qobject_cast<QLabel *>(m_leftWidget);
    if (label) {
        label->setWordWrap(true);
        label->setText(title);
        label->setWordWrap(true);
    }

    setAccessibleName(title);
    m_switchBtn->setAccessibleName(title);
}

bool SwitchWidget::checked() const
{
    return m_switchBtn->isChecked();
}

void SwitchWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_switchBtn->geometry().contains(event->pos())) {
        Q_EMIT clicked();
    }

    return SettingsItem::mouseReleaseEvent(event);
}

}
}
