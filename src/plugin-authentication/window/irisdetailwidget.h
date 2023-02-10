/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>
 *
 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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
#include "widgets/iris/iriswidget.h"

#include <DTipLabel>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
QT_END_NAMESPACE

class CharaMangerModel;

class IrisDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IrisDetailWidget(CharaMangerModel *model, QWidget *parent = nullptr);
    ~IrisDetailWidget();

private:
    void initIrisShow();
    QString getDisplayPath();

Q_SIGNALS:
    void requestAddIris(const QString &driverName, const int &charaType, const QString &charaName);
    void requestDeleteIrisItem(const int &charaType, const QString &charaName);
    void requestRenameIrisItem(const int &charaType,
                               const QString &oldIrisName,
                               const QString &newIrisName);
    void noticeEnrollCompleted(const QString &driverName, const int &CharaType);
    void requestEntollStart(const QString &driverName,
                            const int &charaType,
                            const QString &charaName);
    void requestStopEnroll();

public Q_SLOTS:
    void onDeviceStatusChanged(bool hasDevice);
    void onShowAddIrisDialog(const QString &driverName,
                             const int &charaType,
                             const QString &charaName);

private:
    CharaMangerModel *m_model;

    QVBoxLayout *m_mainContentLayout;
    IrisWidget *m_irisWidget;
    QLabel *m_pNotDevice;
    DTK_WIDGET_NAMESPACE::DLabel *m_tip;
};
