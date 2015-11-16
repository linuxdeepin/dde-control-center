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

public slots:
    void onGeneratingAccessToken();
    void onGenAccessTokenFailed();
    void onGenAccessTokenSuccessed(QString);
    void onDisconnected();
    void onDisconnectedImmediately();
    void onSharing();

private:
    IShareController* m_controller;
};

#endif /* end of include guard: SHARE_PANEL_H_WNAMLTE2 */
