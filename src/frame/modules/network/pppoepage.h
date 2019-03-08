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

#ifndef PPPOEPAGE_H
#define PPPOEPAGE_H

#include "widgets/contentwidget.h"
#include "connectioneditpage.h"

#include <QMap>
#include <QPointer>

class QVBoxLayout;
class QPushButton;

namespace dde {
namespace network {
class NetworkModel;
}
}

namespace dcc {

namespace widgets {
    class SettingsGroup;
    class SwitchWidget;
    class LoadingNextPageWidget;
}

namespace network {

class PppoePage : public ContentWidget
{
    Q_OBJECT
public:
    explicit PppoePage(QWidget *parent = 0);
    ~PppoePage();

    void setModel(dde::network::NetworkModel *model);

Q_SIGNALS:
    void requestNextPage(ContentWidget * const w) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;

private Q_SLOTS:
    void createPPPoEConnection();
    void onConnectionListChanged();
    void onConnectionDetailClicked();
    void onPPPoESelected();
    void onActiveConnectionChanged(const QList<QJsonObject> &conns);

private:
    dde::network::NetworkModel *m_model;

    widgets::SettingsGroup *m_settingsGrp;
    QPushButton *m_createBtn;
    QMap<widgets::LoadingNextPageWidget *, QString> m_connUuid;
    QPointer<ConnectionEditPage> m_editPage;

    QString m_editingUuid;
};

}

}

#endif // PPPOEPAGE_H
