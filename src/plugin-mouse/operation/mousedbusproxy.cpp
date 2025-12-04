//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
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

MouseDBusProxy::MouseDBusProxy(MouseWorker *worker, QObject *parent)
    : QObject(parent)
    , m_worker(worker)
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

    m_worker->setMouseExist(exist);
    m_worker->setLeftHandState(leftHanded);
    m_worker->setMouseNaturalScrollState(naturalScroll);
    m_worker->setDouClick(doubleClick);
    m_worker->setDisTouchPad(disableTpad);
    m_worker->setAccelProfile(adaptiveAccelProfile);
    m_worker->setMouseMotionAcceleration(motionAcceleration);

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

    m_worker->setTouchpadMotionAcceleration(motionAcceleration);
    m_worker->setTpadEnabled(touchpadEnabled);
    m_worker->setTapClick(tapClick);
    m_worker->setTpadExist(exist);
    m_worker->setTouchNaturalScrollState(naturalScroll);
    m_worker->setDisTyping(disableIfTyping);
    m_worker->setPalmDetect(palmDetect);
    m_worker->setPalmMinWidth(palmMinWidth);
    m_worker->setPalmMinz(palmMinZ);

    // initial redpoint settings
    motionAcceleration = m_dbusTrackPointProperties->call("Get", TrackpointInterface, "MotionAcceleration").arguments().at(0).value<QDBusVariant>().variant().toDouble();
    exist = m_dbusTouchPadProperties->call("Get", TrackpointInterface, "Exist").arguments().at(0).value<QDBusVariant>().variant().toBool();

    m_worker->setTrackPointMotionAcceleration(motionAcceleration);
    m_worker->setRedPointExist(exist);

    // initial device properties
    uint wheelSpeed  = m_dbusDevicesProperties->call("Get", InputDevicesInterface, "WheelSpeed").arguments().at(0).value<QDBusVariant>().variant().toUInt();

    m_worker->setScrollSpeed(wheelSpeed);

    QVariant gestureInfos = m_dbusGestureProperties->call("Get", GestureInterface, "Infos").arguments().at(0).value<QDBusVariant>().variant();
    parseGesturesData(qvariant_cast<QDBusArgument>(gestureInfos));
    m_worker->initFingerGestures();

    listCursor();
    auto cursorSize = m_appearance->property("CursorSize").toInt();
    m_worker->setCursorSize(cursorSize);
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

    // set Mouse settings from dde-control-center
    connect(m_worker,
            &MouseWorker::requestSetLeftHandState,
            this,
            &MouseDBusProxy::setLeftHandState);
    connect(m_worker,
            &MouseWorker::requestSetMouseNaturalScrollState,
            this,
            &MouseDBusProxy::setMouseNaturalScrollState);
    connect(m_worker, &MouseWorker::requestSetDouClick, this, &MouseDBusProxy::setDouClick);
    connect(m_worker,
            &MouseWorker::requestSetDisTouchPad,
            this,
            &MouseDBusProxy::setDisableTouchPadWhenMouseExist);
    connect(m_worker, &MouseWorker::requestSetAccelProfile, this, &MouseDBusProxy::setAccelProfile);
    connect(m_worker,
            &MouseWorker::requestSetMouseMotionAcceleration,
            this,
            &MouseDBusProxy::setMouseMotionAcceleration);

    // set Touchpad settings from dde-control-center
    connect(m_worker,
            &MouseWorker::requestSetTouchNaturalScrollState,
            this,
            &MouseDBusProxy::setTouchNaturalScrollState);
    connect(m_worker, &MouseWorker::requestSetDisTyping, this, &MouseDBusProxy::setDisTyping);
    connect(m_worker,
            &MouseWorker::requestSetTouchpadMotionAcceleration,
            this,
            &MouseDBusProxy::setTouchpadMotionAcceleration);
    connect(m_worker, &MouseWorker::requestSetTapClick, this, &MouseDBusProxy::setTapClick);
    connect(m_worker, &MouseWorker::requestSetPalmDetect, this, &MouseDBusProxy::setPalmDetect);
    connect(m_worker, &MouseWorker::requestSetPalmMinWidth, this, &MouseDBusProxy::setPalmMinWidth);
    connect(m_worker, &MouseWorker::requestSetPalmMinz, this, &MouseDBusProxy::setPalmMinz);

    // set Redpoint settings from dde-control-center
    connect(m_worker,
            &MouseWorker::requestSetTrackPointMotionAcceleration,
            this,
            &MouseDBusProxy::setTrackPointMotionAcceleration);

    // set Device properties from dde-control-center
    connect(m_worker, &MouseWorker::requestSetScrollSpeed, this, &MouseDBusProxy::setScrollSpeed);
    connect(m_worker,
            &MouseWorker::requestSetTouchpadEnabled,
            this,
            &MouseDBusProxy::setTouchpadEnabled);

    connect(m_worker, &MouseWorker::requestSetGesture, this, &MouseDBusProxy::setGesture);
    connect(m_worker, &MouseWorker::requestSetCursorSize, this, &MouseDBusProxy::setCursorSize);
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

        argument >> actionType ;
        argument >> direction ;
        argument >> fingerNum;
        argument >> actionName;
        argument.endStructure();

        GestureData data;
        data.setActionType(actionType);
        data.setDirection(direction);
        data.setActionName(actionName);
        data.setFingersNum(fingerNum);

        QDBusPendingReply<QString> reply = m_dbusGesture->callWithArgumentList(QDBus::BlockWithGui,"GetGestureAvaiableActions", {actionType, fingerNum});

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
                        actionPair.first = object.value("Name").toString();;
                        actionPair.second = object.value("Description").toString();
                        data.addActiosPair(actionPair);
                    }
                }
            }
        }

        m_worker->setGestureData(data);
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
                m_worker->setMouseExist(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "LeftHanded") {
                m_worker->setLeftHandState(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "NaturalScroll") {
                m_worker->setMouseNaturalScrollState(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "DoubleClick") {
                m_worker->setDouClick(changedProps.value(keys.at(i)).toInt());
            } else if(keys.at(i) == "DisableTpad") {
                m_worker->setDisTouchPad(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "AdaptiveAccelProfile") {
                m_worker->setAccelProfile(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "MotionAcceleration") {
                m_worker->setMouseMotionAcceleration(changedProps.value(keys.at(i)).toDouble());
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
                m_worker->setTpadExist(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "TPadEnable") {
                m_worker->setTpadEnabled(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "NaturalScroll") {
                m_worker->setTouchNaturalScrollState(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "DisableIfTyping") {
                m_worker->setDisTyping(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "DoubleClick") {
                m_worker->setDouClick(changedProps.value(keys.at(i)).toInt());
            } else if(keys.at(i) == "MotionAcceleration") {
                m_worker->setTouchpadMotionAcceleration(changedProps.value(keys.at(i)).toDouble());
            } else if(keys.at(i) == "TapClick") {
                m_worker->setTapClick(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "PalmDetect") {
                m_worker->setPalmDetect(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "PalmMinWidth") {
                m_worker->setPalmMinWidth(changedProps.value(keys.at(i)).toInt());
            } else if(keys.at(i) == "PalmMinZ") {
                m_worker->setPalmMinz(changedProps.value(keys.at(i)).toInt());
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
                m_worker->setRedPointExist(changedProps.value(keys.at(i)).toBool());
            } else if(keys.at(i) == "MotionAcceleration") {
                m_worker->setTrackPointMotionAcceleration(changedProps.value(keys.at(i)).toDouble());
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
                m_worker->setScrollSpeed(changedProps.value(keys.at(i)).toUInt());
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
                m_worker->setCursorSize(cursorSize);
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
                        m_worker->setAvailableCursorSizes(availableSizes);
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
