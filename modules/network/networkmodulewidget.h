#ifndef NETWORKMODULEWIDGET_H
#define NETWORKMODULEWIDGET_H

#include "modulewidget.h"

namespace dcc {
    class NextPageWidget;
}

class NetworkModuleWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit NetworkModuleWidget();

signals:
    void requestShowDetailPage() const;

private:
    dcc::NextPageWidget *m_pppBtn;
    dcc::NextPageWidget *m_vpnBtn;
    dcc::NextPageWidget *m_proxyBtn;
    dcc::NextPageWidget *m_detailBtn;
};

#endif // NETWORKMODULEWIDGET_H
