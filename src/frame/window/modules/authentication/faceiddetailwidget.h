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
#include "facewidget.h"

#include <DTipLabel>
#include <QWidget>

namespace dcc {
namespace authentication {
class CharaMangerModel;
}
}

namespace DCC_NAMESPACE {
namespace authentication {
class AddFaceInfoDialog;
class FaceInfoDialog;
}
}

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace authentication {
class FaceidDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FaceidDetailWidget(dcc::authentication::CharaMangerModel *model, QWidget *parent = nullptr);
    ~FaceidDetailWidget();

private:
    void initFaceidShow();
    QString getDisplayPath();
    void onShowAddFaceidVideo();

Q_SIGNALS:
    void requestDeleteFaceItem(const int &charaType, const QString &charaName);
    void requestRenameFaceItem(const int &charaType, const QString& oldFaceName, const QString& newFaceName);
    void noticeEnrollCompleted(const QString &driverName, const int &CharaType);
    void requestEntollStart(const QString &driverName, const int &charaType, const QString &charaName);
    void requestStopEnroll();

public Q_SLOTS:
    void onDeviceStatusChanged(bool hasDevice);
    void onShowAddFaceDialog(const QString &driverName, const int &charaType, const QString &charaName);

private Q_SLOTS:
    void onConnectFD(const int &facedf);

private:
    dcc::authentication::CharaMangerModel *m_model;

    QVBoxLayout *m_mainContentLayout;
    FaceWidget *m_faceWidget;
    QLabel *m_pNotDevice;
    DLabel *m_tip;

    AddFaceInfoDialog *m_addFaceInfodlg;
    FaceInfoDialog *m_facedlg;
};

}
}

