/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef THEME_H
#define THEME_H

#include "widgets/settingsitem.h"
#include "widgets/translucentframe.h"
#include "widgets/settingsgroup.h"

#include <QObject>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QMap>

namespace dcc
{
namespace widgets
{
class OptionItem;
class SettingsGroup;
class ThemeItem;
}
namespace personalization
{
class ThemeModel;
class Theme : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit Theme(const QString &title, QWidget *parent = 0);
    void setModel(ThemeModel *const model);

Q_SIGNALS:
    void requestSetDefault(const QJsonObject &value);

public Q_SLOTS:
    void setDefault(const QString &name);
    void onItemClicked(const bool selected);
    void onSetPic(const QString &id, const QString &picPath);
    void onAddItem(const QJsonObject &json);
    void onRemoveItem(const QString &id);

private:
    QVBoxLayout                             *m_mainLayout;
    widgets::SettingsGroup                  *m_mainGroup;
    QMap<widgets::ThemeItem *, QJsonObject> m_valueMap;
    ThemeModel                              *m_model;
};
}
}

#endif // THEME_H
