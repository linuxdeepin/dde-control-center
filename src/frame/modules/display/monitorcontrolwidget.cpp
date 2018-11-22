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

#include "monitorcontrolwidget.h"
#include "monitorsground.h"
#include "displaymodel.h"
#include "monitorproxywidget.h"
#include "widgets/basiclistdelegate.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>

namespace dcc {

namespace display {

MonitorControlWidget::MonitorControlWidget(QWidget *parent)
    : QFrame(parent),

      m_screensGround(new MonitorsGround),
      m_recognize(new IconButton),
      m_split(new IconButton),
      m_join(new IconButton)
{
    m_recognize->setText(tr("Recognize"));
    m_recognize->setCursor(Qt::PointingHandCursor);
    m_recognize->setObjectName("Recognize");
    m_recognize->setHoverIcon(QIcon(loadPixmap(":/display/themes/dark/icons/recognize_hover.png")));
    m_recognize->setPressIcon(QIcon(loadPixmap(":/display/themes/dark/icons/recognize_press.png")));
    m_recognize->setNormalIcon(QIcon(loadPixmap(":/display/themes/dark/icons/recognize_normal.png")));

    m_split->setText(tr("Split"));
    m_split->setCursor(Qt::PointingHandCursor);
    m_split->setObjectName("Split");
    m_split->setHoverIcon(QIcon(loadPixmap(":/display/themes/dark/icons/split_hover.png")));
    m_split->setPressIcon(QIcon(loadPixmap(":/display/themes/dark/icons/split_press.png")));
    m_split->setNormalIcon(QIcon(loadPixmap(":/display/themes/dark/icons/split_normal.png")));

    m_join->setText(tr("Merge"));
    m_join->setCursor(Qt::PointingHandCursor);
    m_join->setObjectName("Merge");
    m_join->setHoverIcon(QIcon(loadPixmap(":/display/themes/dark/icons/union_hover.png")));
    m_join->setPressIcon(QIcon(loadPixmap(":/display/themes/dark/icons/union_press.png")));
    m_join->setNormalIcon(QIcon(loadPixmap(":/display/themes/dark/icons/union_normal.png")));

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_recognize);
    btnsLayout->addWidget(m_split);
    btnsLayout->addWidget(m_join);
    btnsLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_screensGround);
    mainLayout->setAlignment(m_screensGround, Qt::AlignHCenter);
    mainLayout->addLayout(btnsLayout);

    setLayout(mainLayout);

    connect(m_recognize, &QPushButton::clicked, this, &MonitorControlWidget::requestRecognize);
    connect(m_join, &QPushButton::clicked, this, &MonitorControlWidget::requestMerge);
    connect(m_split, &QPushButton::clicked, this, &MonitorControlWidget::requestSplit);
    connect(m_screensGround, &MonitorsGround::requestApplySettings, this, &MonitorControlWidget::requestSetMonitorPosition);
    connect(m_screensGround, &MonitorsGround::requestMonitorPress, this, &MonitorControlWidget::requestMonitorPress);
    connect(m_screensGround, &MonitorsGround::requestMonitorRelease, this, &MonitorControlWidget::requestMonitorRelease);
}

void MonitorControlWidget::setDisplayModel(DisplayModel *model)
{
    m_screensGround->setDisplayModel(model);
}

void MonitorControlWidget::setScreensMerged(const bool merged)
{
    m_split->setVisible(merged);
    m_join->setVisible(!merged);
}

} // namespace display

} // namespace dcc
