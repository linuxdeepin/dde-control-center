/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#pragma once

#include "themeitem.h"

#include <QWidget>
#include <QBoxLayout>
#include <QJsonObject>

namespace dcc {
namespace personalization {
class ThemeModel;
}
}

namespace DCC_NAMESPACE {
namespace personalization {
class ThemeItem;
class PerssonalizationThemeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PerssonalizationThemeWidget(QWidget *parent = nullptr);
    void setModel(dcc::personalization::ThemeModel *const model);
    void setMainLayout(QBoxLayout *layout, bool titleBelowPic);

Q_SIGNALS:
    void requestSetDefault(const QJsonObject &value);

public Q_SLOTS:
    void setDefault(const QString &name);
    void onItemClicked(const bool selected);
    void onSetPic(const QString &id, const QString &picPath);
    void onAddItem(const QJsonObject &json);
    void onRemoveItem(const QString &id);
protected:
    void mouseMoveEvent(QMouseEvent* event)override;
    QBoxLayout *m_centerLayout;
    QMap<ThemeItem *, QJsonObject> m_valueMap;
    dcc::personalization::ThemeModel *m_model;
    bool m_titleBelowPic;
};
}
}
