#include "networkmodel.h"
#include "wirelesspage.h"
#include "wirelessdevice.h"
#include "accesspointwidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "translucentframe.h"
#include "connectioneditpage.h"
#include "connectionsessionworker.h"
#include "connectionsessionmodel.h"

#include <QDebug>
#include <QVBoxLayout>

using namespace dcc::widgets;
using namespace dcc::network;

WirelessPage::WirelessPage(WirelessDevice *dev, QWidget *parent)
    : ContentWidget(parent),

      m_device(dev),

      m_listGroup(new SettingsGroup),

      m_connectHideSSID(new AccessPointWidget(this))
{
    m_sortDelayTimer.setInterval(100);
    m_sortDelayTimer.setSingleShot(true);

    m_connectHideSSID->setAPName(tr("Connect to hidden network"));
    m_connectHideSSID->setStrength(-1);

    m_listGroup->appendItem(m_connectHideSSID);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_listGroup);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
#ifdef QT_DEBUG
    setTitle(dev->path());
#else
    setTitle(tr("WLAN"));
#endif

    connect(&m_sortDelayTimer, &QTimer::timeout, this, &WirelessPage::sortAPList);
    connect(m_connectHideSSID, &AccessPointWidget::requestEdit, this, &WirelessPage::showConnectHidePage);
    connect(dev, &WirelessDevice::apAdded, this, &WirelessPage::onAPAdded);
    connect(dev, &WirelessDevice::apInfoChanged, this, &WirelessPage::onAPChanged);
    connect(dev, &WirelessDevice::apRemoved, this, &WirelessPage::onAPRemoved);
    connect(dev, &WirelessDevice::removed, this, &WirelessPage::onDeviceRemoved);
    connect(dev, &WirelessDevice::sessionCreated, this, &WirelessPage::showAPEditPage);

    // init data
    QTimer::singleShot(0, this, [=] {
        const QString devPath = m_device->path();

        emit requestDeviceAPList(devPath);
    });
}

WirelessPage::~WirelessPage()
{
    if (!m_apEditPage.isNull())
        m_apEditPage->deleteLater();
}

void WirelessPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(m_model, &NetworkModel::activeConnInfoChanged, this, &WirelessPage::onActiveConnInfoChanged);

    onActiveConnInfoChanged(m_model->activeConnInfos());
}

void WirelessPage::onAPAdded(const QJsonObject &apInfo)
{
    const QString ssid = apInfo.value("Ssid").toString();

    AccessPointWidget *w = new AccessPointWidget;

    w->setConnected(ssid == m_activeApName);
#ifdef QT_DEBUG
    w->setAPName(QString::number(apInfo.value("Strength").toInt()) + " " + ssid);
#else
    w->setAPName(ssid);
#endif

    connect(w, &AccessPointWidget::requestEdit, [=](const QString &path) { emit requestEditAP(m_device->path(), path); });
    connect(w, &AccessPointWidget::requestConnect, [=](const QString &path, const QString &ssid) { emit requestConnectAp(m_device->path(), path, ssid); });

    m_apItems.insert(ssid, w);
    m_listGroup->appendItem(w);

    onAPChanged(apInfo);
}

void WirelessPage::onAPChanged(const QJsonObject &apInfo)
{
    const QString ssid = apInfo.value("Ssid").toString();
    if (!m_apItems.contains(ssid))
        return;

    const QString path = apInfo.value("Path").toString();
    const int strength = apInfo.value("Strength").toInt();

    AccessPointWidget *w = m_apItems[ssid];

    if (w->path() == path)
    {
        w->setStrength(strength);
    }
    else if (strength > w->strength())
    {
        w->setStrength(strength);
        w->setPath(path);
    }

    w->setEncyrpt(apInfo.value("Secured").toBool());

    m_sortDelayTimer.start();
}

void WirelessPage::onAPRemoved(const QString &ssid)
{
    if (!m_apItems.contains(ssid))
        return;

    AccessPointWidget *w = m_apItems[ssid];

    m_apItems.remove(ssid);
    m_listGroup->removeItem(w);
    w->deleteLater();
}

void WirelessPage::onDeviceRemoved()
{
    // back if ap edit page exist
    if (!m_apEditPage.isNull())
        m_apEditPage->onDeviceRemoved();

    // destory self page
    emit back();
}

void WirelessPage::sortAPList()
{
    auto cmpFunc = [=](const AccessPointWidget *a, const AccessPointWidget *b) {
        if (a->connected() != b->connected())
            return a->connected();
        return a->strength() > b->strength();
    };

    QList<AccessPointWidget *> sortedList;
    for (auto it(m_apItems.cbegin()); it != m_apItems.cend(); ++it)
    {
        const auto index = std::upper_bound(sortedList.begin(), sortedList.end(), it.value(), cmpFunc);

        sortedList.insert(index, it.value());
    }

    // sort list
    for (int i(0); i != sortedList.size(); ++i)
        m_listGroup->moveItem(sortedList[i], i);
}

void WirelessPage::onActiveConnInfoChanged(const QList<QJsonObject> &activeConns)
{
    const QString hwAddr = m_device->hwAddr();
    for (const auto &info : activeConns)
    {
        if (hwAddr == info.value("HwAddress").toString())
        {
            m_activeApName = info.value("ConnectionName").toString();
            return updateActiveAp();
        }
    }

    m_activeApName.clear();
    updateActiveAp();
}

void WirelessPage::showConnectHidePage()
{
//    if (m_connectHidePage.isNull())
//    {
//        m_connectHidePage = new ConnectHiddenPage(this);
//        connect(m_connectHidePage, &ConnectHiddenPage::requestNextPage, this, &WirelessPage::requestNextPage);
//    }

//    emit requestNextPage(m_connectHidePage);

    emit requestCreateConnection("wireless", m_device->path());
}

void WirelessPage::showAPEditPage(const QString &session)
{
    // ensure edit page is empty
    Q_ASSERT(m_apEditPage.isNull());

    m_apEditPage = new ConnectionEditPage;

    ConnectionSessionModel *sessionModel = new ConnectionSessionModel(m_apEditPage);
    ConnectionSessionWorker *sessionWorker = new ConnectionSessionWorker(session, sessionModel, m_apEditPage);

    m_apEditPage->setModel(m_model, sessionModel);
    connect(m_apEditPage, &ConnectionEditPage::requestCancelSession, sessionWorker, &ConnectionSessionWorker::closeSession);
    connect(m_apEditPage, &ConnectionEditPage::requestChangeSettings, sessionWorker, &ConnectionSessionWorker::changeSettings);
    connect(m_apEditPage, &ConnectionEditPage::accept, sessionWorker, &ConnectionSessionWorker::saveSettings);
    connect(m_apEditPage, &ConnectionEditPage::requestNextPage, this, &WirelessPage::requestNextPage);

    emit requestNextPage(m_apEditPage);
}

void WirelessPage::updateActiveAp()
{
    for (auto it(m_apItems.cbegin()); it != m_apItems.cend(); ++it)
        it.value()->setConnected(it.key() == m_activeApName);

    sortAPList();
}
