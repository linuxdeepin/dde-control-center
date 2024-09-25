// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "mirrorinfolist.h"

#include <DStandardItem>

#include <QWidget>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
namespace update {

class MirrorSourceItem : public QObject, public DStandardItem
{
    Q_OBJECT
public:
    MirrorSourceItem(QObject *parent = nullptr);

    void setMirrorState(const QString &value);
    void setMirrorName(const QString &value);

    void setSelected(bool state);
    bool getSelectState();

    void setMirrorInfo(const MirrorInfo &info, const QString &defaultValue = "");
    MirrorInfo mirrorInfo() const;
    QString getMirrorName();

    int speed() const;
    void setSpeed(const int time);
    void setTesting();

private:
    DViewItemAction *m_action;
    QString m_leftLabel;
    QString m_rightLabel;
    bool m_bSelect;
    MirrorInfo m_info;
    int m_speed;
};

}
}
