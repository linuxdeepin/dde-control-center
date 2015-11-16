#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

#include "dbus/manager.h"

#include "abstractpanel.h"

class ButtonGroup;
class MainPanel : public AbstractPanel
{
    Q_OBJECT
public:
    MainPanel(com::deepin::daemon::Remoting::Manager* manager, QWidget*p=nullptr);

private slots:
    void changeToSharePanel();
    void changeToAccessPanel();
    void emitPanelChanged(ViewPanel);

private:
    com::deepin::daemon::Remoting::Manager* m_manager;
    ButtonGroup* m_buttongroup;
};

#endif
