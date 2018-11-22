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

#ifndef DEFCATEGORYWIDGET_H
#define DEFCATEGORYWIDGET_H

#include "modules/modulewidget.h"
#include "widgets/translucentframe.h"
#include "model/category.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QJsonObject>
#include <QFileInfo>

namespace dcc
{
namespace widgets {
class SettingsHead;
class SettingsGroup;
}
namespace defapp
{
class DefCategoryAddWidget;
class Category;
class OptionWidget;
class DefCategoryWidget : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit DefCategoryWidget(const QString &name, QWidget *parent);
    void setCategory(Category * const category);

Q_SIGNALS:
    void requestSetDefaultApp(const QString &category, const App &item);
    void requestDelUserApp(const QString &name, const App &item);
    void requestFrameAutoHide(const bool autoHide) const;
    void requestCreateFile(const QString &category, const QFileInfo &info);

public Q_SLOTS:
    void setDefault();
    void onDefaultAppSet(const App &app);
    void slotEditMode(bool edit);
    void setCategoryName(const QString &name);

private:
    void AppsItemChanged(const QList<App> &list);
    void addItem(const dcc::defapp::App &item);
    void removeItem(const App &item);

private:
    QVBoxLayout           *m_centralLayout;
    widgets::SettingsGroup    *m_userGroup;
    QWidget               *m_listWidget;
    QVBoxLayout           *m_listLayout;
    QLayoutItem           *m_userGroupLayout;
    DefCategoryAddWidget  *m_addWidget;
    widgets::SettingsHead *m_headWidget;
    QString                m_categoryName;
    Category              *m_category;
    QMap<OptionWidget *, App>  m_valueMap;
    QList<App> m_userMap;
};

}
}

#endif // DEFCATEGORYWIDGET_H
