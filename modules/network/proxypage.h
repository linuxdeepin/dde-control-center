#ifndef PROXYPAGE_H
#define PROXYPAGE_H

#include "contentwidget.h"

#include <dsegmentedcontrol.h>

namespace dcc {

namespace widgets {

class LineEditWidget;

}

namespace network {

class NetworkModel;
class ProxyPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ProxyPage(QWidget *parent = 0);

    void setModel(NetworkModel *model);

signals:
    void requestQueryProxyMethod() const;
    void requestSetProxyMethod(const QString &pm) const;

private slots:
    void onProxyMethodChanged(const QString &proxyMethod);

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

    widgets::LineEditWidget *m_autoUrl;

    Dtk::Widget::DSegmentedControl *m_proxyType;
};

}

}

#endif // PROXYPAGE_H
