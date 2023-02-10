/*
 * Copyright (C) 2021 ~ 2022 Untion Tech Technology Co., Ltd.
 *
 * Author:     lvpeilong <lvpeilong@uniontech.com>
 *
 * Maintainer: lvpeilong <lvpeilong@uniontech.com>
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
#ifndef DCC_WIDGETS_SECURITYLEVEL_H
#define DCC_WIDGETS_SECURITYLEVEL_H

#include "interface/namespace.h"

#include <dtkwidget_global.h>

#include <QWidget>

#define PASSWORD_LEVEL_ICON_NUM 3
DWIDGET_BEGIN_NAMESPACE
class DLineEdit;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class SecurityLevelItem : public QWidget
{
    Q_OBJECT
public:
    explicit SecurityLevelItem(QWidget *parent = nullptr);
    ~SecurityLevelItem();

    void setUser(const QString &userName);
    void bind(DTK_WIDGET_NAMESPACE::DLineEdit *lineEdit);

    enum Level { NoneLevel = 0, LowLevel, MidLevel, HighLevel };
    Q_ENUM(Level)

    enum IconType { NoneIcon = 0, RedIcon, YellowIcon, GreenIcon, ICONTYPE_NR_ITEMS };
    Q_ENUM(IconType)

public:
    void setLevel(Level level);

private:
    const QPixmap loadSvgImg(const QString &fileName, const int width, const int hight);
    void initUi();
    void initIcons();

private:
    QLabel *m_newPasswdLevelText;
    QLabel *m_newPasswdLevelIcons[PASSWORD_LEVEL_ICON_NUM];
    QPixmap m_icons[ICONTYPE_NR_ITEMS];
    Level m_level;
    QString m_userName;
};
} // namespace DCC_NAMESPACE

#endif // DCC_WIDGETS_SECURITYLEVEL_H
