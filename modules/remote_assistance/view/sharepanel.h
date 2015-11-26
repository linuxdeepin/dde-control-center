#ifndef SHARE_PANEL_H_WNAMLTE2
#define SHARE_PANEL_H_WNAMLTE2

#include <QString>
#include "abstractpanel.h"

class IShareController;
class QLabel;
class AbstractView;

class SharePanel : public AbstractPanel
{
    Q_OBJECT
public:
    SharePanel(IShareController*, QWidget* p=nullptr);
    ~SharePanel() { dtor(); }

public slots:
    void onGeneratingAccessToken();
    void onGenAccessTokenFailed();
    void onGenAccessTokenSuccessed(QString);
    void onDisconnected();
    void onDisconnectedImmediately();
    void onSharing();
    void onStopped();

private slots:
    void emitChangePanel();

private:
    void dtor();

private:
    IShareController* m_controller;
};

#endif /* end of include guard: SHARE_PANEL_H_WNAMLTE2 */
