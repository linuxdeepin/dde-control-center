/*
 * Copyright (C) 2011 ~ 2021 Uniontech Technology Co., Ltd.
 *
 * Author:     huangweihua <huangweihua@uniontech.com>
 *
 * Maintainer: huangweihua <huangweihua@uniontech.com>
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

#ifndef TOUCHSCREENWIDGET_H
#define TOUCHSCREENWIDGET_H

#include <interface/namespace.h>

#include <org_freedesktop_notifications.h>

#include <DTipLabel>

#include <QComboBox>

using Notifications = org::freedesktop::Notifications;

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace display {
class TouchscreenModel;
}

namespace widgets {
class ButtonTuple;
class SettingsItem;
} // namespace widgets

} // namespace dcc

namespace DCC_NAMESPACE {

namespace touchscreen {

class MCombobox : public QComboBox
{
    Q_OBJECT
protected:
    void showPopup() override;
};

class TouchscreenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TouchscreenWidget(QWidget *parent = nullptr);

    void setModel(dcc::display::TouchscreenModel *model);

private Q_SLOTS:
    void onMonitorChanged();
    void save();
    void clearShake();

Q_SIGNALS:
    void requestAssociateTouch(const QString &monitor, const QString &touchscreenUUID);

private:
    dcc::display::TouchscreenModel *m_model;

    QVBoxLayout *m_contentLayout;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_dTipLabel;
    QList<dcc::widgets::SettingsItem *> m_settingsItem;
    QList<MCombobox *> m_list;
    dcc::widgets::ButtonTuple *m_buttonTuple;
    QTimer *m_timer;

    Notifications *m_notifyInter;
};

} // namespace touchscreen

} // namespace DCC_NAMESPACE

#endif // TOUCHSCREENWIDGET_H
