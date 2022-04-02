/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef FORMATSETTINGMODULE_H
#define FORMATSETTINGMODULE_H
#include "interface/moduleobject.h"

DCC_BEGIN_NAMESPACE
class SettingsGroup;
DCC_END_NAMESPACE

class DatetimeModel;
class DatetimeWorker;

class FormatSettingModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit FormatSettingModule(DatetimeModel *model, DatetimeWorker *work, QObject *parent = nullptr);

private:
    void initTimeFotmat(DCC_NAMESPACE::SettingsGroup *timeGrp);

private:
    DatetimeModel *m_model;
    DatetimeWorker *m_work;

    const QStringList m_fotmatWeek;
    const QStringList m_fotmatLongDate;
    const QStringList m_fotmatShortDate;
    const QStringList m_fotmatLongTime;
    const QStringList m_fotmatShortTime;
    const QStringList m_weekStartWithDay;
};

#endif // FORMATSETTINGMODULE_H
