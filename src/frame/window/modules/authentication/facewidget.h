// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#pragma once

#include "interface/namespace.h"
#include "authenticationinfoitem.h"
#include "widgets/settingsgroup.h"

#include <DCommandLinkButton>

#include <QVector>
#include <QWidget>
#include <QPointer>

namespace dcc {
namespace authentication {
class CharaMangerModel;
}
}

namespace DCC_NAMESPACE {
namespace authentication {

class FaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FaceWidget(dcc::authentication::CharaMangerModel *model, QWidget *parent = nullptr);
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
    dcc::authentication::CharaMangerModel *m_model;
    dcc::widgets::SettingsGroup *m_listGrp;
    DCommandLinkButton *m_clearBtn;
    QVector<AuthenticationInfoItem*> m_vecItem;
    QPointer<AuthenticationLinkButtonItem> m_addfaceItem;
    QPointer<DCommandLinkButton> m_addBtn;
};

}
}

#endif // FACEWIDGET_H
