//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "mousedbusproxy.h"
#include "gesturedata.h"

#include <QJsonArray>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>
#include <QJsonDocument>
#include <QJsonObject>

using namespace DCC_NAMESPACE;

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
    , m_dbusGestureProperties(nullptr)
    , m_dbusMouse(nullptr)
    , m_dbusTouchPad(nullptr)
    , m_dbusTrackPoint(nullptr)
    , m_dbusDevices(nullptr)
    , m_dbusGesture(nullptr)
    , m_appearance(nullptr)
{
    init();
}

void MouseDBusProxy::active()
{
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
    bool palmDetect = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "PalmDetect").arguments().at(0).value<QDBusVariant>().variant().toInt();
    int palmMinWidth = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "PalmMinWidth").arguments().at(0).value<QDBusVariant>().variant().toInt();
    bool palmMinZ = m_dbusTouchPadProperties->call("Get", TouchpadInterface, "PalmMinZ").arguments().at(0).value<QDBusVariant>().variant().toBool();

    Q_EMIT touchpadMotionAccelerationChanged(motionAcceleration);
    Q_EMIT tpadEnabledChanged(touchpadEnabled);
    Q_EMIT tapClickChanged(tapClick);
    Q_EMIT tpadExistChanged(exist);
    Q_EMIT touchNaturalScrollStateChanged(naturalScroll);
    Q_EMIT disTypingChanged(disableIfTyping);
    Q_EMIT palmDetectChanged(palmDetect);
    Q_EMIT palmMinWidthChanged(palmMinWidth);
    Q_EMIT palmMinzChanged(palmMinZ);

    motionAcceleration = m_dbusTrackPointProperties->call("Get", TrackpointInterface, "MotionAcceleration").arguments().at(0).value<QDBusVariant>().variant().toDouble();
    exist = m_dbusTouchPadProperties->call("Get", TrackpointInterface, "Exist").arguments().at(0).value<QDBusVariant>().variant().toBool();

    Q_EMIT trackPointMotionAccelerationChanged(motionAcceleration);
    Q_EMIT redPointExistChanged(exist);

    // initial device properties
    uint wheelSpeed  = m_dbusDevicesProperties->call("Get", InputDevicesInterface, "WheelSpeed").arguments().at(0).value<QDBusVariant>().variant().toUInt();
    Q_EMIT scrollSpeedChanged(wheelSpeed);

    // initial lid is present
    bool lidIsPresent = getLidIsPresent();
    Q_EMIT lidIsPresentChanged(lidIsPresent);

    QVariant gestureInfos = m_dbusGestureProperties->call("Get", GestureInterface, "Infos").arguments().at(0).value<QDBusVariant>().variant();
    parseGesturesData(qvariant_cast<QDBusArgument>(gestureInfos));

    listCursor();
    auto cursorSize = m_appearance->property("CursorSize").toInt();
    Q_EMIT cursorSizeChanged(cursorSize);
}

void MouseDBusProxy::deactive()
{
}

void MouseDBusProxy::init()
{
    // 监控dbus上的属性改变信号
    QDBusConnection::sessionBus().connect(Service,
                                          MousePath,
                                          PropertiesInterface,
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this,
                                          SLOT(onMousePathPropertiesChanged(QDBusMessage)));

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

    QDBusConnection::sessionBus().connect(Service,
                                          InputDevicesPath,
                                          PropertiesInterface,
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this,
                                          SLOT(onInputDevicesPathPropertiesChanged(QDBusMessage)));
    QDBusConnection::sessionBus().connect(GestureService,
                                          GesturePath,
                                          PropertiesInterface,
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this,
                                          SLOT(onGesturePropertiesChanged(QDBusMessage)));

    QDBusConnection::sessionBus().connect(AppearanceService,
                                          AppearancePath,
                                          PropertiesInterface,
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this,
                                          SLOT(onAppearancePropertiesChanged(QDBusMessage)));

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
    m_dbusGestureProperties = new QDBusInterface(GestureService,
                                                 GesturePath,
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

void MouseDBusProxy::parseGesturesData(const QDBusArgument &argument)
{
    // 开始解析数组
    argument.beginArray();
    while (!argument.atEnd()) {

        QString actionType, direction, actionName;
        qint32 fingerNum;

        // 开始解析 Struct of (String,String,Int32,String)
        argument.beginStructure();

        argument >> actionType;
        argument >> direction;
        argument >> fingerNum;
        argument >> actionName;
        argument.endStructure();

        QVariantMap data;
        data.insert("actionType", actionType);
        data.insert("direction", direction);
        data.insert("actionName", actionName);
        data.insert("fingerNum", fingerNum);

        QDBusPendingReply<QString> reply = m_dbusGesture->asyncCall("GetGestureAvaiableActions", actionType, fingerNum);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        watcher->setProperty("data", QVariant::fromValue(data));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &MouseDBusProxy::onGetGestureAvaiableActionsFinished);
    }
    argument.endArray();
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

void MouseDBusProxy::setGesture(const QString& name, const QString& direction, int fingers, const QString& action)
{
    m_dbusGesture->asyncCallWithArgumentList("SetGesture", { name, direction, fingers, action });
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
            if (keys.at(i) == "Exist") {
                Q_EMIT tpadExistChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "TPadEnable") {
                Q_EMIT tpadEnabledChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "NaturalScroll") {
                Q_EMIT touchNaturalScrollStateChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "DisableIfTyping") {
                Q_EMIT disTypingChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "DoubleClick") {
                Q_EMIT douClickChanged(changedProps.value(keys.at(i)).toInt());
            } else if(keys.at(i) == "MotionAcceleration") {
                Q_EMIT touchpadMotionAccelerationChanged(changedProps.value(keys.at(i)).toDouble());
            } else if(keys.at(i) == "TapClick") {
                Q_EMIT tapClickChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "PalmDetect") {
                Q_EMIT palmDetectChanged(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "PalmMinWidth") {
                Q_EMIT palmMinWidthChanged(changedProps.value(keys.at(i)).toInt());
            } else if(keys.at(i) == "PalmMinZ") {
                Q_EMIT palmMinzChanged(changedProps.value(keys.at(i)).toInt());
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

void MouseDBusProxy::onGesturePropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = msg.arguments().at(0).toString();
    if (interfaceName == GestureInterface) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "Infos") {
                parseGesturesData(qvariant_cast<QDBusArgument>(changedProps.value(keys.at(i))));
            }
        }
    }
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

void MouseDBusProxy::onGetGestureAvaiableActionsFinished(QDBusPendingCallWatcher *w)
{
    QVariantMap dbusData = w->property("data").toMap();

    GestureData data;
    data.setActionType(dbusData.value("actionType").toString());
    data.setDirection(dbusData.value("direction").toString());
    data.setActionName(dbusData.value("actionName").toString());
    data.setFingersNum(dbusData.value("fingerNum").toInt());
    QDBusPendingReply<QString> reply = *w;
    if (!reply.isError()) {
        QString actions = reply.value();
        QJsonDocument document = QJsonDocument::fromJson(actions.toUtf8());

        if (document.isArray()) {
            QJsonArray array = document.array();
            QStringList actionNameList;
            QStringList actionDescriptionList;
            for (int i = 0; i < array.size(); ++i) {
                QJsonValue value = array.at(i);
                if (value.isObject()) {
                    QJsonObject object = value.toObject();

                    QPair<QString, QString> actionPair;
                    actionPair.first = object.value("Name").toString();
                    actionPair.second = object.value("Description").toString();
                    data.addActiosPair(actionPair);
                }
            }
        }
    }

    Q_EMIT gestureDataChanged(data);
    w->deleteLater();
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
