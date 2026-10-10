// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "xdgactivation_p.h"

#include <DSGApplication>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QMetaObject>
#include <QPointer>
#include <QtWaylandClient/QWaylandClientExtension>

#include "qwayland-xdg-activation-v1.h"
#include <private/qwaylanddisplay_p.h>
#include <private/qwaylandinputdevice_p.h>
#include <private/qwaylandwindow_p.h>

Q_LOGGING_CATEGORY(trayXdgActivation, "dde.tray.xdgactivation")

namespace tray {

class XdgActivationTokenV1 : public QObject, public QtWayland::xdg_activation_token_v1
{
    Q_OBJECT

public:
    ~XdgActivationTokenV1() override
    {
        destroy();
    }

Q_SIGNALS:
    void done(const QString &token);

protected:
    void xdg_activation_token_v1_done(const QString &token) override
    {
        Q_EMIT done(token);
    }
};

namespace {

class XdgActivationV1 : public QWaylandClientExtensionTemplate<XdgActivationV1>,
                        public QtWayland::xdg_activation_v1
{
public:
    XdgActivationV1()
        : QWaylandClientExtensionTemplate<XdgActivationV1>(1)
    {
        initialize();
    }

    ~XdgActivationV1() override
    {
        if (isInitialized())
            destroy();
    }

    XdgActivationTokenV1 *createTokenProvider(QWindow *window, const QString &appId)
    {
        auto *provider = new XdgActivationTokenV1;
        provider->init(get_activation_token());

        if (window) {
            if (auto *waylandWindow = dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle())) {
                if (auto *surface = waylandWindow->wlSurface())
                    provider->set_surface(surface);
                if (auto *inputDevice = waylandWindow->display()->lastInputDevice())
                    provider->set_serial(inputDevice->serial(), inputDevice->wl_seat());
            }
        }

        if (!appId.isEmpty())
            provider->set_app_id(appId);

        provider->commit();
        return provider;
    }
};

XdgActivationV1 *activationV1()
{
    static QPointer<XdgActivationV1> activation;
    if (activation)
        return activation;

    activation = new XdgActivationV1;
    activation->setParent(qApp);
    return activation;
}

} // namespace

// ---------------------------------------------------------------------------
// XdgActivationPrivate
// ---------------------------------------------------------------------------

XdgActivationPrivate::XdgActivationPrivate(XdgActivation *qq)
    : DObjectPrivate(qq)
{
}

XdgActivationPrivate::~XdgActivationPrivate() = default;

// ---------------------------------------------------------------------------
// XdgActivation
// ---------------------------------------------------------------------------

XdgActivation::XdgActivation(QObject *parent)
    : QObject(parent)
    , DObject(*new XdgActivationPrivate(this))
{
}

XdgActivation::~XdgActivation() = default;

bool XdgActivation::isActive() const
{
    auto *activation = activationV1();
    const bool active = activation && activation->isActive();
    qCDebug(trayXdgActivation) << "isActive:" << active;
    return active;
}

void XdgActivation::requestToken(QWindow *window, const QString &appId)
{
    D_D(XdgActivation);

    if (d->provider) {
        qCWarning(trayXdgActivation) << "XDG activation token request already started";
        return;
    }

    if (!isActive()) {
        qCDebug(trayXdgActivation) << "xdg_activation_v1 is not active; token request skipped";
        Q_EMIT tokenReady({});
        return;
    }

    const QString effectiveAppId = appId.isEmpty() ? QString::fromUtf8(DTK_CORE_NAMESPACE::DSGApplication::id()) : appId;
    if (effectiveAppId.isEmpty())
        qCWarning(trayXdgActivation) << "XDG activation request has empty app id";

    auto effectiveWindow = window ? window : QGuiApplication::focusWindow();

    auto *provider = activationV1()->createTokenProvider(effectiveWindow, effectiveAppId);
    provider->setParent(this);
    d->provider = provider;

    connect(provider, &XdgActivationTokenV1::done, this, [this, provider, effectiveAppId](const QString &token) {
        D_D(XdgActivation);
        d->provider = nullptr;

        if (token.isEmpty())
            qCWarning(trayXdgActivation) << "XDG activation token missing for app:" << effectiveAppId;
        else
            qCDebug(trayXdgActivation) << "XDG activation token received for app:" << effectiveAppId;

        provider->deleteLater();
        Q_EMIT tokenReady(token);
    }, Qt::SingleShotConnection);
}

} // namespace tray

#include "xdgactivation.moc"
