#ifndef UPDATEVIEW_H
#define UPDATEVIEW_H

#include "modulewidget.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"

namespace dcc{
namespace update{
class UpdateView : public ModuleWidget
{
    Q_OBJECT

public:
    explicit UpdateView();

signals:
    void pushUpdate();
    void pushMirrors();

private:
    dcc::SettingsGroup* m_updateGroup;
    dcc::SettingsGroup* m_settingsGroup;
    dcc::NextPageWidget* m_updateItem;
    dcc::NextPageWidget* m_settingsItem;
};

}
}
#endif // UPDATEVIEW_H
