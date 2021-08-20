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
    QVector<AccounntFingeItem*> m_vecItem;
};

}
}
