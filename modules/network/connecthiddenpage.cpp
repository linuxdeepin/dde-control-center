#include "connecthiddenpage.h"
#include "nextpagewidget.h"
#include "lineeditwidget.h"
#include "translucentframe.h"
#include "settingsgroup.h"

#include <QPushButton>
#include <QVBoxLayout>

using namespace dcc::widgets;
using namespace dcc::network;

ConnectHiddenPage::ConnectHiddenPage(QWidget *parent)
    : ContentWidget(parent),

      m_ssid(new LineEditWidget),
      m_passwd(new LineEditWidget),
      m_enctyptType(new NextPageWidget),
      m_cancel(new QPushButton),
      m_accept(new QPushButton)
{
    m_ssid->setTitle(tr("Name"));
    m_passwd->setTitle(tr("Password"));
    m_enctyptType->setTitle(tr("Encryption way"));
    m_cancel->setText(tr("Cancel"));
    m_accept->setText(tr("Connect"));

    SettingsGroup *settingsGroup = new SettingsGroup;
    settingsGroup->appendItem(m_ssid);
    settingsGroup->appendItem(m_enctyptType);
    settingsGroup->appendItem(m_passwd);

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(m_cancel);
    btnsLayout->addWidget(m_accept);
    btnsLayout->setSpacing(0);
    btnsLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(settingsGroup);
    mainLayout->addLayout(btnsLayout);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("Connect to hidden network"));

    connect(m_cancel, &QPushButton::clicked, this, &ConnectHiddenPage::back);
}

void ConnectHiddenPage::onDeviceRemoved()
{
    emit back();
}
