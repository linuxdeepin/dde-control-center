// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FORMATSETTINGMODULE_H
#define FORMATSETTINGMODULE_H
#include "interface/pagemodule.h"
#include "settingsitem.h"

#include <QLabel>
#include <QWidget>

namespace DCC_NAMESPACE {
class SettingsGroup;
}

using DCC_NAMESPACE::SettingsItem;
class DatetimeModel;
class DatetimeWorker;

class RegionSettingBtn final : public SettingsItem
{
    Q_OBJECT

public:
    explicit RegionSettingBtn(const QString &region, QWidget *parent = nullptr);

signals:
    void clicked();

public slots:
    void setRegion(const QString &region);

private:
    QLabel *m_regionLabel;
};

class InforShowUnit final : public QWidget
{
    Q_OBJECT
public:
    explicit InforShowUnit(const QString &title,
                           const QString &info = {},
                           QWidget *parent = nullptr);
    void setInfo(const QString &info);

private:
    QLabel *m_info;
};

class FormatShowGrid final : public SettingsItem
{
    Q_OBJECT
public:
    explicit FormatShowGrid(QWidget *parent = nullptr);
};

class RegionAndFormatModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit RegionAndFormatModule(DatetimeModel *model,
                                 DatetimeWorker *work,
                                 QObject *parent = nullptr);

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
