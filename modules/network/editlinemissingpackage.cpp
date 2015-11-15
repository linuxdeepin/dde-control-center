#include <QHBoxLayout>

#include <libdui/dheaderline.h>

#include "constants.h"

#include "dtextaction.h"
#include "editlinemissingpackage.h"
#include "networkglobal.h"

DUI_USE_NAMESPACE

EditLineMissingPackage::EditLineMissingPackage(const QString &section, const QString &key,
                                               DBusConnectionSession *dbus, const QString &title,
                                               QWidget *parent) :
    NetworkBaseEditLine(section, key, dbus, title, parent)
{
    QBoxLayout *layout = qobject_cast<QBoxLayout*>(this->layout());

    if(layout) {
        DTextAction *action = new DTextAction;

        action->setText(tr("Click to install"));
        action->setFixedSize(action->sizeHint());

        layout->setDirection(QBoxLayout::TopToBottom);
        QWidget *label_widget = layout->takeAt(0)->widget();
        layout->addWidget(label_widget);
        layout->addSpacing(5);
        layout->addWidget(action);
        layout->setSpacing(0);

        connect(action, &DTextAction::clicked, this, [this] {
            QString dbus_name = "com.linuxdeepin.softwarecenter_frontend";
            QString dbus_path = "/com/linuxdeepin/softwarecenter_frontend";
            QDBusInterface *iface = new QDBusInterface(dbus_name, dbus_path, dbus_name,
                                                       QDBusConnection::sessionBus());

            ASYN_CALL(iface->asyncCall("install_pkgs", QStringList() << cacheValue().toString()), {
                          ASYN_CALL(iface->asyncCall("raise_to_top"), {
                              ASYN_CALL(iface->asyncCall("show_page", "install"), {
                                  iface->deleteLater();
                              }, iface, this)
                          }, iface, this)
                      }, iface, this)
        });
    }
}

void EditLineMissingPackage::setCurrentVpnName(const QString &name)
{
    setTitle(tr("Current system lacks component for %1").arg(name));
}

QSize EditLineMissingPackage::sizeHint() const
{
    return layout()->sizeHint();
}

void EditLineMissingPackage::hideEvent(QHideEvent *e)
{
    NetworkBaseEditLine::hideEvent(e);

    if(m_isVisible != isVisible()) {
        m_isVisible = isVisible();
        emit visibleChanged(m_isVisible);
    }
}

void EditLineMissingPackage::showEvent(QShowEvent *e)
{
    NetworkBaseEditLine::showEvent(e);

    if(m_isVisible != isVisible()) {
        m_isVisible = isVisible();
        emit visibleChanged(m_isVisible);
    }
}
