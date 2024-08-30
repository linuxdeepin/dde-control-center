//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/authenticationinfoitem.h"
#include "widgets/settingsgroup.h"

#include <DCommandLinkButton>

#include <QVector>
#include <QWidget>

class CharaMangerModel;
class IrisWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IrisWidget(CharaMangerModel *model, QWidget *parent = nullptr);
    ~IrisWidget();

private:
    void initUI();
    void initConnect();

private Q_SLOTS:
    void addIrisButton(const QString &newIrisName);

Q_SIGNALS:
    void requestAddIris(const QString &driverName, const int &charaType, const QString &charaName);
    void requestDeleteIrisItem(const int &charaType, const QString &charaName);
    void requestRenameIrisItem(const int &charaType, const QString &oldName, const QString &newName);
    void noticeEnrollCompleted(const QString &driverName, const int &CharaType);

public Q_SLOTS:
    void onIrisListChanged(const QStringList &irislist);

private:
    CharaMangerModel *m_model;
    DCC_NAMESPACE::SettingsGroup *m_listGrp;
    DTK_WIDGET_NAMESPACE::DCommandLinkButton *m_clearBtn;
    QVector<AuthenticationInfoItem*> m_vecItem;
};
