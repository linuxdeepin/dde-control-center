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
 *             listenerri <listenerri@gmail.com>
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

#include "pppoepage.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/translucentframe.h"
#include "widgets/loadingnextpagewidget.h"
#include "widgets/switchwidget.h"

#include <networkmodel.h>
#include <wireddevice.h>
#include <DHiDPIHelper>
#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dcc::network;
using namespace dde::network;

PppoePage::PppoePage(QWidget *parent)
    : ContentWidget(parent),
      m_settingsGrp(new SettingsGroup),
      m_createBtn(new QPushButton)
{
    m_createBtn->setText(tr("Create PPPoE Connection"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_settingsGrp);
    mainLayout->addWidget(m_createBtn);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addSpacing(10);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("PPP"));

    connect(m_createBtn, &QPushButton::clicked, this, &PppoePage::createPPPoEConnection);
}

PppoePage::~PppoePage()
{
    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void PppoePage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(model, &NetworkModel::connectionListChanged, this, &PppoePage::onConnectionListChanged);
    connect(model, &NetworkModel::activeConnectionsChanged, this, &PppoePage::onActiveConnectionChanged);

    onConnectionListChanged();
}

void PppoePage::createPPPoEConnection()
{
    m_editPage = new ConnectionEditPage(ConnectionEditPage::ConnectionType::PppoeConnection, "/");
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &PppoePage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &PppoePage::requestFrameKeepAutoHide);

    Q_EMIT requestNextPage(m_editPage);
}

void PppoePage::onConnectionListChanged()
{
    m_settingsGrp->clear();
    qDeleteAll(m_connUuid.keys());
    m_connUuid.clear();

    for (const auto &pppoe : m_model->pppoes())
    {
        const auto name = pppoe.value("Id").toString();
        const auto uuid = pppoe.value("Uuid").toString();

        LoadingNextPageWidget *w = new LoadingNextPageWidget;
        w->setTitle(name);

        connect(w, &LoadingNextPageWidget::acceptNextPage,  this, &PppoePage::onConnectionDetailClicked);
        connect(w, &LoadingNextPageWidget::selected, this, &PppoePage::onPPPoESelected);

        m_settingsGrp->appendItem(w);
        m_connUuid[w] = uuid;
    }

    onActiveConnectionChanged(m_model->activeConns());
}

void PppoePage::onConnectionDetailClicked()
{
    LoadingNextPageWidget *w = static_cast<LoadingNextPageWidget *>(sender());
    Q_ASSERT(w && m_connUuid.contains(w));

    m_editingUuid = m_connUuid[w];

    m_editPage = new ConnectionEditPage(ConnectionEditPage::ConnectionType::PppoeConnection, "/", m_editingUuid);
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &PppoePage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &PppoePage::requestFrameKeepAutoHide);

    Q_EMIT requestNextPage(m_editPage);
}

void PppoePage::onPPPoESelected()
{
    LoadingNextPageWidget *w = static_cast<LoadingNextPageWidget *>(sender());
    Q_ASSERT(w && m_connUuid.contains(w));

    m_editingUuid = m_connUuid[w];
    Q_EMIT requestActivateConnection("/", m_editingUuid);
}

void PppoePage::onActiveConnectionChanged(const QList<QJsonObject> &conns)
{
    for (LoadingNextPageWidget *widget : m_connUuid.keys()) {
        widget->setIcon(QPixmap());
        widget->setLoading(false);
    }

    for (const QJsonObject &connObj : conns) {
        const QString &uuid = connObj.value("Uuid").toString();
        LoadingNextPageWidget *w = m_connUuid.key(uuid);
        // the State of Active Connection
        // 0:Unknow, 1:Activating, 2:Activated, 3:Deactivating, 4:Deactivated
        if (w) {
            int state = m_model->activeConnObjectByUuid(uuid).value("State").toInt(0);
            if(state == 2) {
                w->setIcon(DHiDPIHelper::loadNxPixmap(":/network/themes/dark/icons/select.svg"));
                w->setLoading(false);
            } else if(state == 1) {
                w->setLoading(true);
            } else {
                w->setLoading(false);
            }
        }
    }
}
