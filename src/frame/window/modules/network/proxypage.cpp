/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "proxypage.h"
#include "widgets/buttontuple.h"
#include "widgets/translucentframe.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/lineeditwidget.h"
#include "widgets/settingsitem.h"
#include "widgets/switchwidget.h"
#include "widgets/comboxwidget.h"
#include "window/utils.h"
#include <networkmodel.h>

#include <DPalette>
#include <DApplicationHelper>

#include <QVBoxLayout>
#include <QDebug>
#include <QComboBox>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::widgets;
using namespace dde::network;

namespace DCC_NAMESPACE {
namespace network {
const QString MANUAL = "manual";
const QString AUTO = "auto";
const QStringList ProxyMethodList = { MANUAL, AUTO };

ProxyPage::ProxyPage(QWidget *parent)
    : QWidget(parent)
    , m_autoWidget(new TranslucentFrame)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
    , m_proxySwitch(new SwitchWidget)
    , m_proxyTypeBox(new ComboxWidget)
{
    m_buttonTuple->leftButton()->setText(tr("Cancel"));
    m_buttonTuple->rightButton()->setText(tr("Save"));
    m_buttonTuple->setVisible(false);
    m_buttonTuple->setEnabled(false);

    // 初始化代理开关、代理类型下拉框
    SettingsGroup *proxyTypeGroup = new SettingsGroup;
    m_proxySwitch->setTitle(tr("System Proxy"));
    m_proxyTypeBox->setTitle(tr("Proxy Type"));
    // 如果扩展，addItem添加顺序必须与ProxyMethodList顺序一致
    m_proxyTypeBox->comboBox()->addItem(tr("Manual"));
    m_proxyTypeBox->comboBox()->addItem(tr("Auto"));
    m_proxyTypeBox->setVisible(false);
    proxyTypeGroup->appendItem(m_proxySwitch);
    proxyTypeGroup->appendItem(m_proxyTypeBox);

    // 手动代理编辑界面处理逻辑提取
    auto initProxyGroup = [this](LineEditWidget *&proxyEdit, LineEditWidget *&portEdit, const QString &proxyTitle, SettingsGroup *&group){
        proxyEdit = new LineEditWidget;
        proxyEdit->setPlaceholderText(tr("Optional"));
        proxyEdit->setTitle(proxyTitle);
        proxyEdit->textEdit()->installEventFilter(this);
        portEdit = new LineEditWidget;
        portEdit->setPlaceholderText(tr("Optional"));
        portEdit->setTitle(tr("Port"));
        portEdit->textEdit()->installEventFilter(this);
        group->appendItem(proxyEdit);
        group->appendItem(portEdit);
        connect(portEdit->textEdit(), &QLineEdit::textChanged, this, [ = ](const QString &str){
            if (str.toInt() < 0) {
                portEdit->setText("0");
            } else if(str.toInt() > 65535) {
                portEdit->setText("65535");
            }
        });
    };

    //  手动代理编辑界面控件初始化
    SettingsGroup *httpGroup = new SettingsGroup;
    initProxyGroup(m_httpAddr, m_httpPort, tr("HTTP Proxy"), httpGroup);

    SettingsGroup *httpsGroup = new SettingsGroup;
    initProxyGroup(m_httpsAddr, m_httpsPort, tr("HTTPS Proxy"), httpsGroup);

    SettingsGroup *ftpGroup = new SettingsGroup;
    initProxyGroup(m_ftpAddr, m_ftpPort, tr("FTP Proxy"), ftpGroup);

    SettingsGroup *socksGroup = new SettingsGroup;
    initProxyGroup(m_socksAddr, m_socksPort, tr("SOCKS Proxy"), socksGroup);

    // 手动代理界面忽略主机编辑框初始化
    m_ignoreList = new DTextEdit;
    m_ignoreList->installEventFilter(this);
    QLabel *ignoreTips = new QLabel;
    ignoreTips->setWordWrap(true);
    ignoreTips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ignoreTips->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ignoreTips->setText(tr("Ignore the proxy configurations for the above hosts and domains"));

    // 自动代理界面控件初始化
    SettingsGroup *autoGroup = new SettingsGroup;
    m_autoUrl = new LineEditWidget;
    m_autoUrl->setPlaceholderText(tr("Optional"));
    m_autoUrl->setTitle(tr("Configuration URL"));
    m_autoUrl->textEdit()->installEventFilter(this);
    autoGroup->appendItem(m_autoUrl);

    // 手动代理界面布局
    QVBoxLayout *manualLayout = new QVBoxLayout;
    manualLayout->addWidget(httpGroup);
    manualLayout->addWidget(httpsGroup);
    manualLayout->addWidget(ftpGroup);
    manualLayout->addWidget(socksGroup);
    manualLayout->addWidget(m_ignoreList);
    manualLayout->addWidget(ignoreTips);
    manualLayout->setMargin(0);
    manualLayout->setSpacing(10);
    m_manualWidget = new TranslucentFrame;
    m_manualWidget->setLayout(manualLayout);
    m_manualWidget->setVisible(false);

    // 自动代理界面布局
    QVBoxLayout *autoLayout = new QVBoxLayout;
    autoLayout->setContentsMargins(ThirdPageContentsMargins);
    autoLayout->addWidget(autoGroup);
    autoLayout->setSpacing(10);
    autoLayout->setMargin(0);
    m_autoWidget->setLayout(autoLayout);
    m_autoWidget->setVisible(false);

    // 主窗口布局组合
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(proxyTypeGroup);
    mainLayout->addWidget(m_manualWidget);
    mainLayout->addWidget(m_autoWidget);
    mainLayout->addSpacing(10);
    mainLayout->addStretch();

    // 加入button布局
    ContentWidget *conentwidget = new ContentWidget;
    setWindowTitle(tr("System Proxy"));
    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(mainLayout);
    conentwidget->setContent(w);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setMargin(0);
    vLayout->setContentsMargins(ThirdPageContentsMargins);
    QVBoxLayout *btnLayout = new QVBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->addWidget(m_buttonTuple);
    vLayout->addWidget(conentwidget);
    vLayout->addLayout(btnLayout);
    setLayout(vLayout);

    // 响应系统代理开关
    connect(m_proxySwitch, &SwitchWidget::checkedChanged, m_proxyTypeBox, [=](const bool checked){
        m_buttonTuple->setEnabled(checked);
        if (checked) {
            // 打开代理默认手动
            onProxyMethodChanged(MANUAL);
        } else {
            m_proxyTypeBox->setVisible(false);
            m_manualWidget->setVisible(false);
            m_autoWidget->setVisible(false);
            m_buttonTuple->setVisible(false);
            m_buttonTuple->rightButton()->clicked();
        }
    });

    // 处理协议类型下拉框切换
    connect(m_proxyTypeBox->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
        m_buttonTuple->setEnabled(true);
        int manualId = ProxyMethodList.indexOf(MANUAL);
        int autoId = ProxyMethodList.indexOf(AUTO);
        m_manualWidget->setVisible(index == manualId);
        m_autoWidget->setVisible(index == autoId);
        m_buttonTuple->setVisible(index == manualId || index == autoId);
    });

    // 取消、确定按钮响应
    connect(m_buttonTuple->rightButton(), &QPushButton::clicked, this, &ProxyPage::applySettings);
    connect(m_buttonTuple->leftButton(), &QPushButton::clicked, this, [this] {
        m_buttonTuple->setEnabled(false);
        if (m_proxyTypeBox->comboBox()->currentIndex() == ProxyMethodList.indexOf(MANUAL)) {
            onProxyChanged("http", m_model->proxy("http"));
            onProxyChanged("https", m_model->proxy("https"));
            onProxyChanged("ftp", m_model->proxy("ftp"));
            onProxyChanged("socks", m_model->proxy("socks"));
            onIgnoreHostsChanged(m_model->ignoreHosts());
        } else {
            m_autoUrl->setText(m_model->autoProxy());
        }
    });

    QTimer::singleShot(1, this, [ = ] {
        Q_EMIT requestQueryProxyData();
    });
}

void ProxyPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(model, &NetworkModel::proxyMethodChanged, this, &ProxyPage::onProxyMethodChanged);
    connect(model, &NetworkModel::proxyIgnoreHostsChanged, this, &ProxyPage::onIgnoreHostsChanged);
    connect(model, &NetworkModel::proxyChanged, this, &ProxyPage::onProxyChanged);
    connect(model, &NetworkModel::autoProxyChanged, m_autoUrl, &LineEditWidget::setText);

    onProxyChanged("http", model->proxy("http"));
    onProxyChanged("https", model->proxy("https"));
    onProxyChanged("ftp", model->proxy("ftp"));
    onProxyChanged("socks", model->proxy("socks"));
    onProxyMethodChanged(model->proxyMethod());
    onIgnoreHostsChanged(model->ignoreHosts());
    m_autoUrl->setText(model->autoProxy());
}

void ProxyPage::onProxyMethodChanged(const QString &proxyMethod)
{
    m_proxyTypeBox->comboBox()->blockSignals(true);
    if (proxyMethod == "none") {
        m_proxySwitch->setChecked(false);
        m_manualWidget->setVisible(false);
        m_autoWidget->setVisible(false);
        m_buttonTuple->setVisible(false);
        m_proxyTypeBox->setVisible(false);
    } else if (proxyMethod == MANUAL) {
        m_proxySwitch->setChecked(true);
        m_manualWidget->setVisible(true);
        m_autoWidget->setVisible(false);
        m_buttonTuple->setVisible(true);
        m_proxyTypeBox->comboBox()->setCurrentIndex(ProxyMethodList.indexOf(MANUAL));
        m_proxyTypeBox->setVisible(true);
    } else if (proxyMethod == AUTO) {
        m_proxySwitch->setChecked(true);
        m_manualWidget->setVisible(false);
        m_autoWidget->setVisible(true);
        m_buttonTuple->setVisible(true);
        m_proxyTypeBox->comboBox()->setCurrentIndex(ProxyMethodList.indexOf(AUTO));
        m_proxyTypeBox->setVisible(true);
    } else {
        qDebug() << "error proxyMethod:" << proxyMethod;
    }
    m_proxyTypeBox->comboBox()->blockSignals(false);
}

void ProxyPage::applySettings()
{
    this->setFocus();
    m_buttonTuple->setEnabled(false);
    if (!m_proxySwitch->checked()) {
        Q_EMIT requestSetProxyMethod("none");
    } else if (m_proxyTypeBox->comboBox()->currentIndex() == ProxyMethodList.indexOf(MANUAL)) {
        Q_EMIT requestSetProxy("http", m_httpAddr->text(), m_httpPort->text());
        Q_EMIT requestSetProxy("https", m_httpsAddr->text(), m_httpsPort->text());
        Q_EMIT requestSetProxy("ftp", m_ftpAddr->text(), m_ftpPort->text());
        Q_EMIT requestSetProxy("socks", m_socksAddr->text(), m_socksPort->text());
        Q_EMIT requestSetIgnoreHosts(m_ignoreList->toPlainText());
        Q_EMIT requestSetProxyMethod(MANUAL);
    } else if (m_proxyTypeBox->comboBox()->currentIndex() == ProxyMethodList.indexOf(AUTO)) {
        Q_EMIT requestSetAutoProxy(m_autoUrl->text());
        Q_EMIT requestSetProxyMethod(AUTO);
    }
}

void ProxyPage::onIgnoreHostsChanged(const QString &hosts)
{
    const QTextCursor cursor = m_ignoreList->textCursor();

    m_ignoreList->blockSignals(true);
    m_ignoreList->setPlainText(hosts);
    m_ignoreList->setTextCursor(cursor);
    m_ignoreList->blockSignals(false);
}

void ProxyPage::onProxyChanged(const QString &type, const ProxyConfig &config)
{
    if (type == "http") {
        m_httpAddr->setText(config.url);
        m_httpPort->setText(QString::number(config.port));
    } else if (type == "https") {
        m_httpsAddr->setText(config.url);
        m_httpsPort->setText(QString::number(config.port));
    } else if (type == "ftp") {
        m_ftpAddr->setText(config.url);
        m_ftpPort->setText(QString::number(config.port));
    } else if (type == "socks") {
        m_socksAddr->setText(config.url);
        m_socksPort->setText(QString::number(config.port));
    }
}

bool ProxyPage::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，捕捉FocusIn消息，DTextEdit没有鼠标点击消息
    if (event->type() == QEvent::FocusIn) {
        if ((dynamic_cast<QLineEdit*>(watched) || dynamic_cast<DTextEdit*>(watched))) {
            m_buttonTuple->setEnabled(true);
        }
    }
    return QWidget::eventFilter(watched, event);
}
}
}
