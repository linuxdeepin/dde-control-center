// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef REGIONMODULE_H
#define REGIONMODULE_H

#include "interface/pagemodule.h"
#include "settingsitem.h"
#include "regionformatdialog.h"
#include "itemmodule.h"

#include <QLabel>
#include <QWidget>

#include <DComboBox>
#include <DListView>
#include <DTipLabel>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

namespace DCC_NAMESPACE {
class SettingsGroup;
}

using DCC_NAMESPACE::SettingsItem;
class DatetimeModel;
class DatetimeWorker;

class RegionModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit RegionModule(DatetimeModel *model,
                          DatetimeWorker *work,
                          QObject *parent = nullptr);

private:
    void initCountryTip(DTipLabel *countryTipLabel);
    void initCountryModule();
    void initRegionFormatTip(DTipLabel *countryTipLabel);
    void initLangRegionModule();
    void initFormatList(DListView *view);
    void initFormatModificationModule();
    void updateRegionFormat(const RegionFormat &regionFormat);
    QString getTranslation(const QString &localeName,const QString &langRegion);

private slots:
    void onLangRegionClicked();

private:
    DatetimeModel *m_model = nullptr;
    DatetimeWorker *m_work = nullptr;
    DComboBox *m_countryCombo = nullptr;
    QLabel *m_langRegionLabel = nullptr;
    DViewItemAction *m_dayAction = nullptr;
    DViewItemAction *m_shortDateAction = nullptr;
    DViewItemAction *m_longDateAction = nullptr;
    DViewItemAction *m_shortTimeAction = nullptr;
    DViewItemAction *m_longTimeAction = nullptr;
    DViewItemAction *m_currencyAction = nullptr;
    DViewItemAction *m_numberAction = nullptr;
    DViewItemAction *m_paperAction = nullptr;
    const QStringList m_fotmatWeek;
    const QStringList m_fotmatLongDate;
    const QStringList m_fotmatShortDate;
    const QStringList m_fotmatLongTime;
    const QStringList m_fotmatShortTime;
    const QStringList m_weekStartWithDay;
    QString m_langRegion;
    QLocale m_locale;
    RegionFormat m_regionFormat;

    ItemModule *m_countryModule = nullptr;
    ItemModule *m_langRegionModule = nullptr;
    ItemModule *m_formatModificationModule = nullptr;

    DListView *m_listView = nullptr;
};

#endif // FORMATSETTINGMODULE_H
