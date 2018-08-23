#include "secretwirelesssection.h"
#include "contentwidget.h"

using namespace dcc::widgets;

static const QList<NetworkManager::WirelessSecuritySetting::KeyMgmt> KeyMgmtList {
    NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone,
    NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep,
    NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk,
    NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap
};
static const QMap<QString, NetworkManager::WirelessSecuritySetting::KeyMgmt> KeyMgmtStrMap {
    {"None", NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone},
    {"WEP 40/128-bit", NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep},
    {"WPA/WPA2 Personal", NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk},
    {"WPA/WPA2 Enterprise", NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap}
};

SecretWirelessSection::SecretWirelessSection(NetworkManager::WirelessSecuritySetting::Ptr wsSeting,
        NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent)
    : Secret8021xSection(sSetting, parent),
      m_keyMgmtChooser(new ComboBoxWidget(this)),
      m_passwdEdit(new PasswdEditWidget(this)),
      m_enableWatcher(new Secret8021xEnableWatcher(this)),
      m_currentKeyMgmt(NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone),
      m_wsSetting(wsSeting),
      m_s8Setting(sSetting)
{
    // init KeyMgmt
    const NetworkManager::WirelessSecuritySetting::KeyMgmt &keyMgmt = m_wsSetting->keyMgmt();
    m_currentKeyMgmt = (keyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Unknown) ?
        NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone : keyMgmt;

    initUI();
    initConnection();

    onKeyMgmtChanged(m_currentKeyMgmt);
}

SecretWirelessSection::~SecretWirelessSection()
{
}

bool SecretWirelessSection::allInputValid()
{
    bool valid = true;

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep
            || m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        valid = !m_passwdEdit->text().isEmpty();
        m_passwdEdit->setIsErr(!valid);
    }

    return valid ? Secret8021xSection::allInputValid() : false;
}

void SecretWirelessSection::saveSettings()
{
    m_wsSetting->setKeyMgmt(m_currentKeyMgmt);
    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep) {
        m_wsSetting->setWepKey0(m_passwdEdit->text());
    } else if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        m_wsSetting->setPsk(m_passwdEdit->text());
    }

    m_wsSetting->setInitialized(true);

    Secret8021xSection::saveSettings();
}

void SecretWirelessSection::initUI()
{
    m_keyMgmtChooser->setTitle(tr("Security"));
    for (auto keyMgmt : KeyMgmtList) {
        m_keyMgmtChooser->appendOption(KeyMgmtStrMap.key(keyMgmt), keyMgmt);
    }
    m_keyMgmtChooser->setCurrent(m_currentKeyMgmt);

    //m_passwdEdit->setText("wadfasdf");
    m_passwdEdit->setPlaceholderText(tr("Required"));

    m_enableWatcher->setSecretEnable(m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap);

    QList<NetworkManager::Security8021xSetting::EapMethod> eapMethodsWantedList;
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodLeap);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodFast);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTtls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodPeap);

    init(m_enableWatcher, eapMethodsWantedList);

    insertItem(1, m_keyMgmtChooser);
    insertItem(2, m_passwdEdit);
}

void SecretWirelessSection::initConnection()
{
    connect(m_keyMgmtChooser, &ComboBoxWidget::requestPage, this, &SecretWirelessSection::requestPage);
    connect(m_keyMgmtChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onKeyMgmtChanged(data.value<NetworkManager::WirelessSecuritySetting::KeyMgmt>());
    });

    connect(m_passwdEdit->textEdit(), &QLineEdit::editingFinished, this, &SecretWirelessSection::saveUserInputPassword);
    connect(m_passwdEdit->textEdit(), &QLineEdit::editingFinished, this, &SecretWirelessSection::allInputValid);
}

void SecretWirelessSection::onKeyMgmtChanged(NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt)
{
    if (m_currentKeyMgmt != keyMgmt) {
        m_currentKeyMgmt = keyMgmt;
    }

    switch (m_currentKeyMgmt) {
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone: {
            m_passwdEdit->setVisible(false);
            m_enableWatcher->setSecretEnable(false);
            break;
        }
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep: {
            m_passwdEdit->setText(m_wsSetting->wepKey0());
            m_passwdEdit->setTitle(tr("Key"));
            m_passwdEdit->setVisible(true);
            m_enableWatcher->setSecretEnable(false);
            break;
        }
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk: {
            m_passwdEdit->setText(m_wsSetting->psk());
            m_passwdEdit->setTitle(tr("Password"));
            m_passwdEdit->setVisible(true);
            m_enableWatcher->setSecretEnable(false);
            break;
        }
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap: {
            m_passwdEdit->setVisible(false);
            m_enableWatcher->setSecretEnable(true);
            break;
        }
        default:
            break;
    }

    if (m_userInputPasswordMap.contains(m_currentKeyMgmt)) {
        m_passwdEdit->setText(m_userInputPasswordMap.value(m_currentKeyMgmt));
    }
}

void SecretWirelessSection::saveUserInputPassword()
{
    m_userInputPasswordMap.insert(m_currentKeyMgmt, m_passwdEdit->text());
}
