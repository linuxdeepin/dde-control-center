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

#ifndef ACCESSPOINTWIDGET_H
#define ACCESSPOINTWIDGET_H

#include "settingsitem.h"

class QVBoxLayout;
class QLabel;

namespace Dtk {

namespace Widget {

class DImageButton;

}

}

namespace dcc {

namespace network {

class AccessPointWidget : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit AccessPointWidget(QWidget *parent = 0);

    int strength() const { return m_strength; }
    bool connected() const { return m_connected; }
    const QString path() const { return m_path; }

Q_SIGNALS:
    void requestEdit(const QString &path, const QString &ssid) const;
    void requestConnect(const QString &path, const QString &ssid) const;

public Q_SLOTS:
    void setStrength(const int strength);
    void setEncyrpt(const bool encyrpt);
    void setConnected(const bool connected);
    void setEditable(const bool editable);
    void setAPName(const QString &name);
    void setPath(const QString &path);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QLabel *m_lockIcon;
    QLabel *m_strengthIcon;
    QLabel *m_apName;
    Dtk::Widget::DImageButton *m_activeIcon;
    Dtk::Widget::DImageButton *m_detailBtn;
    QVBoxLayout *m_mainLayout;

    int m_strength;
    bool m_connected;
    QString m_path;
};

}   // namespace network

}   // namespace dcc

#endif // ACCESSPOINTWIDGET_H
