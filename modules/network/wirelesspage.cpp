#include "wirelesspage.h"
#include "wirelessdevice.h"
#include "accesspointwidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "translucentframe.h"
#include "connecthiddenpage.h"
#include "accesspointeditpage.h"
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
      m_switchBtn(new SwitchWidget),

      m_connectHideSSID(new AccessPointWidget(this))
{
    m_sortDelayTimer.setInterval(100);
    m_sortDelayTimer.setSingleShot(true);

    m_switchBtn->setTitle(tr("Status"));

    m_connectHideSSID->setAPName(tr("Connect to hidden network"));
    m_connectHideSSID->setStrength(-1);

    m_listGroup->appendItem(m_switchBtn);
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
    connect(m_switchBtn, &SwitchWidget::checkedChanegd, [=](const bool enabled) { emit requestDeviceEnabled(m_device->path(), enabled); });
    connect(dev, &WirelessDevice::enableChanged, m_switchBtn, &SwitchWidget::setChecked);
    connect(dev, &WirelessDevice::apAdded, this, &WirelessPage::onAPAdded);
    connect(dev, &WirelessDevice::apInfoChanged, this, &WirelessPage::onAPChanged);
    connect(dev, &WirelessDevice::apRemoved, this, &WirelessPage::onAPRemoved);
    connect(dev, &WirelessDevice::removed, this, &WirelessPage::onDeviceRemoved);
    connect(dev, &WirelessDevice::sessionCreated, this, &WirelessPage::showAPEditPage);
    m_switchBtn->setChecked(dev->enabled());

    // init data
    QTimer::singleShot(0, this, [=] {
        const QString devPath = m_device->path();

        emit requestDeviceStatus(devPath);
        emit requestDeviceAPList(devPath);
    });
}

WirelessPage::~WirelessPage()
{
    if (!m_connectHidePage.isNull())
        m_connectHidePage->deleteLater();

    if (!m_apEditPage.isNull())
        m_apEditPage->deleteLater();
}

void WirelessPage::setModel(NetworkModel *model)
{
    m_model = model;
}

void WirelessPage::onAPAdded(const QJsonObject &apInfo)
{
    const QString ssid = apInfo.value("Ssid").toString();

    AccessPointWidget *w = new AccessPointWidget;

    w->setPath(apInfo.value("Path").toString());

    connect(w, &AccessPointWidget::requestEdit, [=] { emit requestEditAP(m_device->path(), w->path()); });

    m_apItems.insert(ssid, w);
    m_listGroup->appendItem(w);

    onAPChanged(apInfo);
}

void WirelessPage::onAPChanged(const QJsonObject &apInfo)
{
    const QString ssid = apInfo.value("Ssid").toString();
    if (!m_apItems.contains(ssid))
        return;

    AccessPointWidget *w = m_apItems[ssid];

#ifdef QT_DEBUG
    w->setAPName(QString::number(apInfo.value("Strength").toInt()) + " " + ssid);
#else
    w->setAPName(ssid);
#endif
    w->setEncyrpt(apInfo.value("Secured").toBool());
    w->setStrength(apInfo.value("Strength").toInt());

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
    // back if this page exist
    if (!m_connectHidePage.isNull())
        m_connectHidePage->onDeviceRemoved();

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
        m_listGroup->moveItem(sortedList[i], i + 1);
}

void WirelessPage::showConnectHidePage()
{
    if (m_connectHidePage.isNull())
    {
        m_connectHidePage = new ConnectHiddenPage(this);
        connect(m_connectHidePage, &ConnectHiddenPage::requestNextPage, this, &WirelessPage::requestNextPage);
    }

    emit requestNextPage(m_connectHidePage);
}

void WirelessPage::showAPEditPage(const QString &session)
{
    if (!m_apEditPage.isNull())
        m_apEditPage->deleteLater();

    m_apEditPage = new AccessPointEditPage(session, this);

    emit requestNextPage(m_apEditPage);
}
