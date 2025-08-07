//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "operation/charamangermodel.h"
#include <QObject>

namespace dccV25 {

class AbstractBiometricController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CharaMangerModel::AddInfoState addStage READ addStage WRITE setAddStage NOTIFY addStageChanged)

public:
    explicit AbstractBiometricController(QObject *parent = nullptr);
    virtual ~AbstractBiometricController() = default;

    CharaMangerModel::AddInfoState addStage() const { return m_addStage; }
    void setAddStage(CharaMangerModel::AddInfoState stage);

    // 纯虚函数，子类必须实现
    virtual void startEnroll() = 0;
    virtual void stopEnroll() = 0;
    virtual void rename(const QString &oldName, const QString &newName) = 0;
    virtual void remove(const QString &id) = 0;
    
    // 虚函数，子类可以选择性重写
    virtual bool isAvailable() const { return true; }
    virtual QString getTypeName() const = 0;

signals:
    void addStageChanged();
    void enrollCompleted();

protected:
    CharaMangerModel::AddInfoState m_addStage = CharaMangerModel::AddInfoState::StartState;
};

} // namespace dccV25 