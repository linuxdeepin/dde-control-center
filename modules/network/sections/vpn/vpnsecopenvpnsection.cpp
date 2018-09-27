#include "vpnsecopenvpnsection.h"

using namespace dcc::widgets;
using namespace NetworkManager;

static const QMap<QString, QString> CipherStrMap {
    {"Default", "default"}, {"None", "none"}, {"DES-CBC", "DES-CBC"}, {"RC2-CBC", "RC2-CBC"},
        {"DES-EDE-CBC", "DES-EDE-CBC"}, {"DES-EDE3-CBC", "DES-EDE3-CBC"}, {"DESX-CBC", "DESX-CBC"},
        {"BF-CBC", "BF-CBC"}, {"RC2-40-CBC", "RC2-40-CBC"}, {"CAST5-CBC", "CAST5-CBC"},
        {"RC2-64-CBC", "RC2-64-CBC"}, {"AES-128-CBC", "AES-128-CBC"}, {"AES-192-CBC", "AES-192-CBC"},
        {"AES-256-CBC", "AES-256-CBC"}, {"CAMELLIA-128-CBC", "CAMELLIA-128-CBC"},
        {"CAMELLIA-192-CBC", "CAMELLIA-192-CBC"}, {"CAMELLIA-256-CBC", "CAMELLIA-256-CBC"},
        {"SEED-CBC", "SEED-CBC"}
};

static const QMap<QString, QString> HMACStrMap {
    {"Default", "default"}, {"None", "none"}, {"RSA MD-4", "RSA-MD4"}, {"MD-5", "MD5"}, {"SHA-1", "SHA1"},
        {"SHA-224", "SHA224"}, {"SHA-256", "SHA256"}, {"SHA-384", "SHA384"}, {"SHA-512", "SHA512"},
        {"RIPEMD-160", "RIPEMD160"},
};

VpnSecOpenVPNSection::VpnSecOpenVPNSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN Security"), parent),
      m_vpnSetting(vpnSetting),
      m_cipherChooser(new ComboBoxWidget(this)),
      m_hmacChooser(new ComboBoxWidget(this))
{
    m_dataMap = vpnSetting->data();

    initUI();
    initConnection();
}

VpnSecOpenVPNSection::~VpnSecOpenVPNSection()
{
}

bool VpnSecOpenVPNSection::allInputValid()
{
    return true;
}

void VpnSecOpenVPNSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();

    if (m_currentCipher == "default") {
        m_dataMap.remove("cipher");
    } else {
        m_dataMap.insert("cipher", m_currentCipher);
    }

    if (m_currentHMAC == "default") {
        m_dataMap.remove("auth");
    } else {
        m_dataMap.insert("auth", m_currentHMAC);
    }

    m_vpnSetting->setData(m_dataMap);

    m_vpnSetting->setInitialized(true);
}

void VpnSecOpenVPNSection::initUI()
{
    m_cipherChooser->setTitle(tr("Cipher"));
    m_currentCipher = "default";
    for (auto cipher : CipherStrMap.values()) {
        if (cipher == m_dataMap.value("cipher")) {
            m_currentCipher = cipher;
        }
        m_cipherChooser->appendOption(CipherStrMap.key(cipher), cipher);
    }
    m_cipherChooser->setCurrent(m_currentCipher);

    m_hmacChooser->setTitle(tr("HMAC Auth"));
    m_currentHMAC = "default";
    for (auto hmac : HMACStrMap.values()) {
        if (hmac == m_dataMap.value("auth")) {
            m_currentHMAC = hmac;
        }
        m_hmacChooser->appendOption(HMACStrMap.key(hmac), hmac);
    }
    m_hmacChooser->setCurrent(m_currentHMAC);

    appendItem(m_cipherChooser);
    appendItem(m_hmacChooser);
}

void VpnSecOpenVPNSection::initConnection()
{
    connect(m_cipherChooser, &ComboBoxWidget::requestPage, this, &VpnSecOpenVPNSection::requestPage);
    connect(m_cipherChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentCipher = data.toString();
    });

    connect(m_hmacChooser, &ComboBoxWidget::requestPage, this, &VpnSecOpenVPNSection::requestPage);
    connect(m_hmacChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        m_currentHMAC = data.toString();
    });
}
