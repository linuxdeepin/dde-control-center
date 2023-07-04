//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FORMATSETTINGMODULE_H
#define FORMATSETTINGMODULE_H
#include "interface/pagemodule.h"

namespace DCC_NAMESPACE {
class SettingsGroup;
}

class DatetimeModel;
class DatetimeWorker;

class ZoneAndFormatModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit ZoneAndFormatModule(DatetimeModel *model, DatetimeWorker *work, QObject *parent = nullptr);

private:
    void initDateFotmat(DCC_NAMESPACE::SettingsGroup *dateGrp);
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
