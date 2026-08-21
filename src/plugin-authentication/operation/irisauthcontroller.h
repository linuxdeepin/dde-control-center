//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractbiometriccontroller.h"
#include "charamangerworker.h"
#include <qtmetamacros.h>

namespace dccV25 {

class IrisAuthController : public AbstractBiometricController
{
    Q_OBJECT
    Q_PROPERTY(QString irisImgContent READ irisImgContent NOTIFY irisImgContentChanged FINAL)
    Q_PROPERTY(bool enrollIrisSuccess READ enrollIrisSuccess NOTIFY enrollIrisSuccessChanged FINAL)
    Q_PROPERTY(QString enrollIrisTips READ enrollIrisTips NOTIFY enrollIrisTipsChanged FINAL)

public:
    explicit IrisAuthController(CharaMangerModel *model, CharaMangerWorker *worker, QObject *parent = nullptr);

public slots:

    // 实现抽象接口
    void startEnroll() override;
    void stopEnroll() override;
    void rename(const QString &oldName, const QString &newName) override;
    void remove(const QString &id) override;
    bool isAvailable() const override;
    QString getTypeName() const override;

    // 虹膜特有的属性和方法（预留）
    QString irisImgContent() const;
    bool enrollIrisSuccess() const;
    QString enrollIrisTips() const;

signals:
    void irisImgContentChanged();
    void enrollIrisSuccessChanged();
    void enrollIrisTipsChanged();

private slots:
    void onEnrollIrisStatusTips(const QString &tips);
    void onEnrollIrisInfoState(CharaMangerModel::AddInfoState state, const QString &tips);
    void onTryStartInputIris(CharaMangerModel::AddInfoState state);

private:
    CharaMangerModel *m_model;
    CharaMangerWorker *m_worker;
    QString m_irisImgContent;
    QString m_enrollIrisTips;
    bool m_enrollIrisSuccess = false;
    bool m_enrollIrisInProgress = false;
    QString m_themeType;
};

} // namespace dccV25 