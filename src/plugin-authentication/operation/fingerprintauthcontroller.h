//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractbiometriccontroller.h"
#include "charamangerworker.h"
#include <QVariantAnimation>
#include <QTimer>

namespace dccV25 {

class FingerprintAuthController : public AbstractBiometricController
{
    Q_OBJECT
    Q_PROPERTY(QString fingertipImagePath MEMBER m_fingertipImagePath NOTIFY fingerTipsChanged)
    Q_PROPERTY(QString fingerTitleTip MEMBER m_fingerTipTitle NOTIFY fingerTipsChanged)
    Q_PROPERTY(QString fingerMsgTip MEMBER m_fingerTipMessage NOTIFY fingerTipsChanged)

public:
    explicit FingerprintAuthController(CharaMangerModel *model, CharaMangerWorker *worker, QObject *parent = nullptr);

public slots:

    // 实现抽象接口
    void startEnroll() override;
    void stopEnroll() override;
    void rename(const QString &oldName, const QString &newName) override;
    void remove(const QString &id) override;
    bool isAvailable() const override;
    QString getTypeName() const override;

    // 公共访问器方法
    QString fingertipImagePath() const { return m_fingertipImagePath; }
    QString fingerTitleTip() const { return m_fingerTipTitle; }
    QString fingerMsgTip() const { return m_fingerTipMessage; }

signals:
    void fingerTipsChanged();

private slots:
    void onThumbsListChanged(const QStringList &thumbs);
    void onFingerEnrollRetry(const QString &title, const QString &msg);
    void onFingerEnrollStagePass(int pro);
    void onFingerEnrollFailed(const QString &title, const QString &msg);
    void onFingerEnrollCompleted();
    void onFingerEnrollDisconnected();
    void onFingerLiftTimerTimeout();
    void onFingerAniValueChanged(const QVariant &pro);

private:
    CharaMangerModel *m_model;
    CharaMangerWorker *m_worker;
    QString m_fingertipImagePath;
    QString m_fingerTipTitle;
    QString m_fingerTipMessage;
    int m_fingerPro = 0;
    bool m_isStageOne = false;
    QTimer *m_fingerLiftTimer = nullptr;
    QVariantAnimation *m_fingerAni = nullptr;
    QString m_themeType;
};

} // namespace dccV25 