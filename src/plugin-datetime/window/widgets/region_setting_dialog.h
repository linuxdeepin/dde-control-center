//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "datetimedbusproxy.h"

#include <DAbstractDialog>
#include <DDialog>
#include <DListView>
#include <DListWidget>
#include <DSuggestButton>

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QPushButton>

#include <optional>
#include <utility>

using Dtk::Widget::DSuggestButton;
class RegionFormatShowPage final : public QWidget
{
    Q_OBJECT
public:
    explicit RegionFormatShowPage(QWidget *parent = nullptr);
    friend class RegionDialog;

private:
    void updateShowInfo(const std::optional<QString> region);

private:
    QLabel *m_date;
    QLabel *m_time;
    QLabel *m_dateAndTime;
    QLabel *m_number;
    QLabel *m_currency;
};

class RegionDialog final : public Dtk::Widget::DAbstractDialog
{
    Q_OBJECT

public:
    explicit RegionDialog(LocaleList regions, QWidget *parent = nullptr);

    inline std::optional<std::pair<QString, QString>> selectedValue() { return m_selectedValue; }

    enum RegionRole {
        Display = Qt::DisplayRole,
        Key,
    };
    Q_ENUM(RegionRole)

private slots:
    void onFilterChanged(const QString &filter);

private:
    QStandardItemModel *fromListToModel(const LocaleList &regions);

    inline bool hasSelectedIndex() { return m_lastSelectedIndex.has_value(); }


private:
    QLineEdit *m_edit;
    Dtk::Widget::DListView *m_view;
    LocaleList m_regions;
    QSortFilterProxyModel *m_model;
    std::optional<QModelIndex> m_lastSelectedIndex;
    std::optional<std::pair<QString, QString>> m_selectedValue;
    RegionFormatShowPage *m_regionFormatShowPage;
    QPushButton *m_cancelBtn;
    DSuggestButton *m_confirmBtn;
};
