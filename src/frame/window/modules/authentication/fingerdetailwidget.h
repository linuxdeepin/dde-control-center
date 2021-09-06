/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *             duanhongyu <duanhongyu@uniontech.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>duanhongyu
 *             duanhongyu <duanhongyu@uniontech.com>
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
#include "fingerwidget.h"

#include <QWidget>

namespace dcc {
namespace authentication {
class FingerModel;
}
}

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
QT_END_NAMESPACE


namespace DCC_NAMESPACE {
namespace authentication {
class FingerWidget;
class FingerDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FingerDetailWidget(QWidget *parent = nullptr);
    ~FingerDetailWidget();

    void setFingerModel(dcc::authentication::FingerModel *model);

private:
    void initFingerUI();
    void initNotFingerDevice();
    QString getDisplayPath();

Q_SIGNALS:
    void requestAddThumbs(const QString &name, const QString &thumb);
    void requestDeleteFingerItem(const QString &userName, const QString& finger);
    void requestRenameFingerItem(const QString &userName, const QString& finger, const QString& newName);
    void noticeEnrollCompleted(QString username);

public Q_SLOTS:
    void showDeviceStatus(bool hasDevice);

private:
    QString m_currentUserName;
    QScrollArea *m_scrollArea;
    dcc::authentication::FingerModel *m_model;
    FingerWidget *m_fingerWidget; //指纹界面
};

}
}
