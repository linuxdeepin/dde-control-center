#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QFrame>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "moduleheader.h"
#include "user_list/userlistpanel.h"
#include "user_create/createuserpanel.h"
#include "libdui/dconstants.h"
#include "libdui/dtextbutton.h"
#include "libdui/dthememanager.h"
#include "libdui/dseparatorhorizontal.h"
#include "generalremovebutton.h"
#include "generaladdbutton.h"
#include "dynamiclabel.h"

class AccountMainWidget : public QFrame
{
    Q_OBJECT
public:
    enum PanelState {
        StateCreating,
        StateDeleting,
        StateSetting,
        StateNormal
    };

    explicit AccountMainWidget(QWidget *parent = 0);
    void preDestroy();

signals:
    void stateChanged(PanelState state);
    void requestDelete(bool flag);
    void cancelDelete();
    void hideForSetting();
    void showForNormal();

private:
    void initHeader();
    void initListPanel();
    void initCreatePanel();
    void initHeaderStackWidget();

    void setPanelState(PanelState state);
private:
    QStackedWidget *m_headerStackWidget = NULL;
    QStackedWidget *m_stackWidget = NULL;
    CreateUserPanel *m_createPanel = NULL;
    QScrollArea *m_listScrollArea = NULL;
    UserListPanel *m_listPanel = NULL;
    QVBoxLayout *m_mainLayout = NULL;
    ModuleHeader *m_header = NULL;

    PanelState m_state = StateNormal;
};

#endif // MAINWIDGET_H
