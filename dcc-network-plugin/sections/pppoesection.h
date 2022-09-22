// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PPPOESECTION_H
#define PPPOESECTION_H

#include "abstractsection.h"

#include <networkmanagerqt/pppoesetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
  }
}
class PasswdLineEditWidget;

using namespace NetworkManager;
using namespace dcc::widgets;

class PPPOESection : public AbstractSection
{
    Q_OBJECT

public:
    explicit PPPOESection(PppoeSetting::Ptr pppoeSetting, QFrame *parent = Q_NULLPTR);
    virtual ~PPPOESection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    QString getDeviceInterface();
    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    PppoeSetting::Ptr m_pppoeSetting;

    LineEditWidget *m_userName;
    LineEditWidget *m_service;
    PasswdLineEditWidget *m_password;
};

#endif /* PPPOESECTION_H */
