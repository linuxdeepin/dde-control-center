// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mirrorsourceitem.h"

using namespace DCC_NAMESPACE::update;

MirrorSourceItem::MirrorSourceItem(QObject *parent)
    : QObject(parent)
    , m_leftLabel("")
    , m_rightLabel(tr("Untested"))
    , m_bSelect(false)
    , m_speed(0)
{
    m_action = new DViewItemAction;
    m_action->setText(m_rightLabel);
    m_action->setFontSize(DFontSizeManager::T8);
    this->setText(m_leftLabel);
    this->setCheckState(Qt::CheckState::Unchecked);
    this->setActionList(Qt::RightEdge, {m_action});
}

void MirrorSourceItem::setMirrorState(const QString &value)
{
    if ("" != value && m_rightLabel != value) {
        m_rightLabel = value;
        m_action->setText(m_rightLabel);
    }
}

void MirrorSourceItem::setMirrorName(const QString &value)
{
    if ("" != value && m_leftLabel != value) {
        m_leftLabel = value;
        this->setText(m_leftLabel);
    }
}

void MirrorSourceItem::setSelected(bool state)
{
    if (m_bSelect != state) {
        m_bSelect = state;
        this->setCheckState(m_bSelect ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }
}

void MirrorSourceItem::setMirrorInfo(const MirrorInfo &info, const QString &defaultValue)
{
    m_info = info;
    setMirrorName(info.m_name);
    setMirrorState(defaultValue);
}

MirrorInfo MirrorSourceItem::mirrorInfo() const
{
    return m_info;
}

bool MirrorSourceItem::getSelectState()
{
    return m_bSelect;
}

QString MirrorSourceItem::getMirrorName()
{
    return m_leftLabel;
}

int MirrorSourceItem::speed() const
{
    return m_speed;
}

void MirrorSourceItem::setSpeed(const int time)
{
    if (m_speed != time) {
        m_speed = time;

        QString sp = "";
        if (time == 10000) {
            sp = tr("Timeout");
        } else if (time > 2000) {
            sp = tr("Slow");
        } else if (time > 200)
            sp = tr("Medium");
        else {
            sp = tr("Fast");
        }

        setMirrorState(sp);
    }
}

void MirrorSourceItem::setTesting()
{
    setMirrorState("...");
}
