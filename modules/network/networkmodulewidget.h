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
    dcc::NextPageWidget *m_detailPage;
};

#endif // NETWORKMODULEWIDGET_H
