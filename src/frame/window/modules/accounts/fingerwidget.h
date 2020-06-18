/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "interface/namespace.h"
#include "accountfingeitem.h"
#include "modules/accounts/user.h"
#include "modules/accounts/fingermodel.h"
#include "widgets/settingsgroup.h"

#include <com_deepin_daemon_fprintd_device.h>

#include <DCommandLinkButton>

#include <QWidget>

namespace DCC_NAMESPACE {
namespace accounts {

class FingerWidget : public QWidget
{
    Q_OBJECT
public:
    FingerWidget(dcc::accounts::User *user, QWidget *parent = nullptr);
    ~FingerWidget();
    void setFingerModel(dcc::accounts::FingerModel *model);
    void addFingerButton();

Q_SIGNALS:
    void requestAddThumbs(const QString &name, const QString &thumb);
    void requestCleanThumbs(dcc::accounts::User *user);
    void requestDeleteFingerItem(const QString &userName, const QString& finger);
    void requestRenameFingerItem(const QString &userName, const QString& finger, const QString& newName);
    void noticeEnrollCompleted(QString userName);

public Q_SLOTS:
    void onThumbsListChanged(const QStringList &thumbs);

private:
    dcc::accounts::User *m_curUser;
    dcc::accounts::FingerModel *m_model;
    dcc::widgets::SettingsGroup *m_listGrp;
    QString m_notUseThumb;
    DCommandLinkButton *m_clearBtn;
};

}
}
