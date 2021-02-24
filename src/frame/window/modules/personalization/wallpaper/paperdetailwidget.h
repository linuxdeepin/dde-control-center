/*
 * Copyright (C) 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     liuxing <liuxing@uniontech.com>
 *
 * Maintainer: liuxing <liuxing@uniontech.com>
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

#ifndef PAPERDETAILWIDGET_H
#define PAPERDETAILWIDGET_H

#include "interface/namespace.h"

#include <DWidget>
#include <DPushButton>

const QString WallPaperPath = "/usr/share/wallpapers/deepin/";

namespace DCC_NAMESPACE {
namespace personalization {

class PaperIconButton;

class PaperDetailWidget : public Dtk::Widget::DWidget
{
    Q_OBJECT
public:
    explicit PaperDetailWidget(QWidget *parent = nullptr);
    void setPixmapIndex(const int index);
    const QStringList &allWallpaperPaths() const;

Q_SIGNALS:
    void wallPaperClicked();
    void cancelClicked();
    void requestSetDesktop(const QString &wallpaper);
    void requestSetLock(const QString &wallpaper);
    void finishSetWallpaper();

public Q_SLOTS:
    void hideSwitchButton();
    void switchToNext();
    void switchToPrevious();
    void onWallpaperClicked();
    void onConfirmClicked();
    void onSetDesktop();
    void onSetLock();
    void onSetBoth();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

private:
    void initConnect();
    void initVisibility();

private:
    QPixmap m_pixmap;
    QStringList m_paperPaths;
    int m_curIndex;

    // 预览容器窗口及预览按钮
    QWidget *m_previewWidget;
    PaperIconButton *m_toPrevious;
    PaperIconButton *m_toNext;
    PaperIconButton *m_cancel;
    PaperIconButton *m_confirm;
    QTimer *m_hideTimer;

    // 设置组容器窗口及设置按钮
    QWidget *m_settingsWidget;
    Dtk::Widget::DPushButton *m_setDesktop;
    Dtk::Widget::DPushButton *m_setLock;
    Dtk::Widget::DPushButton *m_setBoth;

    QPoint m_pressPos;
};
}
}

#endif // PAPERDETAILWIDGET_H
