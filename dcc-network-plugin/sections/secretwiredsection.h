// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SECRETWIREDSECTION_H
#define SECRETWIREDSECTION_H

#include "secret8021xsection.h"

namespace dcc {
  namespace widgets {
    class SwitchWidget;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class SecretWiredSection : public Secret8021xSection
{
    Q_OBJECT

public:
    SecretWiredSection(Security8021xSetting::Ptr sSetting, QFrame *parent = nullptr);
    virtual ~SecretWiredSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    SwitchWidget *m_secretEnable;
    Secret8021xEnableWatcher *m_enableWatcher;
};

#endif /* SECRETWIREDSECTION_H */
