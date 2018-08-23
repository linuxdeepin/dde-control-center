#ifndef SECRETWIREDSECTION_H
#define SECRETWIREDSECTION_H

#include "secret8021xsection.h"
#include "switchwidget.h"

namespace dcc {
namespace widgets {

class SecretWiredSection : public Secret8021xSection
{
    Q_OBJECT

public:
    SecretWiredSection(NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent = 0);
    virtual ~SecretWiredSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    SwitchWidget *m_secretEnable;
    Secret8021xEnableWatcher *m_enableWatcher;
};

} /* widgets */
} /* dcc */

#endif /* SECRETWIREDSECTION_H */
