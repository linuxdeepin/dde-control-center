/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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
#include "interface/namespace.h"

#include "widgets/settingsitem.h"

namespace DCC_NAMESPACE {
namespace commoninfo {
class CommonBackgroundItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit CommonBackgroundItem(QFrame *parent = nullptr);

Q_SIGNALS:
    void requestEnableTheme(const bool state);
    void requestSetBackground(const QString &path);

public Q_SLOTS:
    void setThemeEnable(const bool state);
    void updateBackground(const QPixmap &pixmap);

protected:
    void paintEvent(QPaintEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void resizeEvent(QResizeEvent *e) override;

private:
    QPixmap m_background;
    QPixmap m_basePixmap;
    bool m_isDrop;
    bool m_themeEnable;
};
} // namespace commoninfo
} // namespace DCC_NAMESPACE
