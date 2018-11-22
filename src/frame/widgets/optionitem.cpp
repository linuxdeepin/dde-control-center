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

#include "widgets/optionitem.h"
#include "widgets/translucentframe.h"

#include <QMouseEvent>

namespace dcc {
namespace widgets {

OptionItem::OptionItem(QWidget *parent) :
    OptionItem("", parent)
{

}

OptionItem::OptionItem(QString title, bool selected, QWidget *parent)
    : SettingsItem(parent),
      m_title(title),
      m_selected(selected),

      m_mainLayout(new QVBoxLayout),
      m_titleLayout(new QHBoxLayout),
      m_titleWidget(new NormalLabel(title)),
      m_contentWidget(nullptr),
      m_selectedIcon(new QLabel)
{
    m_titleWidget->setObjectName("OptionItemTitle");
    m_selectedIcon->setObjectName("OptionItemSelectedIcon");
    m_selectedIcon->setVisible(m_selected);
    m_selectedIcon->setFixedSize(16, 16);

    m_titleFrame = new TranslucentFrame;
    m_titleFrame->setFixedHeight(36);
    m_titleFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_titleFrame->setLayout(m_titleLayout);

    m_titleLayout->setSpacing(0);
    m_titleLayout->setMargin(0);
    m_titleLayout->addWidget(m_titleWidget);
    m_titleLayout->addStretch();
    m_titleLayout->addWidget(m_selectedIcon);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(m_titleFrame);

    setLayout(m_mainLayout);
}

void OptionItem::setTitle(const QString &title)
{
    m_title = title;

    if (m_titleWidget) {
        NormalLabel *label = qobject_cast<NormalLabel*>(m_titleWidget);
        if (label) label->setText(m_title);
    }

    setAccessibleName(m_title);
}

void OptionItem::setTitleFixedHeight(int height)
{
    m_titleFrame->setFixedHeight(height);
}

void OptionItem::setTitleWidget(QWidget *titleWidget)
{
    if (m_titleWidget) {
        m_titleLayout->removeWidget(m_titleWidget);
        m_titleWidget->deleteLater();
    }

    m_titleLayout->insertWidget(0, titleWidget);
    m_titleWidget = titleWidget;
}

void OptionItem::setContentWidget(QWidget *contentWidget)
{
    if (m_contentWidget) {
        m_mainLayout->removeWidget(m_contentWidget);
        m_contentWidget->deleteLater();
    }

    m_mainLayout->addWidget(contentWidget);
    m_contentWidget = contentWidget;
}

void OptionItem::setContentsMargins(int left, int top, int right, int bottom)
{
    m_mainLayout->setContentsMargins(left, top, right, bottom);
}

void OptionItem::mouseReleaseEvent(QMouseEvent *e)
{
    SettingsItem::mouseReleaseEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }

    Q_EMIT selectedChanged();
}

void OptionItem::setSelected(bool selected)
{
    if (m_selected != selected) {
        m_selected = selected;
        m_selectedIcon->setVisible(selected);
    }
}


}
}
