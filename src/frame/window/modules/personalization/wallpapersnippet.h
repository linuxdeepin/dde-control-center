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

#ifndef WALLPAPERSNIPPET_H
#define WALLPAPERSNIPPET_H

#include "widgets/settingsitem.h"
#include "interface/namespace.h"

DGUI_BEGIN_NAMESPACE
class DSvgRenderer;
DGUI_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
class QHBoxLayout;
QT_END_NAMESPACE

using namespace dcc::widgets;

namespace DCC_NAMESPACE {
namespace personalization {
/**
 * @brief The WallpaperItem class 壁纸缩略图
 */
class WallpaperItem : public QWidget
{
    Q_OBJECT
public:
    explicit WallpaperItem(QString path = QString(), QWidget *parent = nullptr);
    void setPath(const QString &picPath);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_path;
    DTK_GUI_NAMESPACE::DSvgRenderer *m_render;
};

/**
 * @brief The WallpaperSnippet class 显示壁纸缩略图列表
 */
class WallpaperSnippet : public SettingsItem
{
    Q_OBJECT
public:
    explicit WallpaperSnippet(QWidget *parent = nullptr);
    void initUi();
    WallpaperItem *addItem(QString path, QString text);
    void setLockPaperPath(QString &path) { Q_EMIT lockPaperChanged(path); }
    void setDesktopPaperPath(QString &path) { Q_EMIT desktopPaperChanged(path); }

private:
    QHBoxLayout *m_mainLayout;

Q_SIGNALS:
    void lockPaperChanged(QString &path);
    void desktopPaperChanged(QString &path);
};
} // namespace personalization
} // namespace DCC_NAMESPACE

#endif // WALLPAPERSNIPPET_H
