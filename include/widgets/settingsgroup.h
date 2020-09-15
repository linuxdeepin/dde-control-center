/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef SETTINGSGROUP_H
#define SETTINGSGROUP_H

#include <DBackgroundGroup>

#include <QFrame>
#include <QTimer>

#include "widgets/translucentframe.h"

class QVBoxLayout;

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class SettingsItem;
class SettingsHeaderItem;

class SettingsGroup : public TranslucentFrame
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
    void setSpacing(const int spaceing);

    int itemCount() const;
    void clear();
    QVBoxLayout *getLayout() const { return m_layout; }

private:
    BackgroundStyle m_bgStyle{ItemBackground};
    QVBoxLayout *m_layout;
    SettingsHeaderItem *m_headerItem;
    DTK_WIDGET_NAMESPACE::DBackgroundGroup *m_bggroup{nullptr};
};

}
}

#endif // SETTINGSGROUP_H
