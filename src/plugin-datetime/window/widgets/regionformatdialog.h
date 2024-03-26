//SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef REGIONFORMATDIALOG_H
#define REGIONFORMATDIALOG_H

#include "interface/namespace.h"
#include "regionproxy.h"

#include <DListView>
#include <DAbstractDialog>

DWIDGET_USE_NAMESPACE

class QLabel;
class QSortFilterProxyModel;

class DatetimeModel;

namespace DCC_NAMESPACE {

class RegionFormatDialog : public DAbstractDialog
{
    Q_OBJECT
public:
    enum RegionFormatRole{
        TextRole = DTK_NAMESPACE::UserRole + 1,
        LocaleRole
    };
    explicit RegionFormatDialog(DatetimeModel *model, QWidget *parent = nullptr);
    ~RegionFormatDialog();

    void setCurrentRegion(const QString &region);

Q_SIGNALS:
    void regionFormatSaved(const QString &langRegion, const QLocale &regionFormat);

private Q_SLOTS:
    void onRegionSelected(const QModelIndex &index);
    void onSaved();

private:
    void initItemModel(DatetimeModel *dateTimeModel);
    QLabel* addFormatItem(const QWidget *frame, const QString &name, const QString &format);
    void updateRegionFormat(const QLocale &locale);
    void updateDataModel(QStandardItemModel *model, const QModelIndex &index);

private:
    QLabel *m_dayLabel = nullptr;
    QLabel *m_shortDateLabel = nullptr;
    QLabel *m_longDateLabel = nullptr;
    QLabel *m_shortTimeLabel = nullptr;
    QLabel *m_longTimeLabel = nullptr;
    QLabel *m_currencyLabel = nullptr;
    QLabel *m_numberLabel = nullptr;
    QLabel *m_paperLabel = nullptr;

    Regions m_regions;
    DListView *m_regionListView;
    QPushButton *m_saveBtn;
    QSortFilterProxyModel *m_proxyModel;
    QStandardItemModel *m_model;
    QModelIndex m_searchModelIndex;
    QModelIndex m_lastSelectedIndex;
    bool m_searchStatus = false;
    QString m_langRegion;
    QLocale m_locale;
};
}
#endif // RegionFormatDialog_H
