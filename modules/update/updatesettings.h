#ifndef UPDATECTROLMIRRORS_H
#define UPDATECTROLMIRRORS_H

#include "contentwidget.h"
#include "switchwidget.h"
#include "nextpagewidget.h"

using namespace dcc::widgets;

namespace dcc{
namespace update{

class UpdateModel;

class UpdateSettings : public ContentWidget
{
    Q_OBJECT

public:
    explicit UpdateSettings(UpdateModel* model, QWidget *parent = 0);

    void setModel(UpdateModel *model);

signals:
    void requestShowMirrorsView();
    void requestSetAutoUpdate(const bool &autoUpdate);

private:
    UpdateModel* m_model;
    SwitchWidget* m_autoDownloadSwitch;
#ifndef DISABLE_SYS_UPDATE_MIRRORS
    NextPageWidget* m_updateMirrors;
#endif
};

}
}
#endif // UPDATECTROLMIRRORS_H
