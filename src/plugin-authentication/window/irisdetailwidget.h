//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/iris/iriswidget.h"

#include <DTipLabel>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
QT_END_NAMESPACE

class CharaMangerModel;
class IrisDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IrisDetailWidget(CharaMangerModel *model, QWidget *parent = nullptr);
    ~IrisDetailWidget();

private:
    void initIrisShow();
    QString getDisplayPath();

Q_SIGNALS:
    void requestAddIris(const QString &driverName, const int &charaType, const QString &charaName);
    void requestDeleteIrisItem(const int &charaType, const QString &charaName);
    void requestRenameIrisItem(const int &charaType, const QString& oldIrisName, const QString& newIrisName);
    void noticeEnrollCompleted(const QString &driverName, const int &CharaType);
    void requestEntollStart(const QString &driverName, const int &charaType, const QString &charaName);
    void requestStopEnroll();

public Q_SLOTS:
    void onDeviceStatusChanged(bool hasDevice);
    void onShowAddIrisDialog(const QString &driverName, const int &charaType, const QString &charaName);

private:
    CharaMangerModel *m_model;

    QVBoxLayout *m_mainContentLayout;
    IrisWidget *m_irisWidget;
    QLabel *m_pNotDevice;
    DTK_WIDGET_NAMESPACE::DLabel *m_tip;
};

