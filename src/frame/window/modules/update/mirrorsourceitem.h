/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <DStandardItem>
#include <types/mirrorinfolist.h>

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
