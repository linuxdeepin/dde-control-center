#include "miracastsettings.h"
#include "translucentframe.h"
#include "labels/normallabel.h"
#include "types/linkinfolist.h"
#include "settingsheaderitem.h"

#include <QTimer>

using namespace dcc;
using namespace dcc::display;

MiracastPage::MiracastPage(const QString &title, QWidget *parent) : ContentWidget(parent)
{
    setTitle(title);

    m_timer = new QTimer(this);
    m_timer->setInterval(1 * 60 * 1000);

    m_deviceSwBtn = new SwitchWidget;
    m_deviceSwBtn->setTitle(title);

    SettingsGroup *switchGrp = new SettingsGroup;
    switchGrp->appendItem(m_deviceSwBtn);

    m_tip = new NormalLabel(tr("WIFI will be disconnected when enable screen projection"));
    m_tip->setWordWrap(true);

    DImageButton *refreshBtn = new DImageButton;
    refreshBtn->setText(tr("Refresh"));
    refreshBtn->setObjectName("RefreshBtn");

    QHBoxLayout *rightLayout = new QHBoxLayout;
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(refreshBtn);
    rightLayout->addSpacing(10);

    QWidget *rightWidget = new QWidget;
    rightWidget->setLayout(rightLayout);

    m_deviceGrp = new SettingsGroup;
    m_deviceGrp->setHeaderVisible(true);
    m_deviceGrp->headerItem()->setTitle(tr("Device List"));
    m_deviceGrp->headerItem()->setRightWidget(rightWidget);
    m_nodevice = new MiracastNoDeviceWidget;
    m_nodevice->setVisible(false);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(10);

    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(switchGrp);
    m_mainLayout->addWidget(m_tip);
    m_mainLayout->addWidget(m_deviceGrp);
    m_mainLayout->addWidget(m_nodevice);
    m_mainLayout->addStretch();

    TranslucentFrame *widget = new TranslucentFrame;
    widget->setLayout(m_mainLayout);

    setContent(widget);

    connect(m_deviceSwBtn, &SwitchWidget::checkedChanged, this, &MiracastPage::onDeviceEnableChanged);
    connect(m_deviceSwBtn, &SwitchWidget::checkedChanged, this, &MiracastPage::onDeviceStateChanged);
    connect(m_nodevice, &MiracastNoDeviceWidget::requestRefreshed, this, &MiracastPage::onRefreshed);
    connect(refreshBtn, &DImageButton::clicked, this, &MiracastPage::onRefreshed);
    connect(m_timer, &QTimer::timeout, this, &MiracastPage::onTimeOut);
}

void MiracastPage::setModel(MiracastDeviceModel *model)
{
    m_model = model;

    connect(model, &MiracastDeviceModel::addItem, this, &MiracastPage::onItemAdded);
    connect(model, &MiracastDeviceModel::removeItem, this, &MiracastPage::onItemRemoved);
    connect(model, &MiracastDeviceModel::linkManageChanged, m_deviceSwBtn, &SwitchWidget::setChecked);
    connect(model, &MiracastDeviceModel::linkManageChanged, this, &MiracastPage::onDeviceTurnoff);
    connect(model, &MiracastDeviceModel::destroyed, this, &MiracastPage::back);

    m_deviceSwBtn->setChecked(model->linkInfo().m_managed);
    m_tip->setVisible(!m_deviceSwBtn->checked());

    m_nodevice->setVisible(m_model->sinkList().isEmpty() && m_deviceSwBtn->checked());
    m_deviceGrp->setVisible(!m_model->sinkList().isEmpty() && m_deviceSwBtn->checked());

    for (MiracastItem *item : model->sinkList()) {
        onItemAdded(item);
    }
}

void MiracastPage::onItemAdded(MiracastItem *item)
{
    connect(item, &MiracastItem::requestSinkConnect, this, &MiracastPage::requestDeviceConnect);
    connect(item, &MiracastItem::requestSinkDisConnect, this, &MiracastPage::requestDeviceDisConnect);

    m_deviceGrp->appendItem(item);

    m_deviceGrp->setVisible(true);
    m_nodevice->setVisible(false);
}

void MiracastPage::onItemRemoved(MiracastItem *item)
{
    m_deviceGrp->removeItem(item);
    item->deleteLater();

    if (m_model->sinkList().isEmpty()) {
        m_deviceGrp->setVisible(false);
        m_nodevice->setVisible(true);
        m_nodevice->setState(MiracastNoDeviceWidget::NoDevice);
    }
}

void MiracastPage::onDeviceEnableChanged(const bool enable)
{
    emit requestDeviceEnable(m_model->linkInfo().m_dbusPath, enable);
}

void MiracastPage::onRefreshed()
{
    emit requestDeviceRefreshed(m_model->linkInfo().m_dbusPath, true);
    m_deviceGrp->setVisible(false);
    m_nodevice->setVisible(true);
    m_nodevice->setState(MiracastNoDeviceWidget::Refreshed);
    m_timer->start();
}

void MiracastPage::onDeviceStateChanged(const bool state)
{
    m_tip->setVisible(!state);
    m_nodevice->setVisible(state);
    m_deviceGrp->setVisible(!m_model->sinkList().isEmpty() && m_deviceSwBtn->checked());
    if (state) {
        m_nodevice->setState(MiracastNoDeviceWidget::Refreshed);
        m_timer->start();
    } else {
        m_timer->stop();
    }
}

void MiracastPage::onDeviceTurnoff(const bool state)
{
    if (state)
        return;

    m_tip->setVisible(!state);
    m_deviceGrp->setVisible(false);
    m_nodevice->setVisible(false);
}

void MiracastPage::onTimeOut()
{
    emit requestDeviceRefreshed(m_model->linkInfo().m_dbusPath, false);
    m_timer->stop();
    m_nodevice->setVisible(m_model->sinkList().isEmpty() && m_deviceSwBtn->checked());
    m_nodevice->setState(MiracastNoDeviceWidget::NoDevice);
}
