/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author: quezhiyong@uniontech.com
 *         wangwei@uniontech.com
 *
 * Maintainer: quezhiyong@uniontech.com
 *             wangwei@uniontech.com
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
#ifndef TOUCHSCREENPAGE_H
#define TOUCHSCREENPAGE_H

#include <interface/namespace.h>

#include "widgets/contentwidget.h"

#include <org_freedesktop_notifications.h>

#include <DListView>

#include <QWidget>
#include <QComboBox>

using Notifications = org::freedesktop::Notifications;

class QVBoxLayout;
class QGridLayout;

namespace dcc {

namespace display {
class DisplayModel;
}

namespace widgets {
class ButtonTuple;
}

} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {

class MCombobox : public QComboBox
{
    Q_OBJECT
protected:
    void showPopup() override;
};

class TouchscreenPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit TouchscreenPage(QWidget *parent = nullptr);

    void setModel(dcc::display::DisplayModel *model);

private Q_SLOTS:
    void onMonitorChanged();
    void save();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

Q_SIGNALS:
    void requestAssociateTouch(const QString &monitor, const QString &touchscreenSerial);

private:
    dcc::display::DisplayModel *m_model{nullptr};
    dcc::widgets::ButtonTuple *m_buttonTuple;
    QScrollArea *m_contentArea;
    QList<MCombobox *> m_list;
    Notifications *m_notifyInter;
    QStringList m_titleName;
    QList<QLabel *> m_labels;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif //! TOUCHSCREENWIDGET_H
