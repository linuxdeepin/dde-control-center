//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/finger/fingerwidget.h"
#include "widgets/finger/fingerdisclaimer.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
QT_END_NAMESPACE

class CharaMangerModel;
class FingerWidget;
class FingerDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FingerDetailWidget(QWidget *parent = nullptr);
    ~FingerDetailWidget();

    void setFingerModel(CharaMangerModel *model);

private:
    void initFingerUI();
    void initNotFingerDevice();
    QString getDisplayPath();

Q_SIGNALS:
    void requestAddThumbs(const QString &name, const QString &thumb);
    void requestStopEnroll(const QString &thumb);
    void requestDeleteFingerItem(const QString &userName, const QString &finger);
    void requestRenameFingerItem(const QString &userName, const QString &finger, const QString &newName);
    void noticeEnrollCompleted(QString username);

public Q_SLOTS:
    void showDeviceStatus(bool hasDevice);
    void showAddFingerDialog(const QString &name, const QString &thumb);
    void showFingerDisclaimer(const QString &name, const QString &thumb);

private:
    QString m_currentUserName;
    CharaMangerModel *m_model;
    FingerWidget *m_fingerWidget; //指纹界面
    FingerDisclaimer *m_disclaimer;
};

