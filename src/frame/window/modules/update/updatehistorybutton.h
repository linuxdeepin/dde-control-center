// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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


