#include "proxypage.h"
#include "networkmodel.h"
#include "translucentframe.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "lineeditwidget.h"
#include "plantextitem.h"

#include <QVBoxLayout>
#include <QDebug>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {

namespace network {

const QStringList ProxyMethodList = { "none", "manual", "auto" };

ProxyPage::ProxyPage(QWidget *parent)
    : ContentWidget(parent),

      m_manualWidget(new TranslucentFrame),
      m_autoWidget(new TranslucentFrame),
      m_buttonTuple(new ButtonTuple),

      m_proxyType(new DSegmentedControl)
{
    m_buttonTuple->leftButton()->setText(tr("Cancel"));
    m_buttonTuple->rightButton()->setText(tr("Confirm"));

    m_proxyType->addSegmented(tr("None"));
    m_proxyType->addSegmented(tr("Manual"));
    m_proxyType->addSegmented(tr("Auto"));
    m_proxyType->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_proxyType->setFixedHeight(30);
    m_proxyType->setStyleSheet("Dtk--Widget--DSegmentedControl {"
                               "background: transparent;"
                               "border: 1px solid #28a9f7;"
                               "border-radius: 3px;"
                               "}"
                               ""
                               "Dtk--Widget--DSegmentedControl  Dtk--Widget--DSegmentedHighlight#Highlight {"
                               "background-color: #28a9f7;"
                               "border: none;"
                               "padding: 5px 3px;"
                               "margin: 1px 0 0 0;"
                               "}"
                               ""
                               "Dtk--Widget--DSegmentedControl QToolButton {"
                               "background: none;"
                               "border: none;"
                               "padding: 5px 3px;"
                               "}"
                               ""
                               "Dtk--Widget--DSegmentedControl QToolButton#Segmented:enabled {"
                               "color: white;"
                               "}"
                               ""
                               "Dtk--Widget--DSegmentedControl QToolButton#Segmented {"
                               "}");

    m_httpAddr = new LineEditWidget;
    m_httpAddr->setTitle(tr("Http Proxy"));
    m_httpPort = new LineEditWidget;
    m_httpPort->setTitle(tr("Port"));

    m_httpsAddr = new LineEditWidget;
    m_httpsAddr->setTitle(tr("Https Proxy"));
    m_httpsPort = new LineEditWidget;
    m_httpsPort->setTitle(tr("Port"));

    m_ftpAddr = new LineEditWidget;
    m_ftpAddr->setTitle(tr("Ftp Proxy"));
    m_ftpPort = new LineEditWidget;
    m_ftpPort->setTitle(tr("Port"));

    m_socksAddr = new LineEditWidget;
    m_socksAddr->setTitle(tr("Socks Proxy"));
    m_socksPort = new LineEditWidget;
    m_socksPort->setTitle(tr("Port"));

    m_ignoreList = new PlainTextItem;
    QLabel *ignoreTips = new QLabel;
    ignoreTips->setWordWrap(true);
    ignoreTips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ignoreTips->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ignoreTips->setText(tr("Ignore the proxy configurations for the above hosts and domains"));

    m_autoUrl = new LineEditWidget;
    m_autoUrl->setTitle(tr("Configuration URL"));

    SettingsGroup *httpGroup = new SettingsGroup;
    httpGroup->appendItem(m_httpAddr);
    httpGroup->appendItem(m_httpPort);

    SettingsGroup *httpsGroup = new SettingsGroup;
    httpsGroup->appendItem(m_httpsAddr);
    httpsGroup->appendItem(m_httpsPort);

    SettingsGroup *ftpGroup = new SettingsGroup;
    ftpGroup->appendItem(m_ftpAddr);
    ftpGroup->appendItem(m_ftpPort);

    SettingsGroup *socksGroup = new SettingsGroup;
    socksGroup->appendItem(m_socksAddr);
    socksGroup->appendItem(m_socksPort);

    SettingsGroup *autoGroup = new SettingsGroup;
    autoGroup->appendItem(m_autoUrl);

    SettingsGroup *ignoreGroup = new SettingsGroup;
    ignoreGroup->appendItem(m_ignoreList);

    QVBoxLayout *manualLayout = new QVBoxLayout;
    manualLayout->addWidget(httpGroup);
    manualLayout->addWidget(httpsGroup);
    manualLayout->addWidget(ftpGroup);
    manualLayout->addWidget(socksGroup);
    manualLayout->addWidget(ignoreGroup);
    manualLayout->addWidget(ignoreTips);
    manualLayout->setMargin(0);
    manualLayout->setSpacing(10);

    QVBoxLayout *autoLayout = new QVBoxLayout;
    autoLayout->addWidget(autoGroup);
    autoLayout->setSpacing(10);
    autoLayout->setMargin(0);

    m_manualWidget->setLayout(manualLayout);
    m_autoWidget->setLayout(autoLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_proxyType);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_manualWidget);
    mainLayout->addWidget(m_autoWidget);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_buttonTuple);
    mainLayout->addStretch();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setTitle(tr("System Proxy"));
    setContent(mainWidget);

    connect(m_buttonTuple->leftButton(), &QPushButton::clicked, this, &ProxyPage::back);
    connect(m_buttonTuple->rightButton(), &QPushButton::clicked, this, &ProxyPage::back, Qt::QueuedConnection);
    connect(m_buttonTuple->rightButton(), &QPushButton::clicked, this, &ProxyPage::applySettings);
    connect(m_proxyType, &DSegmentedControl::currentChanged, this, &ProxyPage::onProxyToggled);
//    connect(m_proxyType, &DSegmentedControl::currentChanged, [=](const int index) { emit requestSetProxyMethod(ProxyMethodList[index]); });
//    connect(m_ignoreList->plainEdit(), &QPlainTextEdit::textChanged, [=] { emit requestSetIgnoreHosts(m_ignoreList->plainEdit()->toPlainText()); });
//    connect(m_httpAddr->textEdit(), &QLineEdit::editingFinished, [=] { applyProxy("http"); });
//    connect(m_httpPort->textEdit(), &QLineEdit::editingFinished, [=] { applyProxy("http"); });
//    connect(m_httpsAddr->textEdit(), &QLineEdit::editingFinished, [=] { applyProxy("https"); });
//    connect(m_httpsPort->textEdit(), &QLineEdit::editingFinished, [=] { applyProxy("https"); });
//    connect(m_ftpAddr->textEdit(), &QLineEdit::editingFinished, [=] { applyProxy("ftp"); });
//    connect(m_ftpPort->textEdit(), &QLineEdit::editingFinished, [=] { applyProxy("ftp"); });
//    connect(m_socksAddr->textEdit(), &QLineEdit::editingFinished, [=] { applyProxy("socks"); });
//    connect(m_socksPort->textEdit(), &QLineEdit::editingFinished, [=] { applyProxy("socks"); });
//    connect(m_autoUrl->textEdit(), &QLineEdit::editingFinished, [=] { emit requestSetAutoProxy(m_autoUrl->text()); });

    QTimer::singleShot(1, this, [=] { emit requestQueryProxyData(); });
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
    const int index = ProxyMethodList.indexOf(proxyMethod);
    if (index == -1)
        return;

    m_proxyType->setCurrentIndex(index);
    onProxyToggled(index);
}

void ProxyPage::onProxyToggled(const int index)
{
    // refersh ui
    m_manualWidget->setVisible(index == 1);
    m_autoWidget->setVisible(index == 2);
}

void ProxyPage::applySettings() const
{
    emit requestSetProxy("http", m_httpAddr->text(), m_httpPort->text());
    emit requestSetProxy("https", m_httpsAddr->text(), m_httpsPort->text());
    emit requestSetProxy("ftp", m_ftpAddr->text(), m_ftpPort->text());
    emit requestSetProxy("socks", m_socksAddr->text(), m_socksPort->text());

    emit requestSetIgnoreHosts(m_ignoreList->plainEdit()->toPlainText());

    emit requestSetAutoProxy(m_autoUrl->text());

    emit requestSetProxyMethod(ProxyMethodList[m_proxyType->currentIndex()]);
}

void ProxyPage::onIgnoreHostsChanged(const QString &hosts)
{
    const QTextCursor cursor = m_ignoreList->plainEdit()->textCursor();

    m_ignoreList->plainEdit()->blockSignals(true);
    m_ignoreList->plainEdit()->setPlainText(hosts);
    m_ignoreList->plainEdit()->setTextCursor(cursor);
    m_ignoreList->plainEdit()->blockSignals(false);
}

void ProxyPage::applyProxy(const QString &type)
{
    if (type == "http")
        emit requestSetProxy("http", m_httpAddr->text(), m_httpPort->text());
    else if (type == "https")
        emit requestSetProxy("https", m_httpsAddr->text(), m_httpsPort->text());
    else if (type == "ftp")
        emit requestSetProxy("ftp", m_ftpAddr->text(), m_ftpPort->text());
    else if (type == "socks")
        emit requestSetProxy("socks", m_socksAddr->text(), m_socksPort->text());
}

void ProxyPage::onProxyChanged(const QString &type, const ProxyConfig &config)
{
    if (type == "http")
    {
        m_httpAddr->setText(config.url);
        m_httpPort->setText(config.port);
    } else if (type == "https") {
        m_httpsAddr->setText(config.url);
        m_httpsPort->setText(config.port);
    } else if (type == "ftp") {
        m_ftpAddr->setText(config.url);
        m_ftpPort->setText(config.port);
    } else if (type == "socks") {
        m_socksAddr->setText(config.url);
        m_socksPort->setText(config.port);
    }
}

}

}
