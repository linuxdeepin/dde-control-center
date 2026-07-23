//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "mousedbusproxy.h"
#include "gesturedata.h"

#include <DGuiApplicationHelper>

#include <QJsonArray>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>
#include <QDBusMetaType>
#include <QJsonDocument>
#include <QJsonObject>

using namespace DCC_NAMESPACE;

struct GestureActionInfoNew {
    QString actionId;
    QString displayName;
    bool supported = false;
    QString unavailableReason;
};
Q_DECLARE_METATYPE(GestureActionInfoNew)
Q_DECLARE_METATYPE(QList<GestureActionInfoNew>)

inline QDBusArgument &operator<<(QDBusArgument &argument, const GestureActionInfoNew &item) {
    argument.beginStructure();
    argument << item.actionId << item.displayName << item.supported << item.unavailableReason;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, GestureActionInfoNew &item) {
    argument.beginStructure();
    argument >> item.actionId >> item.displayName >> item.supported >> item.unavailableReason;
    argument.endStructure();
    return argument;
}

struct GestureInfoNew {
    QString id;
    QString displayName;
    QString category;
    int gestureType;
    int fingerCount;
    int direction;
    int triggerType;
    QStringList triggerValue;
    QString localLanguageName;
    QString localLanguageCategory;
    bool isCustom = false;
    QList<GestureActionInfoNew> availableActions;
};
Q_DECLARE_METATYPE(GestureInfoNew)
Q_DECLARE_METATYPE(QList<GestureInfoNew>)

inline QDBusArgument &operator<<(QDBusArgument &argument, const GestureInfoNew &info) {
    argument.beginStructure();
    argument << info.id << info.displayName << info.category
             << info.gestureType << info.fingerCount << info.direction
             << info.triggerType << info.triggerValue << info.localLanguageName
             << info.localLanguageCategory << info.isCustom << info.availableActions;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, GestureInfoNew &info) {
    argument.beginStructure();
    argument >> info.id >> info.displayName >> info.category
             >> info.gestureType >> info.fingerCount >> info.direction
             >> info.triggerType >> info.triggerValue >> info.localLanguageName
             >> info.localLanguageCategory >> info.isCustom >> info.availableActions;
    argument.endStructure();
    return argument;
}

static QString directionIntToString(int dir) {
    switch (dir) {
    case 1: return QStringLiteral("down");
    case 2: return QStringLiteral("left");
    case 3: return QStringLiteral("up");
    case 4: return QStringLiteral("right");
    case 0: return QStringLiteral("none");
    }
    qWarning() << "Wayland Unexpected gesture direction value:" << dir;
    return QStringLiteral("none");
}

static QString gestureTypeToString(int type) {
    switch (type) {
    case 1: return QStringLiteral("swipe");
    case 2: return QStringLiteral("tap");
    }
    qWarning() << "Wayland Unexpected gesture type value:" << type;
    return QStringLiteral("swipe"); // safe fallback
}

const QString Service = "org.deepin.dde.InputDevices1";
const QString MousePath = "/org/deepin/dde/InputDevice1/Mouse";
const QString TouchpadPath = "/org/deepin/dde/InputDevice1/TouchPad";
const QString TrackpointPath = "/org/deepin/dde/InputDevice1/Mouse";
const QString InputDevicesPath = "/org/deepin/dde/InputDevices1";
const QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const QString MouseInterface = "org.deepin.dde.InputDevice1.Mouse";
const QString TouchpadInterface = "org.deepin.dde.InputDevice1.TouchPad";
const QString TrackpointInterface = "org.deepin.dde.InputDevice1.TrackPoint";
const QString InputDevicesInterface = "org.deepin.dde.InputDevices1";
const QString GestureInterface = "org.deepin.dde.Gesture1";
const QString GesturePath = "/org/deepin/dde/Gesture1";
const QString GestureService = "org.deepin.dde.Gesture1";
const QString AppearanceService = "org.deepin.dde.Appearance1";
const QString AppearancePath = "/org/deepin/dde/Appearance1";
const QString AppearanceInterface = "org.deepin.dde.Appearance1";
const QString PowerService = QStringLiteral("org.deepin.dde.Power1");
const QString PowerPath = QStringLiteral("/org/deepin/dde/Power1");
const QString PowerInterface = QStringLiteral("org.deepin.dde.Power1");

MouseDBusProxy::MouseDBusProxy(QObject *parent)
    : QObject(parent)
    , m_dbusMouseProperties(nullptr)
    , m_dbusTouchPadProperties(nullptr)
    , m_dbusTrackPointProperties(nullptr)
    , m_dbusDevicesProperties(nullptr)
    , m_dbusMouse(nullptr)
    , m_dbusTouchPad(nullptr)
    , m_dbusTrackPoint(nullptr)
    , m_dbusDevices(nullptr)
    , m_dbusGesture(nullptr)
    , m_appearance(nullptr)
    , m_isWayland((qgetenv("XDG_SESSION_TYPE").toLower() == QLatin1String("wayland"))
                  || !qgetenv("WAYLAND_DISPLAY").isEmpty())
{
    qRegisterMetaType<GestureActionInfoNew>("GestureActionInfoNew");
    qDBusRegisterMetaType<GestureActionInfoNew>();
    qRegisterMetaType<QList<GestureActionInfoNew>>("QList<GestureActionInfoNew>");
    qDBusRegisterMetaType<QList<GestureActionInfoNew>>();
    qRegisterMetaType<GestureInfoNew>("GestureInfoNew");
    qDBusRegisterMetaType<GestureInfoNew>();
    qRegisterMetaType<QList<GestureInfoNew>>("QList<GestureInfoNew>");
    qDBusRegisterMetaType<QList<GestureInfoNew>>();

    init();
}

void MouseDBusProxy::active()
{
    if (!m_isWayland) {
        // initial mouse settingss
        bool exist = m_dbusMouseProperties->call("Get", MouseInterface, "Exist").arguments().at(0).value<QDBusVariant>().variant().toBool();
        bool leftHanded = m_dbusMouseProperties->call("Get", MouseInterface, "LeftHanded").arguments().at(0).value<QDBusVariant>().variant().toBool();
        bool naturalScroll = m_dbusMouseProperties->call("Get", MouseInterface, "NaturalScroll").arguments().at(0).value<QDBusVariant>().variant().toBool();
        int doubleClick = m_dbusMouseProperties->call("Get", MouseInterface, "DoubleClick").arguments().at(0).value<QDBusVariant>().variant().toInt();
        bool disableTpad = m_dbusMouseProperties->call("Get", MouseInterface, "DisableTpad").arguments().at(0).value<QDBusVariant>().variant().toBool();
        bool adaptiveAccelProfile = m_dbusMouseProperties->call("Get", MouseInterface, "AdaptiveAccelProfile").arguments().at(0).value<QDBusVariant>().variant().toBool();
        double motionAcceleration = m_dbusMouseProperties->call("Get", MouseInterface, "MotionAcceleration").arguments().at(0).value<QDBusVariant>().variant().toDouble();

        Q_EMIT mouseExistChanged(exist);
        Q_EMIT leftHandStateChanged(leftHanded);
        Q_EMIT mouseNaturalScrollStateChanged(naturalScroll);
        Q_EMIT douClickChanged(doubleClick);
        Q_EMIT disTouchPadChanged(disableTpad);
        Q_EMIT accelProfileChanged(adaptiveAccelProfile);
        Q_EMIT mouseMotionAccelerationChanged(motionAcceleration);

        // initial touchpad settings
        motionAcceleration = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "MotionAcceleration").arguments().at(0).value<QDBusVariant>().variant().toDouble();
        bool tapClick = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "TapClick").arguments().at(0).value<QDBusVariant>().variant().toBool();
        exist = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "Exist").arguments().at(0).value<QDBusVariant>().variant().toBool();
        bool touchpadEnabled = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "TPadEnable").arguments().at(0).value<QDBusVariant>().variant().toBool();
        naturalScroll = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "NaturalScroll").arguments().at(0).value<QDBusVariant>().variant().toBool();
        bool disableIfTyping = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "DisableIfTyping").arguments().at(0).value<QDBusVariant>().variant().toBool();

        Q_EMIT touchpadMotionAccelerationChanged(motionAcceleration);
        Q_EMIT tpadEnabledChanged(touchpadEnabled);
        Q_EMIT tapClickChanged(tapClick);
        Q_EMIT tpadExistChanged(exist);
        Q_EMIT touchNaturalScrollStateChanged(naturalScroll);
        Q_EMIT disTypingChanged(disableIfTyping);

        // initial device properties
        uint wheelSpeed  = m_dbusDevicesProperties->call("Get", InputDevicesInterface, "WheelSpeed").arguments().at(0).value<QDBusVariant>().variant().toUInt();
        Q_EMIT scrollSpeedChanged(wheelSpeed);
    }

    // common settings (X11/treeland 都会使用)
    bool palmDetect = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "PalmDetect").arguments().at(0).value<QDBusVariant>().variant().toInt();
    int palmMinWidth = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "PalmMinWidth").arguments().at(0).value<QDBusVariant>().variant().toInt();
    bool palmMinZ = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "PalmMinZ").arguments().at(0).value<QDBusVariant>().variant().toBool();

    Q_EMIT palmDetectChanged(palmDetect);
    Q_EMIT palmMinWidthChanged(palmMinWidth);
    Q_EMIT palmMinzChanged(palmMinZ);

    double motionAcceleration = m_dbusTrackPointProperties->call("Get", TrackpointInterface, "MotionAcceleration").arguments().at(0).value<QDBusVariant>().variant().toDouble();
    bool exist = m_dbusTouchPadProperties->call("Get", TrackpointInterface, "Exist").arguments().at(0).value<QDBusVariant>().variant().toBool();

    Q_EMIT trackPointMotionAccelerationChanged(motionAcceleration);
    Q_EMIT redPointExistChanged(exist);

    // initial lid is present
    bool lidIsPresent = getLidIsPresent();
    Q_EMIT lidIsPresentChanged(lidIsPresent);

    refreshGestures();

    listCursor();
    auto cursorSize = m_appearance->property("CursorSize").toInt();
    Q_EMIT cursorSizeChanged(cursorSize);
}

void MouseDBusProxy::refreshGestures()
{
    QDBusPendingCall call = m_dbusGesture->asyncCall(QStringLiteral("ListAllGestures"));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &MouseDBusProxy::onListAllGesturesNewFinished);
}

void MouseDBusProxy::deactive()
{
}

void MouseDBusProxy::init()
{
    // 监控dbus上的属性改变信号
    if (!m_isWayland) {
        QDBusConnection::sessionBus().connect(Service,
                                              MousePath,
                                              PropertiesInterface,
                                              "PropertiesChanged",
                                              "sa{sv}as",
                                              this,
                                              SLOT(onMousePathPropertiesChanged(QDBusMessage)));
    }
    QDBusConnection::sessionBus().connect(Service,
                                          TouchpadPath,
                                          PropertiesInterface,
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this,
                                          SLOT(onTouchpadPathPropertiesChanged(QDBusMessage)));

    QDBusConnection::sessionBus().connect(Service,
                                          TrackpointPath,
                                          PropertiesInterface,
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this,
                                          SLOT(onTrackpointPathPropertiesChanged(QDBusMessage)));

    QDBusConnection::sessionBus().connect(GestureService,
                                          GesturePath,
                                          GestureInterface,
                                          "GestureInfosChanged",
                                          this,
                                          SLOT(refreshGestures()));

    QDBusConnection::sessionBus().connect(AppearanceService,
                                          AppearancePath,
                                          PropertiesInterface,
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this,
                                          SLOT(onAppearancePropertiesChanged(QDBusMessage)));
    if (!m_isWayland) {
        QDBusConnection::sessionBus().connect(Service,
                                              InputDevicesPath,
                                              PropertiesInterface,
                                              "PropertiesChanged",
                                              "sa{sv}as",
                                              this,
                                              SLOT(onInputDevicesPathPropertiesChanged(QDBusMessage)));
    }

    // 初始化dbus接口
    m_dbusMouseProperties = new QDBusInterface(Service,
                                               MousePath,
                                               PropertiesInterface,
                                               QDBusConnection::sessionBus());
    m_dbusTouchPadProperties = new QDBusInterface(Service,
                                                  TouchpadPath,
                                                  PropertiesInterface,
                                                  QDBusConnection::sessionBus());
    m_dbusTrackPointProperties = new QDBusInterface(Service,
                                                    TrackpointPath,
                                                    PropertiesInterface,
                                                    QDBusConnection::sessionBus());
    m_dbusDevicesProperties = new QDBusInterface(Service,
                                                 InputDevicesPath,
                                                 PropertiesInterface,
                                                 QDBusConnection::sessionBus());
    m_appearance = new QDBusInterface(AppearanceService,
                                      AppearancePath,
                                      AppearanceInterface,
                                      QDBusConnection::sessionBus());

    m_dbusMouse = new QDBusInterface(Service, MousePath, MouseInterface, QDBusConnection::sessionBus());
    m_dbusTouchPad = new QDBusInterface(Service,
                                        TouchpadPath,
                                        TouchpadInterface,
                                        QDBusConnection::sessionBus());
    m_dbusTrackPoint = new QDBusInterface(Service,
                                          TrackpointPath,
                                          TrackpointInterface,
                                          QDBusConnection::sessionBus());
    m_dbusDevices = new QDBusInterface(Service,
                                       InputDevicesPath,
                                       InputDevicesInterface,
                                       QDBusConnection::sessionBus());
    m_dbusGesture = new QDBusInterface(GestureService,
                                       GesturePath,
                                       GestureInterface,
                                       QDBusConnection::sessionBus());
}

void MouseDBusProxy::onDefaultReset()
{
     QDBusPendingCallWatcher *watcherMouse = new QDBusPendingCallWatcher(m_dbusMouse->asyncCall("Reset"), this);
     QObject::connect(watcherMouse, &QDBusPendingCallWatcher::finished, this, [=](){
        watcherMouse->deleteLater();
     });

     QDBusPendingCallWatcher *watcherTouchPad = new QDBusPendingCallWatcher(m_dbusTouchPad->asyncCall("Reset"), this);
     QObject::connect(watcherTouchPad, &QDBusPendingCallWatcher::finished, this, [=](){
        watcherTouchPad->deleteLater();
     });

     QDBusPendingCallWatcher *watcherTrackPoint = new QDBusPendingCallWatcher(m_dbusTrackPoint->asyncCall("Reset"), this);
     QObject::connect(watcherTrackPoint, &QDBusPendingCallWatcher::finished, this, [=](){
        watcherTrackPoint->deleteLater();
     });

     QDBusPendingCallWatcher *watcherDevices = new QDBusPendingCallWatcher(m_dbusDevices->asyncCall("Reset"), this);
     QObject::connect(watcherDevices, &QDBusPendingCallWatcher::finished, this, [=](){
        watcherDevices->deleteLater();
     });
}

void MouseDBusProxy::setLeftHandState(const bool state)
{
    m_dbusMouseProperties->call("Set", MouseInterface, "LeftHanded", state);
    m_dbusTouchPadProperties->call("Set", TouchpadInterface, "LeftHanded", state);
    m_dbusTrackPointProperties->call("Set", TrackpointInterface, "LeftHanded", state);
}

void MouseDBusProxy::setDouClick(const int &value)
{
    m_dbusMouseProperties->call("Set", MouseInterface, "DoubleClick", value);
    m_dbusTouchPadProperties->call("Set", TouchpadInterface, "DoubleClick", value);
}

void MouseDBusProxy::setMouseNaturalScrollState(const bool state)
{
    m_dbusMouseProperties->call("Set", MouseInterface, "NaturalScroll", state);
}

void MouseDBusProxy::setDisableTouchPadWhenMouseExist(const bool state)
{
    m_dbusMouseProperties->call("Set", MouseInterface, "DisableTpad", state);
}

void MouseDBusProxy::setAccelProfile(const bool state)
{
    m_dbusMouseProperties->call("Set", MouseInterface, "AdaptiveAccelProfile", state);
}

void MouseDBusProxy::setMouseMotionAcceleration(const double &value)
{
    m_dbusMouseProperties->call("Set", MouseInterface, "MotionAcceleration", value);
}

void MouseDBusProxy::setTouchNaturalScrollState(const bool state)
{
    m_dbusTouchPadProperties->call("Set", TouchpadInterface, "NaturalScroll", state);
}

void MouseDBusProxy::setDisTyping(const bool state)
{
    m_dbusTouchPadProperties->call("Set", TouchpadInterface, "DisableIfTyping", state);
}

void MouseDBusProxy::setTouchpadMotionAcceleration(const double &value)
{
    m_dbusTouchPadProperties->call("Set", TouchpadInterface, "MotionAcceleration", value);
}

void MouseDBusProxy::setTapClick(const bool state)
{
    m_dbusTouchPadProperties->call("Set", TouchpadInterface, "TapClick", state);
}

void MouseDBusProxy::setPalmDetect(bool palmDetect)
{
    m_dbusTouchPadProperties->call("Set", TouchpadInterface, "PalmDetect", palmDetect);
}

void MouseDBusProxy::setPalmMinWidth(int palmMinWidth)
{
    m_dbusTouchPadProperties->call("Set", TouchpadInterface, "PalmMinWidth", palmMinWidth);
}

void MouseDBusProxy::setPalmMinz(int palmMinz)
{
    m_dbusTouchPadProperties->call("Set", TouchpadInterface, "PalmMinZ", palmMinz);
}

void MouseDBusProxy::setTouchpadEnabled(bool state)
{
    m_dbusTouchPad->asyncCallWithArgumentList("Enable", { state });
}

void MouseDBusProxy::setCursorSize(const int cursorSize)
{
    m_appearance->setProperty("CursorSize", QVariant::fromValue(cursorSize));
}

void MouseDBusProxy::setTrackPointMotionAcceleration(const double &value)
{
    m_dbusTrackPointProperties->call("Set", TrackpointInterface, "MotionAcceleration", value);
}

void MouseDBusProxy::setScrollSpeed(uint speed)
{
    m_dbusDevicesProperties->call("Set", InputDevicesInterface, "WheelSpeed", speed);
}

void MouseDBusProxy::setGesture(const QString &gestureId, const QString &actionId)
{
    QDBusPendingCall call = m_dbusGesture->asyncCallWithArgumentList(
        QStringLiteral("ModifyGesture"), {gestureId, QStringList{actionId}});
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        QDBusPendingReply<bool> reply = *watcher;
        if (reply.isError() || !reply.value())
            qWarning() << "ModifyGesture failed:" << reply.error();
        watcher->deleteLater();
        refreshGestures();
    });
}

void MouseDBusProxy::onMousePathPropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = msg.arguments().at(0).toString();
    if (interfaceName == MouseInterface) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "Exist") {
                Q_EMIT mouseExistChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "LeftHanded") {
                Q_EMIT leftHandStateChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "NaturalScroll") {
                Q_EMIT mouseNaturalScrollStateChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "DoubleClick") {
                Q_EMIT douClickChanged(changedProps.value(keys.at(i)).toInt());
            } else if(keys.at(i) == "DisableTpad") {
                Q_EMIT disTouchPadChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "AdaptiveAccelProfile") {
                Q_EMIT accelProfileChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "MotionAcceleration") {
                Q_EMIT mouseMotionAccelerationChanged(changedProps.value(keys.at(i)).toDouble());
            }
        }
    }
}

void MouseDBusProxy::onTouchpadPathPropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = msg.arguments().at(0).toString();
    if (interfaceName == TouchpadInterface) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            const QString &key = keys.at(i);

            if (m_isWayland) {
                if (key == "PalmDetect") {
                    Q_EMIT palmDetectChanged(changedProps.value(key).toBool());
                } else if (key == "PalmMinWidth") {
                    Q_EMIT palmMinWidthChanged(changedProps.value(key).toInt());
                } else if (key == "PalmMinZ") {
                    Q_EMIT palmMinzChanged(changedProps.value(key).toInt());
                }
                continue;
            }

            if (key == "PalmDetect") {
                Q_EMIT palmDetectChanged(changedProps.value(key).toBool());
            } else if (key == "PalmMinWidth") {
                Q_EMIT palmMinWidthChanged(changedProps.value(key).toInt());
            } else if (key == "PalmMinZ") {
                Q_EMIT palmMinzChanged(changedProps.value(key).toInt());
            } else if (key == "Exist") {
                Q_EMIT tpadExistChanged(changedProps.value(key).toBool());
            } else if (key == "TPadEnable") {
                Q_EMIT tpadEnabledChanged(changedProps.value(key).toBool());
            } else if (key == "NaturalScroll") {
                Q_EMIT touchNaturalScrollStateChanged(changedProps.value(key).toBool());
            } else if (key == "DisableIfTyping") {
                Q_EMIT disTypingChanged(changedProps.value(key).toBool());
            } else if (key == "DoubleClick") {
                Q_EMIT douClickChanged(changedProps.value(key).toInt());
            } else if (key == "MotionAcceleration") {
                Q_EMIT touchpadMotionAccelerationChanged(changedProps.value(key).toDouble());
            } else if (key == "TapClick") {
                Q_EMIT tapClickChanged(changedProps.value(key).toBool());
            }
        }
    }
}

void MouseDBusProxy::onTrackpointPathPropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = msg.arguments().at(0).toString();
    if (interfaceName == TrackpointInterface) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "Exist") {
                Q_EMIT redPointExistChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "MotionAcceleration") {
                Q_EMIT trackPointMotionAccelerationChanged(changedProps.value(keys.at(i)).toDouble());
            }
        }
    }
}

void MouseDBusProxy::onInputDevicesPathPropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = msg.arguments().at(0).toString();
    if (interfaceName == InputDevicesInterface) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "WheelSpeed") {
                Q_EMIT scrollSpeedChanged(changedProps.value(keys.at(i)).toUInt());
            }
        }
    }
}

static void fillGestureData(GestureData *data, const GestureInfoNew &info)
{
    data->setActionType(gestureTypeToString(info.gestureType));
    data->setGestureId(info.id);
    data->setDisplayName(info.localLanguageName.isEmpty() ? info.displayName
                                                          : info.localLanguageName);
    data->setDirection(directionIntToString(info.direction));
    data->setFingersNum(info.fingerCount);
    data->setActionName(info.triggerValue.isEmpty() ? QStringLiteral("0")
                                                    : info.triggerValue.first());
}

void MouseDBusProxy::onListAllGesturesNewFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QList<GestureInfoNew>> reply = *w;
    if (!reply.isError()) {
        int sequence = 0;
        for (const GestureInfoNew &info : reply.value()) {
            if (info.isCustom)
                continue;
            GestureData data;
            fillGestureData(&data, info);
            data.setSequence(sequence);

            QList<GestureActionData> actions;
            for (const GestureActionInfoNew &item : info.availableActions) {
                actions.append({item.actionId, item.displayName, item.supported,
                                item.unavailableReason});
            }
            data.setActions(actions);
            Q_EMIT gestureDataChanged(data);
            ++sequence;
        }
    } else {
        qWarning() << "ListAllGestures failed:" << reply.error();
    }
    w->deleteLater();
}

void MouseDBusProxy::onAppearancePropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = msg.arguments().at(0).toString();
    if (interfaceName == AppearanceInterface) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "CursorSize") {
                int cursorSize = changedProps.value(keys.at(i)).toInt();
                Q_EMIT cursorSizeChanged(cursorSize);
            } else if (keys.at(i) == "CursorTheme") {
                listCursor();
            }
        }
    }
}

void MouseDBusProxy::listCursor()
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(
        m_appearance->asyncCall("List", "cursor"), this);
        
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](){
        QDBusPendingReply<QString> reply = *watcher;
        if (!reply.isError()) {
            QString cursorTheme =  m_appearance->property("CursorTheme").toString();
            QJsonDocument document = QJsonDocument::fromJson(reply.value().toUtf8());
            auto array = document.array();
            for (int i = 0; i < array.size(); ++i) {
                QJsonValue value = array.at(i);
                if (value.isObject()) {
                    QJsonObject object = value.toObject();
                    QString name = object.value("Id").toString();
                    if (name == cursorTheme) {
                        auto availableSizesValue = object.value("AvailableSize").toArray();
                        QList<int> availableSizes;
                        for (const auto &size : availableSizesValue) {
                            availableSizes.append(size.toInt(-1));
                        }
                        Q_EMIT availableCursorSizesChanged(availableSizes);
                        break;
                    }
                }
            }
        } else {
            qWarning() << "asyncCall list cursor failed:" << reply.error();
        }
        watcher->deleteLater();
    });
}

bool MouseDBusProxy::getLidIsPresent()
{
    // 通过Power1 DBus服务获取Lid状态
    QDBusInterface powerInterface(PowerService,
                                  PowerPath,
                                  PowerInterface,
                                  QDBusConnection::sessionBus());
    QVariant lidIsPresentVariant = powerInterface.property("LidIsPresent");
    if (lidIsPresentVariant.isValid()) {
        return lidIsPresentVariant.toBool();
    }
    return false;
}
