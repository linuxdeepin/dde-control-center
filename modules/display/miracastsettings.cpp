#include "miracastsettings.h"
#include "translucentframe.h"
#include "labels/normallabel.h"
#include "types/linkinfolist.h"

using namespace dcc;
using namespace dcc::display;

MiracastPage::MiracastPage(const QString &title, QWidget *parent) : ContentWidget(parent)
{
    setTitle(title);

    m_deviceSwBtn = new SwitchWidget;
    m_deviceSwBtn->setTitle(title);

    SettingsGroup *switchGrp = new SettingsGroup;
    switchGrp->appendItem(m_deviceSwBtn);

    m_tip = new NormalLabel(tr("WIFI will be disconnected when enable screen projection"));
    m_tip->setWordWrap(true);

    m_deviceGrp = new SettingsGroup;
    m_nodevice = new MiracastNoDevicePage;
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
    connect(m_deviceSwBtn, &SwitchWidget::checkedChanged, this, [=](const bool state) {
        m_tip->setVisible(!state);
    });
}

void MiracastPage::setModel(MiracastDeviceModel *model)
{
    m_model = model;

    connect(model, &MiracastDeviceModel::addItem, this, &MiracastPage::onItemAdded);
    connect(model, &MiracastDeviceModel::removeItem, this, &MiracastPage::onItemRemoved);
    connect(model, &MiracastDeviceModel::linkManageChanged, m_deviceSwBtn, &SwitchWidget::setChecked);
    connect(model, &MiracastDeviceModel::destroyed, this, &MiracastPage::back);

    m_deviceSwBtn->setChecked(model->linkInfo().m_managed);
    m_tip->setVisible(!m_deviceSwBtn->checked());

    for (MiracastItem *item : model->sinkList()) {
        onItemAdded(item);
    }
}

void MiracastPage::onItemAdded(MiracastItem *item)
{
    connect(item, &MiracastItem::requestSinkConnect, this, &MiracastPage::requestDeviceConnect);
    connect(item, &MiracastItem::requestSinkDisConnect, this, &MiracastPage::requestDeviceDisConnect);

    m_deviceGrp->appendItem(item);
}

void MiracastPage::onItemRemoved(MiracastItem *item)
{
    m_deviceGrp->removeItem(item);
    item->deleteLater();
}

void MiracastPage::onDeviceEnableChanged(const bool enable)
{
    emit requestDeviceEnable(m_model->linkInfo().m_dbusPath, enable);
}
