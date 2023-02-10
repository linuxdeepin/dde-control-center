//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/authenticationinfoitem.h"
#include "widgets/settingsgroup.h"
#include <dtkwidget_global.h>

#include <QWidget>
#include <QVector>

DWIDGET_BEGIN_NAMESPACE
class DCommandLinkButton;
DWIDGET_END_NAMESPACE

class CharaMangerModel;
class FingerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FingerWidget(QWidget *parent = nullptr);
    ~FingerWidget();
    void setFingerModel(CharaMangerModel *model);
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
    CharaMangerModel *m_model;
    DCC_NAMESPACE::SettingsGroup *m_listGrp;
    DTK_WIDGET_NAMESPACE::DCommandLinkButton *m_clearBtn;
    QVector<AuthenticationInfoItem*> m_vecItem;
};
