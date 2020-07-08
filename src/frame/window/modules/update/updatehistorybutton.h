/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include <QLabel>
#include <QPaintEvent>

namespace DCC_NAMESPACE {
namespace update {

class UpdateHistoryButton : public dcc::widgets::SettingsItem
{
    Q_OBJECT
    enum clickState {
        MOUSE_DEFAULT,
        MOUSE_PRESS,
        MOUSE_RELEASE,
        MOUSE_MOVE,
        MOUSE_DOUBLE,
    };

public:
    explicit UpdateHistoryButton(QFrame *parent = 0);

    void setImage(const QString &fileName);
    void setLabelText(QString text);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

Q_SIGNALS:
    void notifyBtnClicked();
    void notifyBtnRelease(bool);

private Q_SLOTS:
    void onProcessMouseEvent();

private:
    QLabel *m_messageLabel;
    QLabel *m_label;
    clickState m_mouseType;
    QPixmap m_picture;
    bool m_bIsRecentUpdate;
};

} // namespace update
} // namespace DCC_NAMESPACE


