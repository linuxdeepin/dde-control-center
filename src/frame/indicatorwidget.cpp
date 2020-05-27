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

#include "indicatorwidget.h"

#include <QHBoxLayout>
#include <QWheelEvent>
#include <QTimer>

DWIDGET_USE_NAMESPACE

IndicatorWidget::IndicatorWidget(QWidget *parent)
    : QWidget(parent),

      m_pluginsIndicator(new DPageIndicator),
      m_prevPluginBtn(new DImageButton),
      m_nextPluginBtn(new DImageButton),

      m_ignoreWheelRepeat(new QTimer(this))
{
    QHBoxLayout *centralLayout = new QHBoxLayout;

    centralLayout->addStretch();
    centralLayout->addWidget(m_prevPluginBtn);
    centralLayout->addSpacing(15);
    centralLayout->addWidget(m_pluginsIndicator);
    centralLayout->addSpacing(15);
    centralLayout->addWidget(m_nextPluginBtn);
    centralLayout->addStretch();
    centralLayout->setMargin(0);

    m_prevPluginBtn->setObjectName("PrevBtn");
    m_prevPluginBtn->setAccessibleName("PrevBtn");
    m_nextPluginBtn->setObjectName("NextBtn");
    m_nextPluginBtn->setAccessibleName("NextBtn");
    m_pluginsIndicator->setObjectName("DPIndicator");

    m_prevPluginBtn->setVisible(false);
    m_nextPluginBtn->setVisible(false);

    m_pluginsIndicator->setFixedHeight(40);
    m_pluginsIndicator->setPageCount(1);
    m_pluginsIndicator->setCurrentPage(0);
    m_pluginsIndicator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_ignoreWheelRepeat->setSingleShot(true);
    m_ignoreWheelRepeat->setInterval(200);

    connect(m_nextPluginBtn, &DImageButton::clicked, m_pluginsIndicator, &DPageIndicator::nextPage);
    connect(m_nextPluginBtn, &DImageButton::clicked, this, &IndicatorWidget::requestNext);
    connect(m_prevPluginBtn, &DImageButton::clicked, m_pluginsIndicator, &DPageIndicator::previousPage);
    connect(m_prevPluginBtn, &DImageButton::clicked, this, &IndicatorWidget::requestPrevious);

    setLayout(centralLayout);
}

void IndicatorWidget::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);

    m_prevPluginBtn->setVisible(true);
    m_nextPluginBtn->setVisible(true);
}

void IndicatorWidget::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);

    m_prevPluginBtn->setVisible(false);
    m_nextPluginBtn->setVisible(false);
}

void IndicatorWidget::wheelEvent(QWheelEvent *e)
{
    QWidget::wheelEvent(e);

    if (m_ignoreWheelRepeat->isActive())
        return;
    m_ignoreWheelRepeat->start();

    if (e->delta() < 0)
    {
        m_pluginsIndicator->nextPage();
        Q_EMIT requestNext();
    }
    else
    {
        m_pluginsIndicator->previousPage();
        Q_EMIT requestPrevious();
    }
}
