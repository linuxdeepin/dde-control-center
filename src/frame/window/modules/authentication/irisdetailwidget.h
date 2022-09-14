// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "iriswidget.h"

#include <DTipLabel>
#include <QWidget>

namespace dcc {
namespace authentication {
class CharaMangerModel;
}
}

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace authentication {
class IrisDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IrisDetailWidget(dcc::authentication::CharaMangerModel *model, QWidget *parent = nullptr);
    ~IrisDetailWidget();

private:
    void initIrisShow();
    QString getDisplayPath();

Q_SIGNALS:
    void requestAddIris(const QString &driverName, const int &charaType, const QString &charaName);
    void requestDeleteIrisItem(const int &charaType, const QString &charaName);
    void requestRenameIrisItem(const int &charaType, const QString& oldIrisName, const QString& newIrisName);
    void noticeEnrollCompleted(const QString &driverName, const int &CharaType);
    void requestEnrollStart(const QString &driverName, const int &charaType, const QString &charaName);
    void requestStopEnroll();

public Q_SLOTS:
    void onDeviceStatusChanged(bool hasDevice);
    void onShowAddIrisDialog(const QString &driverName, const int &charaType, const QString &charaName);

private:
    dcc::authentication::CharaMangerModel *m_model;

    QVBoxLayout *m_mainContentLayout;
    IrisWidget *m_irisWidget;
    QLabel *m_pNotDevice;
    DLabel *m_tip;
};

}
}

