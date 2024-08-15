//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/face/facewidget.h"
#include "widgets/face/addfaceinfodialog.h"
#include "widgets/face/faceinfodialog.h"

#include <DTipLabel>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
QT_END_NAMESPACE

class CharaMangerModel;
class FaceidDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FaceidDetailWidget(CharaMangerModel *model, QWidget *parent = nullptr);
    ~FaceidDetailWidget();

private:
    void initFaceidShow();
    QString getDisplayPath();
    void onShowAddFaceidVideo();

Q_SIGNALS:
    void requestDeleteFaceItem(const int &charaType, const QString &charaName);
    void requestRenameFaceItem(const int &charaType, const QString& oldFaceName, const QString& newFaceName);
    void noticeEnrollCompleted(const QString &driverName, const int &CharaType);
    void requestEntollStart(const QString &driverName, const int &charaType, const QString &charaName);
    void requestStopEnroll();

public Q_SLOTS:
    void onDeviceStatusChanged(bool hasDevice);
    void onShowAddFaceDialog(const QString &driverName, const int &charaType, const QString &charaName);

private Q_SLOTS:
    void onConnectFD(const int &facedf);

private:
    CharaMangerModel *m_model;

    QVBoxLayout *m_mainContentLayout;
    FaceWidget *m_faceWidget;
    QLabel *m_pNotDevice;
    DTK_WIDGET_NAMESPACE::DLabel *m_tip;

    AddFaceInfoDialog *m_addFaceInfodlg;
    FaceInfoDialog *m_facedlg;
};

