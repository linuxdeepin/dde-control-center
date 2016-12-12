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
    dcc::widgets::SettingsGroup* m_updateGroup;
    dcc::widgets::SettingsGroup* m_settingsGroup;
    dcc::widgets::NextPageWidget* m_updateItem;
    dcc::widgets::NextPageWidget* m_settingsItem;
};

}
}
#endif // UPDATEVIEW_H
