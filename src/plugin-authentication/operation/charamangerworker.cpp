//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "charamangerworker.h"

#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusReply>
#include <QProcess>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <unistd.h>

#include <pwd.h>
#include <unistd.h>
#include <libintl.h>

#define INPUT_TIME 30

CharaMangerWorker::CharaMangerWorker(CharaMangerModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_charaMangerInter(new CharaMangerDBusProxy(this))
    , m_stopTimer(new QTimer(this))
    , m_fileDescriptor(nullptr)
    , m_currentInputCharaType(0)
{
    m_stopTimer->setSingleShot(true);
    // 监测录入状态
    connect(m_charaMangerInter, &CharaMangerDBusProxy::EnrollStatusCharaManger, this, &CharaMangerWorker::refreshUserEnrollStatus);

    // 若添加新信息 触发本信号
    connect(m_charaMangerInter, &CharaMangerDBusProxy::CharaUpdated, this, &CharaMangerWorker::refreshUserEnrollList);

    // 获取设备信息
    connect(m_charaMangerInter, &CharaMangerDBusProxy::DriverInfoChanged, this, &CharaMangerWorker::predefineDriverInfo);
    connect(m_charaMangerInter, &CharaMangerDBusProxy::DriverChanged, this, &CharaMangerWorker::refreshDriverInfo);

    //处理指纹后端的录入状态信号
    connect(m_charaMangerInter, &CharaMangerDBusProxy::EnrollStatusFingerprint, m_model, [this](const QString &, int code, const QString &msg) {
        m_model->onFingerEnrollStatusChanged(code, msg);
    });

    connect(m_charaMangerInter, &CharaMangerDBusProxy::DefaultDeviceChanged, this, &CharaMangerWorker::onDefaultDeviceChanged);

    //当前此信号末实现
    connect(m_charaMangerInter, &CharaMangerDBusProxy::Touch, m_model, &CharaMangerModel::onTouch);
    connect(m_charaMangerInter, &CharaMangerDBusProxy::LockedChanged, m_model, &CharaMangerModel::lockedChanged);

    initCharaManger();
    initFinger();
}

CharaMangerWorker::~CharaMangerWorker()
{
    if (m_fileDescriptor) {
        delete m_fileDescriptor;
        m_fileDescriptor = nullptr;
    }
    if (m_stopTimer)
        m_stopTimer->stop();
}

void CharaMangerWorker::initCharaManger()
{
    // 获取DeviceInfo属性
    QDBusInterface charaManagerInter("org.deepin.dde.Authenticate1",
                             "/org/deepin/dde/Authenticate1/CharaManger",
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::systemBus());
    QDBusPendingCall call = charaManagerInter.asyncCall("Get", "org.deepin.dde.Authenticate1.CharaManger", "DriverInfo");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call, watcher] {
        if (!call.isError()) {
            QDBusReply<QDBusVariant> reply = call.reply();
            predefineDriverInfo(reply.value().variant().toString());
        } else {
            qWarning() << "Failed to get driver info: " << call.error().message();
        }
        watcher->deleteLater();
    });

    // 录入时间超时 停止录入
    connect(m_stopTimer, &QTimer::timeout, [this] {
        if (m_currentInputCharaType & FACE_CHARA) {
            m_model->onEnrollStatusChanged(CharaMangerModel::EnrollFaceStatusType::STATUS_OVERTIME, QString());
        }
        if (m_currentInputCharaType & IRIS_CHARA) {
            m_model->onEnrollIrisStatusChanged(CharaMangerModel::EnrollIrisStatusType::STATUS_IRIS_OVERTIME, QString());
        }

        stopEnroll();
    });
}

void CharaMangerWorker::initFinger()
{
    struct passwd *pws;
    QString userId;
    pws = getpwuid(getuid());
    userId = QString(pws->pw_name);

    auto defualtDevice = m_charaMangerInter->defaultDevice();
    m_model->setFingerVaild(!defualtDevice.isEmpty());
    m_model->setUserName(userId);

    if (!defualtDevice.isEmpty()) {
        refreshFingerEnrollList(userId);
    }
}

QMap<QString, uint> CharaMangerWorker::parseDriverNameJsonData(const QString &mangerInfo)
{
    QMap<QString, uint> tmpInfo;
    if (mangerInfo.isEmpty())
        return tmpInfo;

    QJsonDocument doc = QJsonDocument::fromJson(mangerInfo.toUtf8());
    QJsonArray jInfo = doc.array();
    for (QJsonValue jValue : jInfo) {
        QJsonObject jObj = jValue.toObject();
        const QString tmpName = jObj["DriverName"].toString();
        const uint tmpType = static_cast<uint>(jObj["CharaType"].toInt());
        tmpInfo.insert(tmpName, tmpType);
    }
    return tmpInfo;
}

QStringList CharaMangerWorker::parseCharaNameJsonData(const QString &mangerInfo)
{
    QMap<QString, uint64_t> tmpInfo;
    QStringList userInfoList;
    if (mangerInfo.isEmpty())
        return QStringList();

    QJsonDocument doc = QJsonDocument::fromJson(mangerInfo.toUtf8());
    QJsonArray jInfo = doc.array();

    for (QJsonValue jValue : jInfo) {
        QJsonObject jObj = jValue.toObject();
        const QString tmpName = jObj["CharaName"].toString();
        const uint64_t time = static_cast<uint64_t>(jObj["Time"].toInt());
        tmpInfo.insert(tmpName, time);

        QMap<QString, uint64_t>::Iterator it = tmpInfo.begin();
        int index = 0;
        while (it != tmpInfo.end()) {
            if (time > it.value()) {
                index++;
            }
            it++;
        }
        userInfoList.insert(index, tmpName);
    }

    return userInfoList;
}

void CharaMangerWorker::predefineDriverInfo(const QString &driverInfo)
{
    // 处理界面显示空设备
    m_model->setFaceDriverVaild(false);
    m_model->setIrisDriverVaild(false);
    if (driverInfo.isNull()) {
        return;
    }
    QStringList faceDriverNames;
    QStringList irisDriverNames;

    // TODO: 处理设备信息
    QMap<QString, uint> driInfo = parseDriverNameJsonData(driverInfo);
    QMap<QString, uint>::Iterator it;

    qDebug() << "info: " << driInfo.size() << driverInfo;
    // 记录driver信息
    for (it = driInfo.begin(); it != driInfo.end(); ++it) {
        // 可用人脸driverName
        if (it.value() & FACE_CHARA) {
            faceDriverNames.append(it.key());
            m_model->setFaceDriverVaild(false);
        }

        if (it.value() & IRIS_CHARA) {
            irisDriverNames.append(it.key());
            m_model->setIrisDriverVaild(false);
        }
    }

    // 获取用户录入的数据
    if (!faceDriverNames.isEmpty()) {
        m_model->setFaceDriverVaild(true);
        m_model->setFaceDriverName(faceDriverNames.at(0));
        refreshUserEnrollList(faceDriverNames.at(0), FACE_CHARA);
    } else {
        m_model->setFaceDriverVaild(false);
    }

    if (!irisDriverNames.isEmpty()) {
        m_model->setIrisDriverVaild(true);
        m_model->setIrisDriverName(irisDriverNames.at(0));
        refreshUserEnrollList(irisDriverNames.at(0), IRIS_CHARA);
    } else {
        m_model->setIrisDriverVaild(false);
    }
}

void CharaMangerWorker::refreshUserEnrollList(const QString &serviceName, const int &CharaType)
{
    auto call = m_charaMangerInter->List(serviceName, CharaType);
    qDebug() << "CharaManger List : " << call;
    if (call.isEmpty()) {
        qDebug() << "facePrintInter ListFaces call Error or MangerList is empty! ";
        if (CharaType & FACE_CHARA)
            m_model->setFacesList(QStringList());

        if (CharaType & IRIS_CHARA)
            m_model->setIrisList(QStringList());

        return;
    }
    refreshUserInfo(call, CharaType);
}

void CharaMangerWorker::refreshUserInfo(const QString &EnrollInfo, const int &CharaType)
{
    QStringList userInfoList = parseCharaNameJsonData(EnrollInfo);

    if (userInfoList.isEmpty()) {
        qDebug() << "get userInfo error! ";
        m_model->setFacesList(QStringList());
        m_model->setIrisList(QStringList());
        return;
    }

    if (CharaType & FACE_CHARA)
        m_model->setFacesList(userInfoList);
    if (CharaType & IRIS_CHARA)
        m_model->setIrisList(userInfoList);
}

void CharaMangerWorker::refreshDriverInfo()
{
    auto driverInfo = m_charaMangerInter->driverInfo();
    predefineDriverInfo(driverInfo);
}

void CharaMangerWorker::entollStart(const QString &driverName, const int &charaType, const QString &charaName)
{
    qDebug() << " CharaMangerWorker::entollStart " << driverName << charaType << charaName;
    m_currentInputCharaType = charaType;

    m_fileDescriptor = new QDBusPendingReply<QDBusUnixFileDescriptor>();
    *m_fileDescriptor = m_charaMangerInter->EnrollStart(driverName, charaType, charaName);

    Q_EMIT requestMainWindowEnabled(false);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(*m_fileDescriptor, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (m_fileDescriptor->isError()) {
            qDebug() << "get File Descriptor error! " << m_fileDescriptor->error();
        } else {
            m_stopTimer->start(1000 * INPUT_TIME);

            if (charaType & FACE_CHARA) {
                Q_EMIT requestMainWindowEnabled(true);
                m_model->setInputFaceFD(m_fileDescriptor->value().fileDescriptor());
            }

            if (charaType & IRIS_CHARA) {
                Q_EMIT requestMainWindowEnabled(true);
                m_model->setInputIrisFD(CharaMangerModel::AddInfoState::Processing);
            }

        }
        Q_EMIT requestMainWindowEnabled(true);
        watcher->deleteLater();
    });
}

void CharaMangerWorker::refreshUserEnrollStatus(const QString &senderid, const int &code, const QString &codeInfo)
{
    Q_UNUSED(senderid);
    if (m_currentInputCharaType & FACE_CHARA)
        m_model->onEnrollStatusChanged(code, codeInfo);

    if (m_currentInputCharaType & IRIS_CHARA)
        m_model->onEnrollIrisStatusChanged(code, codeInfo);
}

void CharaMangerWorker::stopEnroll()
{
    if (m_stopTimer) {
        m_stopTimer->stop();
    }

    m_currentInputCharaType = -1;
    auto call = m_charaMangerInter->EnrollStop();
    if (call.isError()) {
        qDebug() << "call stop Enroll " << call.error();
    }

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this]{
        if (m_fileDescriptor) {
            delete  m_fileDescriptor;
            m_fileDescriptor = nullptr;
        }

        sender()->deleteLater();
    });
}

void CharaMangerWorker::deleteCharaItem(const int &charaType, const QString &charaName)
{
    m_charaMangerInter->Delete(charaType, charaName);
}

void CharaMangerWorker::renameCharaItem(const int &charaType, const QString &oldName, const QString &newName)
{
    auto call = m_charaMangerInter->Rename(charaType, oldName, newName);
    call.waitForFinished();
    if (call.isError()) {
        qDebug() << "call RenameFinger Error : " << call.error();
        m_model->onRefreshEnrollDate(charaType);
    }
}

void CharaMangerWorker::tryEnroll(const QString &name, const QString &thumb)
{
    m_charaMangerInter->setFingerprintInterTimeout(1000 * 60 * 60);
    auto callClaim = m_charaMangerInter->Claim(name, true);
    callClaim.waitForFinished();

    if (callClaim.isError()) {
        qDebug() << "call Claim Error : " << callClaim.error();
        m_model->refreshEnrollResult(CharaMangerModel::EnrollResult::Enroll_ClaimFailed);
    } else {
        m_charaMangerInter->setFingerprintInterTimeout(-1);
        auto callEnroll =  m_charaMangerInter->Enroll(thumb);

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(callEnroll, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (callEnroll.isError()) {
                qDebug() << "call Enroll Error : " << callEnroll.error();
                m_charaMangerInter->Claim(name, false);
                m_model->refreshEnrollResult(CharaMangerModel::EnrollResult::Enroll_Failed);
            } else {
                Q_EMIT requestMainWindowEnabled(true);
                m_model->refreshEnrollResult(CharaMangerModel::EnrollResult::Enroll_AuthSuccess);
            }
            Q_EMIT requestMainWindowEnabled(true);
            watcher->deleteLater();
        });
    }
    m_charaMangerInter->setFingerprintInterTimeout(-1);

}

void CharaMangerWorker::refreshFingerEnrollList(const QString &id)
{
    QStringList call = m_charaMangerInter->ListFingers(id);
    if (call.isEmpty()) {
        qDebug() << "m_charaMangerInter->ListFingers call Error";
        m_model->setThumbsList(QStringList());
        return;
    } else {
        qDebug() << "m_charaMangerInter->ListFingers";
    }
    m_model->setThumbsList(call);

}

void CharaMangerWorker::stopFingerEnroll(const QString &userName)
{
    qDebug() << "stopEnroll";
    m_charaMangerInter->StopEnroll();
    auto callClaim = m_charaMangerInter->Claim(userName, false);
    callClaim.waitForFinished();
    if (callClaim.isError()) {
        qDebug() << "call Claim Error : " << callClaim.error();
    }
}

void CharaMangerWorker::deleteFingerItem(const QString &userName, const QString &finger)
{
    m_charaMangerInter->setFingerprintInterTimeout(1000 * 60 * 60);
    auto callClaim = m_charaMangerInter->Claim(userName, true);
    callClaim.waitForFinished();
    if (callClaim.isError()) {
        qDebug() << "call Claim Error : " << callClaim.error();
        m_model->refreshEnrollResult(CharaMangerModel::EnrollResult::Enroll_ClaimFailed);
    } else {
        m_charaMangerInter->setFingerprintInterTimeout(-1);
        auto call = m_charaMangerInter->DeleteFinger(userName, finger);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

        QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            refreshFingerEnrollList(userName);
            sender()->deleteLater();
            auto callStopClaim = m_charaMangerInter->Claim(userName, false);
            callStopClaim.waitForFinished();
            if (callStopClaim.isError()) {
                qDebug() << "call stop Claim Error : " << callStopClaim.error();
            }
        });
    }
    m_charaMangerInter->setFingerprintInterTimeout(-1);
}

void CharaMangerWorker::renameFingerItem(const QString &userName, const QString &finger, const QString &newName)
{
    auto call = m_charaMangerInter->RenameFinger(userName, finger, newName);
    call.waitForFinished();
    if (call.isError()) {
        qWarning() << "call RenameFinger Error : " << call.error();
    }
    refreshFingerEnrollList(userName);
}

void CharaMangerWorker::onDefaultDeviceChanged(const QString &device)
{
    qInfo() << "Finger DefaultDeviceChanged: " << device;
    bool isDeviceAvailable = !device.isEmpty();
    m_model->setFingerVaild(isDeviceAvailable);

    // Refresh fingerprint list when device is reconnected
    if (isDeviceAvailable) {
        refreshFingerEnrollList(m_model->userName());
    }
}
