#include <QLabel>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dtextbutton.h>

#include "constants.h"

#include "sharepanel.h"
#include "interface.h"
#include "generatingview.h"
#include "generatedview.h"
#include "connectedview.h"
#include "errorview.h"

DUI_USE_NAMESPACE

SharePanel::SharePanel(IShareController* controller, QWidget* p)
    : AbstractPanel(tr("Sharing"), p)
{
    setObjectName("SharePanel");
    m_controller = controller;
    connect(controller, SIGNAL(noNetwork()), this, SLOT(onNoNetwork()));

    if (controller->isSharing()) {
        onSharing();
        return;
    }

    connect(controller, SIGNAL(generatingAccessToken()), this, SLOT(onGeneratingAccessToken()));
    connect(controller, SIGNAL(disconnected()), this, SLOT(onDisconnectedImmediately()));
    connect(controller, SIGNAL(genAccessTokenFailed()), this, SLOT(onGenAccessTokenFailed()));
    connect(controller, SIGNAL(genAccessTokenSuccessed(QString)), this, SLOT(onGenAccessTokenSuccessed(QString)));
    controller->startGenAccessToken();
}

void SharePanel::onSharing()
{
    qDebug() << "sharing";
    auto view = new ConnectedView;
    connect(view, SIGNAL(disconnect()), this, SLOT(onDisconnected()));
    setWidget(view);
}

void SharePanel::onGeneratingAccessToken()
{
    qDebug() << "generating";
    auto view = new GeneratingView;
    connect(view, SIGNAL(cancel()), this, SLOT(onDisconnectedImmediately()));
    setWidget(view);
}

void SharePanel::onDisconnected()
{
    qDebug() << "disconnect";
    m_controller->disconnect();
    emit changePanel(ViewPanel::Main);
}

void SharePanel::onDisconnectedImmediately()
{
    qDebug() << "disconnect immedately";
    m_controller->cancel();
    emit changePanel(ViewPanel::Main);
}

void SharePanel::onGenAccessTokenFailed()
{
    setTitle(tr("Remote Assistance"));
    qDebug() << "gen access token failed";
    auto view = new ErrorView;
    auto button = new DTextButton(tr("Cancel"));
    QObject::connect(button, &DTextButton::clicked, [this]{
        onDisconnected();
    });
    view->addButton(button);
    button = new DTextButton(tr("Retry"));
    QObject::connect(button, &DTextButton::clicked, [this]{
        setTitle(tr("Sharing"));
        m_controller->retry();
    });
    view->addButton(button);
    setWidget(view->setText(tr("Failed to establish the connection, you can retry to connect")));
}

void SharePanel::onGenAccessTokenSuccessed(QString token)
{
    qDebug() << "gen access token done";
    auto view = new GeneratedView(token);
    connect(view, SIGNAL(cancel()), this, SLOT(onDisconnectedImmediately()));
    setWidget(view);
}
