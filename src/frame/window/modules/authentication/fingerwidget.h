// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "authenticationinfoitem.h"
#include "widgets/settingsgroup.h"

#include <com_deepin_daemon_fprintd_device.h>

#include <DCommandLinkButton>
#include <QVector>
#include <QWidget>

namespace dcc {
namespace authentication {
class FingerModel;
}
}

namespace DCC_NAMESPACE {
namespace authentication {

class FingerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FingerWidget(QWidget *parent = nullptr);
    ~FingerWidget();
    void setFingerModel(dcc::authentication::FingerModel *model);
    void addFingerButton(const QString &newFingerName);
    bool eventFilter(QObject *watched, QEvent *event)override;

Q_SIGNALS:
    void requestAddThumbs(const QString &name, const QString &thumb);
    void requestDeleteFingerItem(const QString &userName, const QString& finger);
    void requestRenameFingerItem(const QString &userName, const QString& finger, const QString& newName);
    void noticeEnrollCompleted(QString userName);

public Q_SLOTS:
    void onThumbsListChanged(const QStringList &thumbs);

private:
    QString m_currentUserName;
    dcc::authentication::FingerModel *m_model;
    dcc::widgets::SettingsGroup *m_listGrp;
    DCommandLinkButton *m_clearBtn;
    QVector<AuthenticationInfoItem*> m_vecItem;
};

}
}
