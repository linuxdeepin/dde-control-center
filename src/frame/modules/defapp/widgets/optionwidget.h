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

#ifndef OPTIONWIDGET_H
#define OPTIONWIDGET_H

#include "widgets/settingsitem.h"
#include "widgets/labels/normallabel.h"
#include "../model/category.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <dimagebutton.h>
#include <QJsonObject>
#include <QObject>

namespace dcc
{
namespace defapp
{
DWIDGET_USE_NAMESPACE
class OptionWidget : public widgets::SettingsItem
{
    Q_OBJECT
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)

public:
    explicit OptionWidget(const bool state = false, QWidget *parent = 0);
    inline const App getItem() { return m_CategoryItem;}
    void setItem(const App &item);
    bool checked() const;
    void setCheckedIcon(const QPixmap &icon);
    bool userCheck() const;
    QHBoxLayout *layout();

public Q_SLOTS:
    void setChecked(const bool checked);
    void setDelete(const bool delchecked);
    void setMime(const QString &mime);

private:
    QHBoxLayout  *m_mainlayout;
    QLabel       *m_optionIcon;
    QLabel       *m_execPath;
    DImageButton *m_delete;
    QLabel       *m_checkedIconLabel;
    widgets::NormalLabel  *m_displayName;
    bool          m_checked           = false;
    bool          m_delchecked        = false;
    bool          m_userCheck         = false;
    QString       m_mime;
    App m_CategoryItem;

protected:
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void removeItem(const App &item);
    void setDefault();
    void checkedChanged(const bool checked) const;
};
}
}



#endif // OPTIONWIDGET_H
