//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "mousemodel.h"
#include "dccfactory.h"
#include "mouseworker.h"
#include "mousedbusproxy.h"

using namespace DCC_NAMESPACE;
MouseModel::MouseModel(QObject *parent)
    : QObject(parent)
    , m_leftHandState(false)
    , m_disIfTyping(false)
    , m_tpadExist(false)
    , m_mouseExist(true)
    , m_redPointExist(false)
    , m_mouseNaturalScroll(false)
    , m_tpadNaturalScroll(false)
    , m_accelProfile(true)
    , m_disTpad(false)
    , m_palmDetect(false)
    , m_tapClick(false)
    , m_touchpadEnabled(true)
    , m_doubleSpeed(1)
    , m_mouseMoveSpeed(1)
    , m_tpadMoveSpeed(1)
    , m_redPointMoveSpeed(1)
    , m_palmMinWidth(1)
    , m_palmMinz(100)
    , m_scrollSpeed(1)
    , m_gestureFingerAniPath("")
    , m_gestureActionAniPath("")
    , m_themeType(Dtk::Gui::DGuiApplicationHelper::instance()->themeType())
    , m_threeFingerGestureModel(new GestureModel(this))
    , m_fourFigerGestureModel(new GestureModel(this))
    , m_worker(new MouseWorker(this, this))
{
    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged, this, [ this ]() {
        auto themeColorType = Dtk::Gui::DGuiApplicationHelper::instance()->themeType();
        updateFigerAniPath(themeColorType);
        setThemeType(themeColorType);
    });
}

MouseModel::~MouseModel()
{

}

void MouseModel::setLeftHandState(const bool state)
{
    if (m_leftHandState == state)
        return;

    m_leftHandState = state;

    QMetaObject::invokeMethod(m_worker,"onLeftHandStateChanged", Qt::QueuedConnection, Q_ARG(bool, m_leftHandState));
    Q_EMIT leftHandStateChanged(state);
}

void MouseModel::setDisIfTyping(const bool state)
{
    if (m_disIfTyping == state)
        return;

    m_disIfTyping = state;

    QMetaObject::invokeMethod(m_worker,"onDisTypingChanged", Qt::QueuedConnection, Q_ARG(bool, m_disIfTyping));
    Q_EMIT disIfTypingStateChanged(state);
}

void MouseModel::setTpadExist(bool tpadExist)
{
    if (m_tpadExist == tpadExist)
        return;

    m_tpadExist = tpadExist;

    Q_EMIT tpadExistChanged(tpadExist);
}

void MouseModel::setMouseExist(bool mouseExist)
{
    if (m_mouseExist == mouseExist)
        return;

    m_mouseExist = mouseExist;

    Q_EMIT mouseExistChanged(mouseExist);
}

void MouseModel::setRedPointExist(bool redPointExist)
{
    if (m_redPointExist == redPointExist)
        return;

    m_redPointExist = redPointExist;

    Q_EMIT redPointExistChanged(redPointExist);
}

void MouseModel::setDoubleSpeed(int doubleSpeed)
{
    if (m_doubleSpeed == doubleSpeed)
        return;

    m_doubleSpeed = doubleSpeed;

    QMetaObject::invokeMethod(m_worker,"onDouClickChanged", Qt::QueuedConnection, Q_ARG(int, m_doubleSpeed));
    Q_EMIT doubleSpeedChanged(doubleSpeed);
}

void MouseModel::setMouseNaturalScroll(bool mouseNaturalScroll)
{
    if (m_mouseNaturalScroll == mouseNaturalScroll)
        return;

    m_mouseNaturalScroll = mouseNaturalScroll;

    QMetaObject::invokeMethod(m_worker,"onMouseNaturalScrollStateChanged", Qt::QueuedConnection, Q_ARG(bool, m_mouseNaturalScroll));
    Q_EMIT mouseNaturalScrollChanged(mouseNaturalScroll);
}

void MouseModel::setTpadNaturalScroll(bool tpadNaturalScroll)
{
    if (m_tpadNaturalScroll == tpadNaturalScroll)
        return;

    m_tpadNaturalScroll = tpadNaturalScroll;

    QMetaObject::invokeMethod(m_worker,"onTouchNaturalScrollStateChanged", Qt::QueuedConnection, Q_ARG(bool, m_tpadNaturalScroll));
    Q_EMIT tpadNaturalScrollChanged(tpadNaturalScroll);
}

void MouseModel::setMouseMoveSpeed(int mouseMoveSpeed)
{
    if (m_mouseMoveSpeed == mouseMoveSpeed)
        return;

    m_mouseMoveSpeed = mouseMoveSpeed;

    QMetaObject::invokeMethod(m_worker,"onMouseMotionAccelerationChanged", Qt::QueuedConnection, Q_ARG(int, m_mouseMoveSpeed));
    Q_EMIT mouseMoveSpeedChanged(mouseMoveSpeed);
}

void MouseModel::setTpadMoveSpeed(int tpadMoveSpeed)
{
    if (m_tpadMoveSpeed == tpadMoveSpeed)
        return;

    m_tpadMoveSpeed = tpadMoveSpeed;

    QMetaObject::invokeMethod(m_worker,"onTouchpadMotionAccelerationChanged", Qt::QueuedConnection, Q_ARG(int, m_tpadMoveSpeed));
    Q_EMIT tpadMoveSpeedChanged(tpadMoveSpeed);
}

void MouseModel::setAccelProfile(bool useAdaptiveProfile)
{
    if (m_accelProfile == useAdaptiveProfile)
        return;

    m_accelProfile = useAdaptiveProfile;

    QMetaObject::invokeMethod(m_worker,"onAccelProfileChanged", Qt::QueuedConnection, Q_ARG(bool, m_accelProfile));
    Q_EMIT accelProfileChanged(useAdaptiveProfile);
}

void MouseModel::setDisTpad(bool disTpad)
{
    if (m_disTpad == disTpad)
        return;

    m_disTpad = disTpad;

    QMetaObject::invokeMethod(m_worker,"onDisTouchPadChanged", Qt::QueuedConnection, Q_ARG(bool, m_disTpad));
    Q_EMIT disTpadChanged(disTpad);
}

void MouseModel::setRedPointMoveSpeed(int redPointMoveSpeed)
{
    if (m_redPointMoveSpeed == redPointMoveSpeed)
        return;

    m_redPointMoveSpeed = redPointMoveSpeed;

    Q_EMIT redPointMoveSpeedChanged(redPointMoveSpeed);
}

void MouseModel::setPalmDetect(bool palmDetect)
{
    if (m_palmDetect == palmDetect)
        return;

    m_palmDetect = palmDetect;

    Q_EMIT palmDetectChanged(palmDetect);
}

void MouseModel::setPalmMinWidth(int palmMinWidth)
{
    if (m_palmMinWidth == palmMinWidth)
        return;

    m_palmMinWidth = palmMinWidth;

    Q_EMIT palmMinWidthChanged(palmMinWidth);
}

void MouseModel::setPalmMinz(int palmMinz)
{
    if (m_palmMinz == palmMinz)
        return;

    m_palmMinz = palmMinz;

    Q_EMIT palmMinzChanged(palmMinz);
}

void MouseModel::setTapClick(bool tapClick)
{
    if (m_tapClick == tapClick)
        return;

    m_tapClick = tapClick;

    QMetaObject::invokeMethod(m_worker,"onTapClick", Qt::QueuedConnection, Q_ARG(bool, m_tapClick));
    Q_EMIT tapClickChanged(tapClick);
}

void MouseModel::setTapEnabled(bool tabEnabled)
{
    if (m_touchpadEnabled == tabEnabled)
        return;

    m_touchpadEnabled = tabEnabled;

    QMetaObject::invokeMethod(m_worker,"onTouchpadEnabledChanged", Qt::QueuedConnection, Q_ARG(bool, m_touchpadEnabled));
    Q_EMIT tapEnabledChanged(tabEnabled);
}

void MouseModel::setScrollSpeed(int speed)
{
    if (m_scrollSpeed == speed)
        return;

    m_scrollSpeed = speed;

    QMetaObject::invokeMethod(m_worker,
                              "onScrollSpeedChanged",
                              Qt::QueuedConnection,
                              Q_ARG(int, m_scrollSpeed));
    Q_EMIT scrollSpeedChanged(speed);
}

void MouseModel::updateGesturesData(const GestureData &gestureData)
{
    GestureModel* gestureModel = NULL;
    if (gestureData.fingersNum() == 3) {

        gestureModel = m_threeFingerGestureModel;
    } else if (gestureData.fingersNum() == 4) {
        gestureModel = m_fourFigerGestureModel;
    } else {
        return;
    }

    if (gestureModel->containsGestures(gestureData.direction(), gestureData.fingersNum())) {
        gestureModel->updateGestureData(gestureData);
    } else {
        GestureData *data = new GestureData(this);
        data->setActionType(gestureData.actionType());
        data->setDirection(gestureData.direction());
        data->setActionName(gestureData.actionName());
        data->setFingersNum(gestureData.fingersNum());
        data->setActionMaps(gestureData.actionMaps());
        gestureModel->addGestureData(data);
    }
}

GestureModel *MouseModel::threeFingerGestureModel() const
{
    return m_threeFingerGestureModel;
}

GestureModel *MouseModel::fourFigerGestureModel() const
{
    return m_fourFigerGestureModel;
}

void MouseModel::setGestures(int fingerNum, int index, QString acitonDec)
{
    GestureModel *gestureModel = NULL;
    if (fingerNum == 4) {
        gestureModel = m_fourFigerGestureModel;
    } else if (fingerNum == 3) {
        gestureModel = m_threeFingerGestureModel;
    } else {
        return;
    }

    GestureData *data = gestureModel->getGestureData(index);
    if (data) {
        QString actionName = data->getActionFromActionDec(acitonDec);
        qDebug() << " setGestures action name : " << actionName << data->actionName();
        if (actionName == data->actionName())
            return;

        updateFigerAniPath(actionName, data);
        Q_EMIT m_worker->requestSetGesture(data->actionType(),
                                           data->direction(),
                                           data->fingersNum(),
                                           actionName);
    }
}

void MouseModel::updateFigerGestureAni(int fingerNum, int index, QString acitonDec)
{
    GestureModel *gestureModel = NULL;
    if (fingerNum == 4) {
        gestureModel = m_fourFigerGestureModel;
    } else if (fingerNum == 3) {
        gestureModel = m_threeFingerGestureModel;
    } else {
        return;
    }
    updateFigerAniPath(acitonDec, gestureModel->getGestureData(index));
}

QString MouseModel::getGestureFingerAniPath() const
{
    return m_gestureFingerAniPath;
}

void MouseModel::setGestureFingerAniPath(const QString &newGestureFingerAniPath)
{
    if (m_gestureFingerAniPath == newGestureFingerAniPath)
        return;
    qDebug() << "setGestureFingerAniPath : " << newGestureFingerAniPath;
    m_gestureFingerAniPath = newGestureFingerAniPath;
    emit gestureFingerAniPathChanged();
}

QString MouseModel::getGestureActionAniPath() const
{
    return m_gestureActionAniPath;
}

void MouseModel::setGestureActionAniPath(const QString &newGestureActionAniPath)
{
    if (m_gestureActionAniPath == newGestureActionAniPath)
        return;

    qDebug() << "setGestureActionAniPath : " << newGestureActionAniPath;
    m_gestureActionAniPath = newGestureActionAniPath;
    emit gestureActionAniPathChanged();
}

Dtk::Gui::DGuiApplicationHelper::ColorType MouseModel::themeType() const
{
    return m_themeType;
}

void MouseModel::updateFigerAniPath(QString actionName, GestureData *data)
{
    if (data == nullptr) {
        data = m_threeFingerGestureModel->getGestureData(0);
    }
    if (actionName == "") {
        actionName = data->actionName();
    }
    QString themeColor = "";
    if (m_themeType == Dtk::Gui::DGuiApplicationHelper::ColorType::DarkType) {
        themeColor = "dark";
    } else if (m_themeType == Dtk::Gui::DGuiApplicationHelper::ColorType::LightType) {
        themeColor = "light";
    }

    QString gestureDirection = data->actionType() == "tap" ? data->actionType() : data->direction();

    QString fingerNum = "";
    if (data->fingersNum() == 4) {
        fingerNum = "Four";
    } else if (data->fingersNum() == 3) {
        fingerNum = "Three";
    }

    setGestureFingerAniPath(QString("qrc:/icons/deepin/builtin/icons/%1/%2_finger_%3_ani.webp")
                                    .arg(themeColor)
                                    .arg(fingerNum)
                                    .arg(gestureDirection));
    setGestureActionAniPath(QString("qrc:/icons/deepin/builtin/icons/%1/%2.webp").arg(themeColor).arg(actionName));
}

void MouseModel::updateFigerAniPath(const Dtk::Gui::DGuiApplicationHelper::ColorType &newThemeType)
{
    QString currentThemeColor = "";
    QString changedThemeColor = "";


    currentThemeColor = m_themeType == Dtk::Gui::DGuiApplicationHelper::ColorType::DarkType ? "dark" : "light";
    changedThemeColor = newThemeType == Dtk::Gui::DGuiApplicationHelper::ColorType::DarkType ? "dark" : "light";

    setGestureFingerAniPath(getGestureFingerAniPath().replace(currentThemeColor,changedThemeColor));
    setGestureActionAniPath(getGestureActionAniPath().replace(currentThemeColor,changedThemeColor));
}

void MouseModel::setThemeType(const Dtk::Gui::DGuiApplicationHelper::ColorType &newThemeType)
{
    if (m_themeType == newThemeType)
        return;
    m_themeType = newThemeType;
    emit themeTypeChanged();
}


DCC_FACTORY_CLASS(MouseModel)

#include "mousemodel.moc"
