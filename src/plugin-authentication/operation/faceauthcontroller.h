//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "abstractbiometriccontroller.h"
#include "charamangerworker.h"

// 前向声明
struct DA_img;

namespace dccV25 {

class FaceAuthController : public AbstractBiometricController
{
    Q_OBJECT
    Q_PROPERTY(QString faceImgContent READ faceImgContent NOTIFY faceImgContentChanged FINAL)
    Q_PROPERTY(bool enrollFaceSuccess READ enrollFaceSuccess NOTIFY enrollFaceSuccessChanged FINAL)
    Q_PROPERTY(QString enrollFaceTips READ enrollFaceTips NOTIFY enrollFaceTipsChanged FINAL)

public:
    explicit FaceAuthController(CharaMangerModel *model, CharaMangerWorker *worker, QObject *parent = nullptr);

public slots:

    // 实现抽象接口
    void startEnroll() override;
    void stopEnroll() override;
    void rename(const QString &oldName, const QString &newName) override;
    void remove(const QString &id) override;
    bool isAvailable() const override;
    QString getTypeName() const override;

    // 人脸特有的属性和方法
    QString faceImgContent() const;
    bool enrollFaceSuccess() const;
    QString enrollFaceTips() const;

signals:
    void faceImgContentChanged();
    void enrollFaceSuccessChanged();
    void enrollFaceTipsChanged();

private slots:
    void onEnrollStatusTips(const QString &tips);
    void onEnrollInfoState(CharaMangerModel::AddInfoState state, const QString &tips);
    void onTryStartInputFace(int fd);

private:
    static void updateFaceImgContent(void *const context, const DA_img *const img);

private:
    CharaMangerModel *m_model;
    CharaMangerWorker *m_worker;
    QString m_faceImgContent;
    QString m_enrollFaceTips;
    bool m_enrollFaceSuccess = false;
    bool m_enrollFaceInProgress = false;
    QString m_themeType;
};

} // namespace dccV25 