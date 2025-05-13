//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "powerinterface.h"
#include "powermodel.h"
#include "dccfactory.h"
#include "poweroperatormodel.h"
#include <QGuiApplication>
#include <QLoggingCategory>
#include "utils.h"

Q_LOGGING_CATEGORY(DdcPowerInterface, "dcc-power-interface")

PowerInterface::PowerInterface(QObject *parent) 
: QObject(parent)
, m_model(new PowerModel(this))
, m_worker(new PowerWorker(m_model, this))
, m_powerLidClosedOperatorModel(new PowerOperatorModel(this))
, m_powerPressedOperatorModel(new PowerOperatorModel(this))
, m_batteryLidClosedOperatorModel(new PowerOperatorModel(this))
, m_batteryPressedOperatorModel(new PowerOperatorModel(this))
{
    setPowerActionsVisible({m_powerLidClosedOperatorModel, m_batteryLidClosedOperatorModel}, 
        {POT_ShutDown, POT_ShowShutDownInter}, false);

    connect(m_model, &PowerModel::hibernateChanged, this, [this](bool value){
        setPowerActionsVisible(
            {m_powerLidClosedOperatorModel, m_powerPressedOperatorModel, m_batteryLidClosedOperatorModel, m_batteryPressedOperatorModel},
            {POT_Hibernate}, value &&!isVirtualEnvironment());
    });
    connect(m_model, &PowerModel::suspendChanged, this, [this](bool value){
        setPowerActionsVisible(
            {m_powerLidClosedOperatorModel, m_powerPressedOperatorModel, m_batteryLidClosedOperatorModel, m_batteryPressedOperatorModel},
            {POT_Suspend}, value &&!isVirtualEnvironment());
    });
    connect(m_model, &PowerModel::shutdownChanged, this, [this](bool value){
        setPowerActionsVisible(
            {m_powerPressedOperatorModel, m_batteryPressedOperatorModel},
            {POT_ShutDown}, value);
    });

    if (isVirtualEnvironment()) {
        qCInfo(DdcPowerInterface) << "virtual environment, disable suspend and hibernate";
        setPowerActionsVisible(
            {m_powerLidClosedOperatorModel, m_powerPressedOperatorModel, m_batteryLidClosedOperatorModel, m_batteryPressedOperatorModel},
            {POT_Hibernate, POT_Suspend}, false);
    }

    m_worker->active();
}

int PowerInterface::indexByValueOnModel(QAbstractListModel *model, int targetValue)
{
    if (!model) return -1;

    QHash<int, QByteArray> roles = model->roleNames();
    int valueRole = -1;

    for (auto it = roles.constBegin(); it != roles.constEnd(); ++it) {
        if (it.value() == "value") {
            valueRole = it.key();
            break;
        }
    }

    if (valueRole == -1) {
        return -1;
    }

    for (int i = 0; i < model->rowCount(); i++) {
        QModelIndex index = model->index(i, 0);
        QVariant value = model->data(index, valueRole);

        if (value.toInt() == targetValue) {
            return i;
        }
    }

    return -1;
}

int PowerInterface::indexByValueOnMap(const QVariantList& dataMap, int targetValue)
{
    for (int i = 0; i < dataMap.size(); i++) {
        QVariantMap map = dataMap.at(i).toMap();
        if (map.value("value").toInt() == targetValue) {
            return i;
        }
    }

    return -1;
}

QString PowerInterface::platformName()
{
    return qApp->platformName();
}

void PowerInterface::setPowerActionsVisible(QList<PowerOperatorModel*> actionModels, QList<PowerOperatorType> type,  bool visible)
{
    for (auto model : actionModels) {
        if (!model) {
            continue;
        }
        for (auto t : type) {
            model->setVisible(t, visible);
        }
    }
}

DCC_FACTORY_CLASS(PowerInterface)

#include "powerinterface.moc"