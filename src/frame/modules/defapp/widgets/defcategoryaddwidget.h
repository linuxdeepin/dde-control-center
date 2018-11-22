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

#ifndef DEFCATEGORYADDWIDGET_H
#define DEFCATEGORYADDWIDGET_H

#include "widgets/settingsitem.h"
#include "funcbutton.h"
#include <QObject>
#include <QFileInfo>
namespace dcc
{
namespace widgets {
class SettingsItem;
}
namespace defapp
{
class FuncButton;
class DefCategoryAddWidget : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit DefCategoryAddWidget(QWidget *parent = 0);
    inline void setCategory(const QString &category){ m_category = category;}

Q_SIGNALS:
    void requestFrameAutoHide(const bool autoHide) const;
    void requestCreateFile(const QString &category, const QFileInfo &info);

public Q_SLOTS:
    void clicked();

private:
    FuncButton *m_add;
    QString m_category;
};
}
}



#endif // DEFCATEGORYADDWIDGET_H
