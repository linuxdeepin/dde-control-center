// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "fingerwidget.h"
#include "fingedisclaimer.h"

#include <QWidget>

namespace dcc {
namespace authentication {
class FingerModel;
}
}

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
QT_END_NAMESPACE


namespace DCC_NAMESPACE {
namespace authentication {
class FingerWidget;
class FingerDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FingerDetailWidget(QWidget *parent = nullptr);
    ~FingerDetailWidget();

    void setFingerModel(dcc::authentication::FingerModel *model);

private:
    void initFingerUI();
    void initNotFingerDevice();
    QString getDisplayPath();
    void closeFingerDisclaimer();

Q_SIGNALS:
    void requestAddThumbs(const QString &name, const QString &thumb);
    void requestStopEnroll(const QString &thumb);
    void requestDeleteFingerItem(const QString &userName, const QString &finger);
    void requestRenameFingerItem(const QString &userName, const QString &finger, const QString &newName);
    void noticeEnrollCompleted(QString username);

public Q_SLOTS:
    void showDeviceStatus(bool hasDevice);
    void showAddFingeDialog(const QString &name, const QString &thumb);
    void showFingeDisclaimer(const QString &name, const QString &thumb);

private:
    QString m_currentUserName;
    dcc::authentication::FingerModel *m_model;
    FingerWidget *m_fingerWidget; //指纹界面
    FingerDisclaimer *m_disclaimer;
};

}
}
