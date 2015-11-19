#include <QLabel>
#include <QVBoxLayout>

#include "accesspanel.h"
#include <libdui/dtextbutton.h>

#include "connectingview.h"
#include "connectedview.h"
#include "errorview.h"
#include "inputview.h"

DUI_USE_NAMESPACE

AccessPanel::AccessPanel(IAccessController* controller, QWidget* p)
    : AbstractPanel(tr("Accessing"), p)
{
    setObjectName("AccessPanel");
    m_controller = controller;
    connect(controller, SIGNAL(noNetwork()), this, SLOT(onNoNetwork()));

    if (controller->isAlreadyConnected()) {
        onConnected();
        return;
    }

    auto view = new InputView;
    connect(view, SIGNAL(connect(QString)), this, SLOT(onConnect(QString)));
    connect(view, SIGNAL(cancel()), this, SLOT(onDisconnected()));
    setWidget(view);

    connect(controller, SIGNAL(connecting()), this, SLOT(onConnecting()));
    connect(controller, SIGNAL(connectFailed(AccessErrors)), this, SLOT(onConnectFailed(AccessErrors)));
    controller->initStatus();
}

void AccessPanel::onConnect(QString token)
{
    m_controller->connect(token);
    onConnecting();
}

void AccessPanel::onConnecting()
{
    setTitle(tr("Accessing"));
    qDebug() << "connecting";
    auto view = new ConnectingView;
    connect(view, SIGNAL(cancel()), this, SLOT(onDisconnected()));
    setWidget(view);
}

void AccessPanel::onConnected()
{
    qDebug() << "connected";
    auto view = new ConnectedView;
    connect(view, SIGNAL(disconnect()), this, SLOT(onDisconnected()));
    setWidget(view);
    connect(m_controller, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void AccessPanel::onConnectFailed(AccessErrors e)
{
    qDebug() << "connect failed";
    setTitle(tr("Remote Assistance"));
    auto view = new ErrorView;
    switch (e) {
    case AccessError::ConnectServerFailed:
        qDebug() << "connect server failed";
        break;
    case AccessError::InvalidToken:
        qDebug() << "invalid token";
        break;
    }
    view->setText(tr("Failed to establish the connection, you can retry to connect"));
    auto button = new DTextButton(tr("Cancel"));
    connect(button, &DTextButton::clicked, [this]{
        emit changePanel(ViewPanel::Main);
    });
    view->addButton(button);
    button = new DTextButton(tr("Retry"));
    button->setEnabled(false);
    // waiting the remoting window to be closed.
    // NB: QTimer::singleShot not support lambda in Qt5.3.
    auto timer = new QTimer;
    timer->setInterval(2000);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [=]{
        timer->deleteLater();
        qDebug() << "enable retry button";
        button->setEnabled(true);
    });
    connect(button, &DTextButton::clicked, [this]{
        m_controller->retry();
    });
    view->addButton(button);
    setWidget(view);
}

void AccessPanel::onDisconnected()
{
    qDebug() << "disconnected accessing";
    m_controller->disconnect();
    emit changePanel(ViewPanel::Main);
}

void AccessPanel::focus()
{
    if (m_view->objectName() == "InputView")
        qobject_cast<InputView*>(m_view)->focus();
}
