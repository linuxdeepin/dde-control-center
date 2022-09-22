// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CHAINSPROXYPAGE_H
#define CHAINSPROXYPAGE_H

#include <QObject>

#include <widgets/contentwidget.h>

namespace dde {
  namespace network {
    class NetworkController;
    struct AppProxyConfig;
  }
}

namespace dcc {
  namespace widgets {
    class ComboxWidget;
    class LineEditWidget;
    class ButtonTuple;
  }
}

using namespace dde::network;
using namespace dcc::widgets;

class QComboBox;

class PasswdLineEditWidget;

class ChainsProxyPage : public dcc::ContentWidget
{
    Q_OBJECT

public:
    explicit ChainsProxyPage(QWidget *parent = nullptr);
    ~ChainsProxyPage() Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestShowTypePage() const;

private:
    void initUiData();

private Q_SLOTS:
    void onCheckValue();
    void onRestoreValue();
    bool isIPV4(const QString &ipv4);

private:
    ComboxWidget *m_proxyType;
    LineEditWidget *m_addr;
    LineEditWidget *m_port;
    LineEditWidget *m_username;
    PasswdLineEditWidget *m_password;
    QComboBox *m_comboBox;
    ButtonTuple *m_btns;
};

#endif // CHAINSPROXYPAGE_H
