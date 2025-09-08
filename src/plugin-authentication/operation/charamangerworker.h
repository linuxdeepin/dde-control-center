//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FACEIDWORKER_H
#define FACEIDWORKER_H

#include "charamangerdbusproxy.h"
#include "charamangermodel.h"

#include <QDBusPendingReply>
#include <QDBusUnixFileDescriptor>
#include <QObject>
#include <QTimer>

class CharaMangerWorker : public QObject
{
    Q_OBJECT
public:
    explicit CharaMangerWorker(CharaMangerModel *model, QObject *parent = nullptr);
    ~CharaMangerWorker();
    void initCharaManger();
    void initFinger();

private:
    /**
     * @brief parseJsonData  仅仅将 mangerInfo 进行数据转化
     * @return 获取后端所有解析后的数据
     */
    QMap<QString, uint> parseDriverNameJsonData(const QString& mangerInfo);
    QStringList parseCharaNameJsonData(const QString& mangerInfo);

Q_SIGNALS:
    void tryStartInputFace(const int &fd);
    void tryStartInputIris(CharaMangerModel::AddInfoState state);
    void requestMainWindowEnabled(const bool isEnabled) const;

public Q_SLOTS:
    void predefineDriverInfo(const QString &driverInfo);

    /**
     * @brief refreshUserEnrollList 获取用户录入信息
     * @param driverName 驱动名称
     * @param CharaType 对应生物特征
     */
    void refreshUserEnrollList(const QString &driverName, const int &CharaType);

    void refreshUserInfo(const QString &EnrollInfo, const int &CharaType);

    void refreshDriverInfo();

    /**
     * @brief entollStart  处理认证接口
     * @param driverName
     * @param charaType
     * @param charaName
     */
    void entollStart(const QString &driverName, const int &charaType, const QString &charaName);

    /**
     * @brief refreshUserEnrollStatus 获取设备录入状态
     * @param senderid dbus对应senderID
     * @param code 对应信息
     * @param codeInfo 信息描述
     */
    void refreshUserEnrollStatus(const QString &senderid, const int &code, const QString &codeInfo);

    void stopEnroll();

    /**
     * @brief deleteFaceidItem 删除对应录入信息
     * @param charaType 唯一值
     * @param charaName 对应名称
     */
    void deleteCharaItem(const int &charaType, const QString &charaName);
    void renameCharaItem(const int &charaType, const QString &oldName, const QString &newName);

    // Fingerprint
    void tryEnroll(const QString &name, const QString &thumb);
    void refreshFingerEnrollList(const QString &id);
    void stopFingerEnroll(const QString& userName);
    void deleteFingerItem(const QString& userName, const QString& finger);
    void renameFingerItem(const QString& userName, const QString& finger, const QString& newName);
    void refreshFingerDriverStatus(const QString &defaultDevice);

private:
    CharaMangerModel *m_model;
    CharaMangerDBusProxy *m_charaMangerInter;

    /**
     * @brief m_stopTimer 开始录入进行计时 1Min后若没有录入成功则失败
     * 注： timer stop时机
     */
    QTimer *m_stopTimer;
    QDBusPendingReply<QDBusUnixFileDescriptor>* m_fileDescriptor;
    /**
     * @brief m_currentInputCharaType  当前录入方式 注：确保唯一性
     */
    int m_currentInputCharaType;
};

#endif // FACEIDWORKER_H
