#ifndef CHAINSPROXYPAGE_H
#define CHAINSPROXYPAGE_H

#include "networkmodel.h"
#include "contentwidget.h"

#include <QObject>

namespace dcc {

namespace widgets {
class NextPageWidget;
class LineEditWidget;
}

namespace network {

class ChainsProxyPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit ChainsProxyPage(QWidget *parent = nullptr);
    void setModel(NetworkModel *model);

signals:
    void requestSet(const ProxyConfig &config) const;
    void requestShowTypePage() const;

private slots:
    void onCheckValue();

private:
    NetworkModel *m_model;
    dcc::widgets::NextPageWidget *m_proxyType;
    dcc::widgets::LineEditWidget *m_addr;
    dcc::widgets::LineEditWidget *m_port;
    dcc::widgets::LineEditWidget *m_username;
    dcc::widgets::LineEditWidget *m_password;

};
}
}

#endif // CHAINSPROXYPAGE_H
