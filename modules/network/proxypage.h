#ifndef PROXYPAGE_H
#define PROXYPAGE_H

#include "contentwidget.h"

#include <dsegmentedcontrol.h>

namespace dcc {

namespace widgets {

class LineEditWidget;
class PlainTextItem;

}

namespace network {

struct ProxyConfig;
class NetworkModel;
class ProxyPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ProxyPage(QWidget *parent = 0);

    void setModel(NetworkModel *model);

signals:
    void requestQueryProxyData() const;
    void requestSetProxy(const QString &type, const QString &addr, const QString &port) const;
    void requestSetProxyMethod(const QString &pm) const;
    void requestSetAutoProxy(const QString &proxy) const;
    void requestSetIgnoreHosts(const QString &list) const;

private slots:
    void onProxyMethodChanged(const QString &proxyMethod);
    void onIgnoreHostsChanged(const QString &hosts);
    void onProxyChanged(const QString &type, const ProxyConfig &config);

private:
    void applyProxy(const QString &type);

private:
    NetworkModel *m_model;

    QWidget *m_manualWidget;
    QWidget *m_autoWidget;

    widgets::LineEditWidget *m_httpAddr;
    widgets::LineEditWidget *m_httpPort;
    widgets::LineEditWidget *m_httpsAddr;
    widgets::LineEditWidget *m_httpsPort;
    widgets::LineEditWidget *m_ftpAddr;
    widgets::LineEditWidget *m_ftpPort;
    widgets::LineEditWidget *m_socksAddr;
    widgets::LineEditWidget *m_socksPort;
    widgets::PlainTextItem *m_ignoreList;

    widgets::LineEditWidget *m_autoUrl;

    Dtk::Widget::DSegmentedControl *m_proxyType;
};

}

}

#endif // PROXYPAGE_H
