#include "cbandsection.h"
#include <networkmanagerqt/utils.h>
#include "widgets/comboxwidget.h"
#include <qcombobox.h>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;


CbandSection::CbandSection(NetworkManager::WirelessSetting::Ptr wiredSetting, QFrame *parent):AbstractSection(parent), m_bandChooser(new ComboxWidget(this))
{
    m_settings = wiredSetting;
    initUi();
}

CbandSection::~CbandSection()
{

}

bool CbandSection::allInputValid()
{
    return true;
}

void CbandSection::saveSettings()
{
    m_settings->setBand(m_bandList.value(m_bandChooser->comboBox()->currentText()));
}

void CbandSection::initUi()
{
    m_bandList.insert("5Ghz", NetworkManager::WirelessSetting::FrequencyBand::A);
    m_bandList.insert("2.4Ghz", NetworkManager::WirelessSetting::FrequencyBand::Bg);
    m_bandChooser->setTitle("Band");
    m_bandChooser->comboBox()->addItems(m_bandList.keys());

    int index = 0;
    for (; index < m_bandChooser->comboBox()->count(); ++index) {
        if (m_bandList.value(m_bandChooser->comboBox()->itemText(index)) == m_settings->band()) {
            m_bandChooser->comboBox()->setCurrentIndex(index);
            break;
        }
    }

    appendItem(m_bandChooser);
}
