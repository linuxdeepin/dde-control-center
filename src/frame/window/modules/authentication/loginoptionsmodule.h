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

#include "interface/moduleinterface.h"
#include "../../mainwindow.h"
#include "faceinfodialog.h"
#include "addirisinfodialog.h"

namespace dcc {
namespace authentication {
class FingerModel;
class FingerWorker;
class CharaMangerModel;
class CharaMangerWorker;
}
}

namespace DCC_NAMESPACE {
namespace authentication {
class LoginOptionsWidget;
class LoginOptionsModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit LoginOptionsModule(FrameProxyInterface *frame, QObject *parent = nullptr);

    void initialize() override;
    void reset() override;
    const QString name() const override;
    const QString displayName() const override;
    void showPage(const QString &pageName) override;
    void contentPopped(QWidget *const w) override;
    void active() override;
    int load(const QString &path) override;
    QStringList availPage() const override;

public Q_SLOTS:
    void showFingerPage();
    void showFaceidPage();
    void showIrisPage();
    void onShowAddThumb(const QString &name, const QString &thumb);
    void onShowAddFace(const QString &driverName, const int &charaType, const QString &charaName);
    void onShowAddFaceidVideo(const QString &driverName, const int &charaType, const QString &charaName);
    void onShowAddIris(const QString &driverName, const int &charaType, const QString &charaName);

private:
    ~LoginOptionsModule();
    void onSetMainWindowEnabled(const bool isEnabled);

private:
    MainWindow *m_pMainWindow = nullptr;
    LoginOptionsWidget *m_loginOptionsWidget = nullptr;
    dcc::authentication::FingerModel *m_fingerModel{nullptr};
    dcc::authentication::FingerWorker *m_fingerWorker{nullptr};
    dcc::authentication::CharaMangerModel *m_charaMangerModel{nullptr};
    dcc::authentication::CharaMangerWorker *m_charaMangerWorker{nullptr};

    FaceInfoDialog *m_facedlg;
};

}   // namespace authentication
}   // namespace dccV20
