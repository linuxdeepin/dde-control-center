// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "charamangerworker.h"

#include <QDBusPendingCall>
#include <QProcess>

#include <unistd.h>

using namespace dcc;
using namespace dcc::authentication;

const QString CharaMangerService("com.deepin.daemon.Authenticate");
const QString FreeDesktopService("org.freedesktop.DBus");

#define INPUT_TIME 30

CharaMangerWorker::CharaMangerWorker(CharaMangerModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_charaMangerInter(new CharaManger(CharaMangerService, "/com/deepin/daemon/Authenticate/CharaManger",
                                  QDBusConnection::systemBus(), this))
    , m_stopTimer(new QTimer(this))
    , m_fileDescriptor(nullptr)
    , m_dbusSenderID("")
    , m_currentInputCharaType(0)
{
    m_charaMangerInter->setSync(false);
    m_dbusSenderID = getControlCenterDbusSender();
    m_stopTimer->setSingleShot(true);
    // 监测录入状态
    connect(m_charaMangerInter, &CharaManger::EnrollStatus, this, &CharaMangerWorker::refreshUserEnrollStatus);

    // 若添加新信息 触发本信号
    connect(m_charaMangerInter, &CharaManger::CharaUpdated, this, &CharaMangerWorker::refreshUserEnrollList);

    // 获取设备信息
    connect(m_charaMangerInter, &CharaManger::DriverInfoChanged, this, &CharaMangerWorker::predefineDriverInfo);
    connect(m_charaMangerInter, &CharaManger::DriverChanged, this, &CharaMangerWorker::refreshDriverInfo);

    // 获取DeviceInfo属性
    QDBusInterface charaManagerInter("com.deepin.daemon.Authenticate",
                             "/com/deepin/daemon/Authenticate/CharaManger",
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::systemBus());
    QDBusPendingCall call = charaManagerInter.asyncCall("Get", "com.deepin.daemon.Authenticate.CharaManger", "DriverInfo");
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
            m_model->onEnrollStatusChanged(dcc::authentication::CharaMangerModel::EnrollFaceStatusType::STATUS_OVERTIME, QString());
        }
        if (m_currentInputCharaType & IRIS_CHARA) {
            m_model->onEnrollIrisStatusChanged(CharaMangerModel::EnrollIrisStatusType::STATUS_IRIS_OVERTIME, QString());
        }

        stopEnroll();
    });
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

QString CharaMangerWorker::getControlCenterDbusSender()
{
    QDBusInterface Interface("org.freedesktop.DBus",
                             "/org/freedesktop/DBus",
                             "org.freedesktop.DBus",
                             QDBusConnection::sessionBus());
    QDBusMessage reply = Interface.call("GetNameOwner", "com.deepin.dde.ControlCenter");
    QString dbusSenderID = reply.arguments()[0].toString();

    return dbusSenderID;
}

void CharaMangerWorker::predefineDriverInfo(const QString &driverInfo)
{
    if (driverInfo.isNull()) {
        // 处理界面显示空设备
        m_model->setFaceDriverVaild(false);
        m_model->setIrisDriverVaild(false);
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
        }

        if (it.value() & IRIS_CHARA) {
            irisDriverNames.append(it.key());
        }

        if (it.value() == 0) {
            // 处理界面显示空设备
            m_model->setFaceDriverVaild(false);
            m_model->setIrisDriverVaild(false);
            return;
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
    call.waitForFinished();

    if (call.isError() || call.value().isEmpty()) {
        qDebug() << "facePrintInter ListFaces call Error or MangerList is empty! " << call.error();
        if (CharaType & FACE_CHARA)
            m_model->setFacesList(QStringList());

        if (CharaType & IRIS_CHARA)
            m_model->setIrisList(QStringList());

        return;
    }

    refreshUserInfo(call.value(), CharaType);
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
    m_model->setAddButtonStatus(false);
    qDebug() << " CharaMangerWorker::entollStart " << driverName << charaType << charaName;
    m_currentInputCharaType = charaType;

    m_fileDescriptor = new QDBusPendingReply<QDBusUnixFileDescriptor>();
      // 设置超时时间为INT_MAX（约等于无限大），阻塞等待后端响应
    m_charaMangerInter->setTimeout(INT_MAX);
    *m_fileDescriptor = m_charaMangerInter->EnrollStart(driverName, charaType, charaName);

    Q_EMIT requestMainWindowEnabled(false);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(*m_fileDescriptor, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        m_model->setAddButtonStatus(true);
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
    // 设置超时时间为-1时，库函数实现为25s
    m_charaMangerInter->setTimeout(-1);
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
    Q_EMIT requestMainWindowEnabled(false);
    auto call = m_charaMangerInter->Delete(charaType, charaName);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this]{
        Q_EMIT requestMainWindowEnabled(true);
        sender()->deleteLater();
    });

    if (call.isError()) {
        qDebug() << "call deleteFaceidItem Error : " << call.error();
    }
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
