/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef MIRACASTITEM_H
#define MIRACASTITEM_H

#include "widgets/settingsitem.h"
#include "widgets/loadingindicator.h"
#include "connectwidget.h"
#include <types/sinkinfolist.h>
#include <QWidget>
#include <QLabel>

using namespace dcc::widgets;

namespace dcc {
namespace display {
class MiracastItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit MiracastItem(const SinkInfo &info, QWidget *parent = 0);
    const SinkInfo info() const;
    void onConnectState(bool state);

Q_SIGNALS:
    void requestSinkConnect(const QDBusObjectPath &path, const QRect &rect);
    void requestSinkDisConnect(const QDBusObjectPath &path);

private Q_SLOTS:
    void onDisConnect();

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

private:
    SinkInfo m_info;
    QLabel *m_title;
    QLabel *m_connect;
    ConnectWidget *m_connectWidget;
};
}
}

#endif // MIRACASTITEM_H
