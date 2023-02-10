//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/authenticationinfoitem.h"
#include "widgets/settingsgroup.h"

#include <DCommandLinkButton>

#include <QWidget>
#include <QVector>

class CharaMangerModel;
class FaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FaceWidget(CharaMangerModel *model, QWidget *parent = nullptr);
    ~FaceWidget();

private:
    void initUI();
    void initConnect();

private Q_SLOTS:
    void addFaceButton(const QString &newFaceName);

Q_SIGNALS:
    void requestAddFace(const QString &driverName, const int &charaType, const QString &charaName);
    void requestDeleteFaceItem(const int &charaType, const QString &charaName);
    void requestRenameFaceItem(const int &charaType, const QString &oldName, const QString &newName);
    void noticeEnrollCompleted(const QString &driverName, const int &CharaType);

public Q_SLOTS:
    void onFaceidListChanged(const QStringList &facelist);

private:
    CharaMangerModel *m_model;
    DCC_NAMESPACE::SettingsGroup *m_listGrp;
    DTK_WIDGET_NAMESPACE::DCommandLinkButton *m_clearBtn;
    QVector<AuthenticationInfoItem*> m_vecItem;
};
