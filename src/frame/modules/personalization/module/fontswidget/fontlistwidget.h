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

#ifndef FONTLISTWIDGET_H
#define FONTLISTWIDGET_H

#include "widgets/contentwidget.h"
#include "widgets/translucentframe.h"
#include <QObject>
#include <QVBoxLayout>
#include <QApplication>
#include <QJsonObject>
#include <QList>
#include <QLabel>
#include <QFont>
#include <QMap>

namespace dcc
{
namespace widgets
{
class TranslucentFrame;
class SettingsGroup;
class OptionItem;
}
namespace personalization
{
class FontModel;
class FontListWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit FontListWidget(const QString &title, QWidget *parent = 0);

Q_SIGNALS:
    void requestSetDefault(const QJsonObject &value);

public Q_SLOTS:
    void setModel(FontModel *const model);
    void setList(const QList<QJsonObject> &list);
    void setDefault(const QString &name);
    void onItemClicked();

private:
    FontModel                               *m_model;
    QVBoxLayout                             *m_mainLayout;
    widgets::TranslucentFrame               *m_widget;
    widgets::SettingsGroup                  *m_mainGroup;
    QMap<widgets::OptionItem *, QJsonObject> m_valueMap;
};
}
}


#endif // FONTLISTWIDGET_H
