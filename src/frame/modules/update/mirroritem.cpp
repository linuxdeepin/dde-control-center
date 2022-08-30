// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mirroritem.h"

#include <DApplicationHelper>

#include <QHBoxLayout>
#include <QResizeEvent>
#include "window/utils.h"

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace dcc::update;

MirrorItem::MirrorItem(QFrame *parent)
    : SettingsItem(parent)
    , m_selected(false)
    , m_speed(0)
    , m_selectedBtn(new DFloatingButton(DStyle::SP_IndicatorChecked, this))
    , m_mirrorName(new SmallLabel)
    , m_mirrorSpeed(new SmallLabel(tr("Untested")))
    , m_layout(new QHBoxLayout(this))
{
    setMinimumHeight(36);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_mirrorName->setWordWrap(true);

    m_selectedBtn->setObjectName("DCC-Update-MirrorItem-SelectedBtn");
    m_selectedBtn->setFixedSize(24, 24);
    m_selectedBtn->setVisible(false);

    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(20, 10, 20, 10);
    m_layout->addWidget(m_mirrorName);
    m_layout->addStretch();
    m_layout->addWidget(m_selectedBtn);
    m_layout->addSpacing(List_Interval);
    m_layout->addWidget(m_mirrorSpeed);

    setLayout(m_layout);
}

void MirrorItem::setMirrorInfo(const MirrorInfo &info)
{
    m_info = info;
    setMirrorName(info.m_name);
}

void MirrorItem::setSelected(bool state)
{
    if (state == m_selected)
        return;

    m_selected = state;
    m_selectedBtn->setVisible(state);

    Q_EMIT selectStateChanged(state);
}

void MirrorItem::setSpeed(const int time)
{
    m_speed = time;

    if (time == 10000)
        m_mirrorSpeed->setText(tr("Timeout"));
    else if (time > 2000)
        m_mirrorSpeed->setText(tr("Slow"));
    else if (time > 200)
        m_mirrorSpeed->setText(tr("Medium"));
    else
        m_mirrorSpeed->setText(tr("Fast"));
}

void MirrorItem::setTesting()
{
    m_mirrorSpeed->setText("...");
}

void MirrorItem::setMirrorName(const QString &name)
{
    m_mirrorName->setText(name);
}

void MirrorItem::mouseReleaseEvent(QMouseEvent *e)
{
    Q_EMIT clicked(this);

    QFrame::mouseReleaseEvent(e);
}

int MirrorItem::speed() const
{
    return m_speed;
}

int MirrorItem::calcSpeedTextWidth() const
{
    QFontMetrics fm = m_mirrorSpeed->fontMetrics();
    return qMax(fm.width(tr("Timeout")),
                qMax(fm.width(tr("Slow")),
                     qMax(fm.width(tr("Medium")),
                          qMax(fm.width(tr("Fast")),
                               fm.width(tr("Untested"))))));
}
