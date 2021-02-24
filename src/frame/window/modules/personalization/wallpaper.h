/*
   * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     xiechuan <xiechuan@uniontech.com>
   *
   * Maintainer: xiechaun <xiechaun@uniontech.com>
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

#ifndef WALLPAPER_H
#define WALLPAPER_H

#include "interface/namespace.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QStandardItem;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class SettingsGroup;
}
} // namespace dcc

namespace DCC_NAMESPACE {
namespace personalization {
class WallpaperSnippet;
class Wallpaper : public QWidget
{
    Q_OBJECT
public:
    explicit Wallpaper(QWidget *parent = nullptr);
    void initUi();
    void setLockPaperPath(QString path);
    void setDesktopPaperPath(QString path);

private:
    QStandardItemModel *m_model;
    dcc::widgets::SettingsGroup *m_group;
    WallpaperSnippet *m_snippet;
};
} // namespace personalization
} // namespace DCC_NAMESPACE

#endif // WALLPAPER_H
