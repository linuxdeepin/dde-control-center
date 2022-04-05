/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "chainsproxypage.h"
#include "window/utils.h"

#include "networkcontroller.h"
#include "proxycontroller.h"

#include <DTipLabel>

#include <QHBoxLayout>
#include <QRegularExpression>
#include <QComboBox>
#include <QHostAddress>

#include "widgets/buttontuple.h"
#include "widgets/comboxwidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dde::network;

static int indexFromProxyType(const AppProxyType &type)
{
    if (type == AppProxyType::Http)
        return 0;

    if (type == AppProxyType::Socks4)
        return 1;

    return 2;
}

static AppProxyType proxyTypeFromIndex(int index)
{
    if (index == 0)
        return AppProxyType::Http;

    if (index == 1)
        return AppProxyType::Socks4;

    return AppProxyType::Socks5;
}

ChainsProxyPage::ChainsProxyPage(QWidget *parent)
    : ContentWidget(parent)
{
    setTitle(tr("Application Proxy"));

    TranslucentFrame *frameContent = new TranslucentFrame(this);
    SettingsGroup *grp = new SettingsGroup(frameContent);

    m_proxyType = new ComboxWidget(grp);
    m_proxyType->setTitle(tr("Proxy Type"));

    m_comboBox = m_proxyType->comboBox();
    m_comboBox->addItem("http");
    m_comboBox->addItem("socks4");
    m_comboBox->addItem("socks5");

    m_addr = new LineEditWidget(grp);
    m_addr->setTitle(tr("IP Address"));
    m_addr->setPlaceholderText(tr("Required"));
    m_addr->textEdit()->installEventFilter(this);

    m_port = new LineEditWidget(grp);
    m_port->setTitle(tr("Port"));
    m_port->setPlaceholderText(tr("Required"));
    m_port->textEdit()->installEventFilter(this);

    m_username = new LineEditWidget(grp);
    m_username->setTitle(tr("Username"));
    m_username->setPlaceholderText(tr("Optional"));
    m_username->textEdit()->installEventFilter(this);

    m_password = new LineEditWidget(grp);
    m_password->setTitle(tr("Password"));
    m_password->setPlaceholderText(tr("Optional"));
    m_password->textEdit()->setEchoMode(QLineEdit::Password);
    m_password->textEdit()->installEventFilter(this);
    m_password->textEdit()->setValidator(new QRegExpValidator(QRegExp("^\\S+$"), this));

    grp->appendItem(m_proxyType);
    grp->appendItem(m_addr);
    grp->appendItem(m_port);
    grp->appendItem(m_username);
    grp->appendItem(m_password);

    QVBoxLayout *layout = new QVBoxLayout(frameContent);
    layout->setMargin(0);
    layout->setContentsMargins(QMargins(10, 0, 10, 0));       // 设置列表项与背景左右间距分别为10
    layout->setSpacing(10);

    QHBoxLayout *topLayout = new QHBoxLayout(frameContent);
    topLayout->addSpacing(20);
    layout->addLayout(topLayout);
    layout->addWidget(grp);

    DTipLabel *tip = new DTipLabel(tr("Check \"Use a proxy\" in application context menu in Launcher after configured"), frameContent);
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignLeft);
    tip->setMargin(8);          // 设置tips与列表中文字左对齐
    layout->addWidget(tip);

    m_btns = new ButtonTuple(ButtonTuple::Save);
    m_btns->leftButton()->setText(tr("Cancel", "button"));
    m_btns->rightButton()->setText(tr("Save", "button"));
    m_btns->leftButton()->setEnabled(false);
    m_btns->rightButton()->setEnabled(false);

    layout->addStretch();

    layout->addWidget(m_btns, 0, Qt::AlignBottom);

    frameContent->setLayout(layout);

    setContentsMargins(QMargins(0, 10, 0, 10));    // 设置圆角上下间距分别为10
    setContent(frameContent);

    connect(m_btns->leftButton(), &QPushButton::clicked, this, &ChainsProxyPage::onRestoreValue);
    connect(m_btns->rightButton(), &QPushButton::clicked, this, &ChainsProxyPage::onCheckValue);

    onRestoreValue();
    initUiData();

    connect(m_proxyType, &ComboxWidget::onIndexChanged, this, [ = ] {
        m_btns->leftButton()->setEnabled(true);
        m_btns->rightButton()->setEnabled(true);
    });
}

ChainsProxyPage::~ChainsProxyPage()
{
    delete m_proxyType;
}

void ChainsProxyPage::initUiData()
{
    ProxyController *proxyController = NetworkController::instance()->proxyController();

    connect(proxyController, &ProxyController::appUsernameChanged, this, [ = ] (const QString &username) {
        m_username->textEdit()->blockSignals(true);
        m_username->setText(username);
        m_username->textEdit()->blockSignals(false);
    });
    connect(proxyController, &ProxyController::appPasswordChanged, this, [ = ] (const QString &password) {
        m_password->textEdit()->blockSignals(true);
        m_password->setText(password);
        m_password->textEdit()->blockSignals(false);
    });

    connect(proxyController, &ProxyController::appIPChanged, this, [ = ] (const QString &ip) {
        m_addr->textEdit()->blockSignals(true);
        m_addr->setText(ip);
        m_addr->textEdit()->blockSignals(false);
    });

    connect(proxyController, &ProxyController::appTypeChanged, this, [ = ] (const AppProxyType &type) {
        m_proxyType->comboBox()->blockSignals(true);
        m_proxyType->setCurrentIndex(indexFromProxyType(type));
        m_proxyType->comboBox()->blockSignals(false);
    });

    connect(proxyController, &ProxyController::appPortChanged, this, [ = ] (const uint &port) {
        m_port->textEdit()->blockSignals(true);
        m_port->setText(QString::number(port));
        m_port->textEdit()->blockSignals(false);
    });
}

void ChainsProxyPage::onRestoreValue()
{
    ProxyController *proxyController = NetworkController::instance()->proxyController();
    AppProxyConfig config = proxyController->appProxy();

    m_proxyType->blockSignals(true);
    m_proxyType->setCurrentIndex(indexFromProxyType(config.type));
    m_proxyType->blockSignals(false);

    m_addr->textEdit()->blockSignals(true);
    m_addr->textEdit()->setText(config.ip);
    m_addr->textEdit()->blockSignals(false);

    m_port->textEdit()->blockSignals(true);
    m_port->setText(QString::number(config.port));
    m_port->textEdit()->blockSignals(false);

    m_username->textEdit()->blockSignals(true);
    m_username->setText(config.username);
    m_username->textEdit()->blockSignals(false);

    m_password->textEdit()->blockSignals(true);
    m_password->setText(config.password);
    m_password->textEdit()->blockSignals(false);

    m_btns->leftButton()->setEnabled(false);
    m_btns->rightButton()->setEnabled(false);
}

void ChainsProxyPage::onCheckValue()
{
    m_addr->setIsErr(false);
    m_port->setIsErr(false);
    m_username->setIsErr(false);
    m_password->setIsErr(false);

    ProxyController *proxyController = NetworkController::instance()->proxyController();

    // 如果地址和端口为0，删除配置文件
    if (m_addr->text().isEmpty() && m_port->text().toInt() == 0) {
        AppProxyConfig config;
        config.port = 0;
        config.ip.clear();

        m_btns->leftButton()->setEnabled(false);
        m_btns->rightButton()->setEnabled(false);

        proxyController->setAppProxy(config);
        Q_EMIT back();
        return;
    }

    const QString &addr = m_addr->text();
    if (addr.isEmpty() || !isIPV4(addr)) {
        m_addr->setIsErr(true);
        m_addr->dTextEdit()->showAlertMessage(tr("Invalid IP address"), m_addr, 2000);
        return;
    }

    bool ok = true;
    const uint port = m_port->text().toUInt(&ok);
    if (!ok || port > 65535) {
        m_port->setIsErr(true);
        m_port->dTextEdit()->showAlertMessage(tr("Invalid port"), m_port, 2000);
        return;
    }

    const QString &username = m_username->text();
    const QString &password = m_password->text();

    AppProxyConfig config;
    config.type = proxyTypeFromIndex(m_proxyType->comboBox()->currentIndex());
    config.ip = addr;
    config.port = port;
    config.username = username;
    config.password = password;

    m_btns->leftButton()->setEnabled(false);
    m_btns->rightButton()->setEnabled(false);

    proxyController->setAppProxy(config);
    Q_EMIT back();
}

bool ChainsProxyPage::isIPV4(const QString &ipv4)
{
    QHostAddress ipAddr(ipv4);
    if (ipAddr == QHostAddress(QHostAddress::Null) || ipAddr == QHostAddress(QHostAddress::AnyIPv4)
            || ipAddr.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv4Protocol) {
        return false;
    }

    QRegExp regExpIP("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
    return regExpIP.exactMatch(ipv4);
}

bool ChainsProxyPage::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(watched)) {
            m_btns->leftButton()->setEnabled(true);
            m_btns->rightButton()->setEnabled(true);
        }
    }

    return QWidget::eventFilter(watched, event);
}
