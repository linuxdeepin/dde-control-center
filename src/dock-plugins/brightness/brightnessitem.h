// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "commoniconbutton.h"
#include "brightnessapplet.h"

#include <QWidget>

namespace Dock {
class TipsWidget;
}

class BrightnessItem : public QObject
{
    Q_OBJECT

public:
    explicit BrightnessItem(QObject *parent = nullptr);
    ~BrightnessItem();

    QWidget *itemWidget() const { return m_icon; };
    QWidget *tipsWidget();
    BrightnessApplet *popupApplet() const { return m_applet; }
    const QString contextMenu() const;
    void invokeMenuItem(const QString menuId, const bool checked);
    void updateTips();

signals:
    void requestHideApplet();

private:
    void init();

private:
    QLabel *m_tipsLabel;
    BrightnessApplet *m_applet;
    CommonIconButton *m_icon;
    QPixmap m_iconPixmap;
};
