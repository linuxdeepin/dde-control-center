//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include <dtkwidget_global.h>

#include <QFrame>

DWIDGET_BEGIN_NAMESPACE
class DBackgroundGroup;
DWIDGET_END_NAMESPACE

class QVBoxLayout;

namespace DCC_NAMESPACE {

class SettingsItem;
class SettingsHeaderItem;

class SettingsGroup : public QFrame
{
    Q_OBJECT

public:
    enum BackgroundStyle {
        ItemBackground = 0,
        GroupBackground,
        NoneBackground
    };

    explicit SettingsGroup(QFrame *parent = nullptr, BackgroundStyle bgStyle = ItemBackground);
    explicit SettingsGroup(const QString &title, QFrame *parent = nullptr);
    ~SettingsGroup();

    SettingsHeaderItem *headerItem() const { return m_headerItem; }
    void setHeaderVisible(const bool visible);

    SettingsItem *getItem(int index);
    void insertWidget(QWidget *widget);
    void insertItem(const int index, SettingsItem *item);
    void appendItem(SettingsItem *item);
    void appendItem(SettingsItem *item, BackgroundStyle bgStyle);
    void removeItem(SettingsItem *item);
    void moveItem(SettingsItem *item, const int index);
    void setSpacing(const int spacing);

    int itemCount() const;
    void clear();
    QVBoxLayout *getLayout() const { return m_layout; }

    void setBackgroundStyle(BackgroundStyle bgStyle);
    BackgroundStyle backgroundStyle() const { return m_bgStyle; }

protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    BackgroundStyle m_bgStyle{ItemBackground};
    QVBoxLayout *m_layout;
    SettingsHeaderItem *m_headerItem;
    DTK_WIDGET_NAMESPACE::DBackgroundGroup *m_bggroup{nullptr};
};

}
