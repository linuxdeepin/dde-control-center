#include "vpnpage.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "translucentframe.h"
#include "networkmodel.h"
#include "nextpagewidget.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonObject>

using namespace dcc::widgets;
using namespace dcc::network;

VpnPage::VpnPage(QWidget *parent)
    : ContentWidget(parent),

      m_vpnSwitch(new SwitchWidget),
      m_vpnGroup(new SettingsGroup)
{
    m_vpnSwitch->setTitle(tr("VPN Status"));
    m_vpnGroup->appendItem(m_vpnSwitch);

    QPushButton *createVpnBtn = new QPushButton;
    createVpnBtn->setText(tr("Create VPN"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_vpnGroup);
    mainLayout->addWidget(createVpnBtn);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("VPN"));

    connect(m_vpnSwitch, &SwitchWidget::checkedChanegd, this, &VpnPage::requestVpnEnabled);
}

void VpnPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(m_model, &NetworkModel::vpnEnabledChanged, m_vpnSwitch, &SwitchWidget::setChecked);

    m_vpnSwitch->setChecked(m_model->vpnEnabled());

    refershVpnList(m_model->vpns());
}

void VpnPage::refershVpnList(const QList<QJsonObject> &vpnList)
{
    for (const auto &vpn : vpnList)
    {
        NextPageWidget *w = new NextPageWidget;
        w->setTitle(vpn.value("Id").toString());

        m_vpnGroup->appendItem(w);
    }
}
