/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liumaochuan <liumaochuan@uniontech.com>
 *
 * Maintainer: liumaochuan <liumaochuan@uniontech.com>
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

#ifndef SYNCITEMWIGET_H
#define SYNCITEMWIGET_H

#include <QWidget>
#include <QCheckBox>
#include <DGuiApplicationHelper>
#include "modules/sync/utils.h"

using namespace dcc::cloudsync;

class QLabel;
class syncItemwiget : public QWidget
{
    Q_OBJECT
public:
    explicit syncItemwiget(QString icon, QString name, bool checked, QWidget *parent = nullptr);
    QCheckBox *getCheckBox();
    void setSyncType(SyncType syncType);

Q_SIGNALS:
    void toTellStateChanged(bool state, SyncType syncType);

private Q_SLOTS:
    void onThemeChanged();
    void onStateChanged(int state);

private:
    QCheckBox *m_checkBox;
    QLabel *m_iconLabel;
    QString m_qstrIconName;
    SyncType m_syncType;
};

#endif // SYNCITEMWIGET_H
