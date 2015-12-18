#ifndef ACCESS_PANEL_H_T7BZFECR
#define ACCESS_PANEL_H_T7BZFECR

#include <QString>

#include <libdui/libdui_global.h>

#include "abstractpanel.h"
#include "../controller/access.h"

DUI_BEGIN_NAMESPACE
class DTextButton;
DUI_END_NAMESPACE

class IAccessController;

class AccessPanel: public AbstractPanel
{
    Q_OBJECT
public:
    AccessPanel(IAccessController* controller, QWidget* p=nullptr);
    ~AccessPanel() { dtor(); }

public slots:
    void focus();

private slots:
    void onStopped();
    void onConnect(QString);
    void onConnecting();
    void onConnected();
    void onConnectFailed(AccessErrors);
    void onDisconnected();
    void abort() Q_DECL_OVERRIDE;

private slots:
    void emitChangePanel() Q_DECL_OVERRIDE;

private:
    void dtor();

private:
    IAccessController* m_controller;
    DUI_NAMESPACE::DTextButton* m_retryBtn;
};

#endif /* end of include guard: ACCESS_PANEL_H_T7BZFECR */
