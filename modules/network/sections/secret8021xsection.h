#ifndef SECRET8021xSECTION_H
#define SECRET8021xSECTION_H

#include "abstractsection.h"
#include "comboboxwidget.h"
#include "lineeditwidget.h"
#include "passwdeditwidget.h"

#include <networkmanagerqt/security8021xsetting.h>

namespace dcc {
namespace widgets {

class Secret8021xEnableWatcher : public QObject
{
    Q_OBJECT

public:
    Secret8021xEnableWatcher (QObject *parent = 0) : QObject(parent) {};
    virtual ~Secret8021xEnableWatcher () {};

    inline bool secretEnabled() {return m_secretEnabled;}

public Q_SLOTS:
    inline void setSecretEnable(const bool enabled) {
        if (m_secretEnabled != enabled) {
            m_secretEnabled = enabled;
            Q_EMIT secretEnableChanged(m_secretEnabled);
        }
    }

Q_SIGNALS:
    void secretEnableChanged(bool enabled);

private:
    bool m_secretEnabled;
};

class Secret8021xSection : public AbstractSection
{
    Q_OBJECT

public:
    virtual ~Secret8021xSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

protected:
    explicit Secret8021xSection(NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent = 0);
    // This method must be called by subclass after initialization
    void init(Secret8021xEnableWatcher *watcher, QList<NetworkManager::Security8021xSetting::EapMethod> eapMethodsSupportList);

private:
    void initUI();
    void initConnection();
    void initEapItems(NetworkManager::Security8021xSetting::EapMethod method);
    void initEapMethodTlsItems(QList<SettingsItem *> *itemList);
    void initEapMethodFastItems(QList<SettingsItem *> *itemList);
    void initEapMethodTtlsItems(QList<SettingsItem *> *itemList);
    void initEapMethodPeapItems(QList<SettingsItem *> *itemList);

    void onSecretEnableChanged(const bool enable);
    void onEapMethodChanged(NetworkManager::Security8021xSetting::EapMethod method);
    void onPasswordFlagsChanged(NetworkManager::Setting::SecretFlagType type);
    void saveUserInputIdentify();
    void saveUserInputPassword();

    bool commonItemsInpuValid();
    bool tlsItemsInputValid();
    bool fastItemsInputValid();
    bool ttlsItemsInputValid();
    bool peapItemsInputValid();

    void saveCommonItems();
    void saveTlsItems();
    void saveFastItems();
    void saveTtlsItems();
    void savePeapItems();

private:
    ComboBoxWidget *m_eapMethmodChooser;
    ComboBoxWidget *m_passwordFlagsChooser;
    LineEditWidget *m_identity;
    PasswdEditWidget *m_password;
    Secret8021xEnableWatcher *m_enableWatcher;

    NetworkManager::Security8021xSetting::EapMethod m_currentEapMethod;
    NetworkManager::Setting::SecretFlagType m_currentPasswordType;
    NetworkManager::Security8021xSetting::Ptr m_secretSetting;

    QList<NetworkManager::Security8021xSetting::EapMethod> m_eapMethodsWantedList;
    QMap<NetworkManager::Security8021xSetting::EapMethod, QList<SettingsItem *>> m_eapMethodItemsMap;
    QMap<NetworkManager::Security8021xSetting::EapMethod, QString> m_userInputIdentifyMap;
    QMap<NetworkManager::Security8021xSetting::EapMethod, QString> m_userInputPasswordMap;
};

} /* widgets */
} /* dcc */

Q_DECLARE_METATYPE(NetworkManager::Security8021xSetting::EapMethod);
Q_DECLARE_METATYPE(NetworkManager::Setting::SecretFlagType);

#endif /* SECRET8021xSECTION_H */
