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

#ifndef LOGOITEM_H
#define LOGOITEM_H

#include "widgets/settingsitem.h"

class QLabel;
namespace DCC_NAMESPACE {

class LogoItem : public SettingsItem
{
    Q_OBJECT

    Q_PROPERTY(QString logo READ logo WRITE setLogo DESIGNABLE true SCRIPTABLE true)

public:
    explicit LogoItem(QFrame *parent = 0);
    void setDescription(const QString &des);
    void setDescription(bool isVisible);//修改m_description是否默认不显示
    void setLogo(const QString &logo);
    const QString logo();

private:
    QLabel *m_logo;
    QLabel *m_description;
};

}
#endif // LOGOITEM_H
