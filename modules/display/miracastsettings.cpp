#include "miracastsettings.h"
#include "translucentframe.h"
#include "labels/normallabel.h"

using namespace dcc;
using namespace dcc::display;

MiracastPage::MiracastPage(const QString &title, QWidget *parent) : ContentWidget(parent)
{
    setTitle(title);

    SwitchWidget *switchWidget = new SwitchWidget;
    switchWidget->setTitle(title);

    SettingsGroup *switchGrp = new SettingsGroup;
    switchGrp->appendItem(switchWidget);

    NormalLabel *tip = new NormalLabel(tr("WIFI will be disconnected when enable screen projection"));
    tip->setWordWrap(true);

    m_deviceGrp = new SettingsGroup;
    m_nodevice = new MiracastNoDevicePage;

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(10);

    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(switchGrp);
    m_mainLayout->addWidget(tip);
    m_mainLayout->addWidget(m_deviceGrp);
    m_mainLayout->addWidget(m_nodevice);
    m_mainLayout->addStretch();

    TranslucentFrame *widget = new TranslucentFrame;
    widget->setLayout(m_mainLayout);

    setContent(widget);

    connect(switchWidget, &SwitchWidget::checkedChanged, this, &MiracastPage::requestSwitchDevice);
    connect(switchWidget, &SwitchWidget::checkedChanged, this, [=](const bool state) {
        tip->setVisible(!state);
    });
}
