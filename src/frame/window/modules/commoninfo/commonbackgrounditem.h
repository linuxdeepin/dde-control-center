// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "interface/namespace.h"

#include "widgets/settingsitem.h"

namespace DCC_NAMESPACE {
namespace commoninfo {
class CommonBackgroundItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit CommonBackgroundItem(QWidget *parent = nullptr);

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
