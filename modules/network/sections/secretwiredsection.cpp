#include "secretwiredsection.h"

using namespace dcc::widgets;

SecretWiredSection::SecretWiredSection(NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent)
    : Secret8021xSection(sSetting, parent),
      m_secretEnable(new SwitchWidget(this)),
      m_enableWatcher(new Secret8021xEnableWatcher(this))
{
    m_secretEnable->setTitle(tr("Security Required"));
    m_secretEnable->setChecked(!sSetting->toMap().isEmpty());

    m_enableWatcher->setSecretEnable(m_secretEnable->checked());

    connect(m_secretEnable, &SwitchWidget::checkedChanged, m_enableWatcher, &Secret8021xEnableWatcher::setSecretEnable);
    connect(m_secretEnable, &SwitchWidget::checkedChanged, this, &SecretWiredSection::allInputValid);

    QList<NetworkManager::Security8021xSetting::EapMethod> eapMethodsWantedList;
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodMd5);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodFast);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTtls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodPeap);

    init(m_enableWatcher, eapMethodsWantedList);

    insertItem(1, m_secretEnable);
}

SecretWiredSection::~SecretWiredSection()
{
}

bool SecretWiredSection::allInputValid()
{
    return Secret8021xSection::allInputValid();
}

void SecretWiredSection::saveSettings()
{
    Secret8021xSection::saveSettings();
}
