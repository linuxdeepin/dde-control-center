/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "interface/moduleinterface.h"
#include "../../mainwindow.h"

QT_BEGIN_NAMESPACE
#include <QObject>
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace commoninfo {
class CommonInfoModel;
class CommonInfoWork;
class CommonInfoWidget;
class BootWidget;
class DeveloperModeWidget;
class UserExperienceProgramWidget;
// 以下内容为平板模式做预留
//class TabletModeWidget;

class CommonInfoModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit CommonInfoModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~CommonInfoModule();
    void preInitialize(bool sync);
    //　初始化模块
    virtual void initialize() override;
    // 返回模块名
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    // 当模块第一次被点击时会被调用
    virtual void active() override;
    virtual void deactive() override;
    // 当搜索到相关字段后，load会被调用
    virtual int load(QString path) override;
    QStringList availPage() const override;
public Q_SLOTS:
    void onShowBootWidget(); // for bootmenu
    void onShowDeveloperWidget(); // for developer mode
    void onShowUEPlanWidget(); // for user exprience program
    void onShowTabletModeWidget(); // for tablet mode
private:
    void initBootWidget();
    void initUeProgramWidget(); // for user experience program
private:
    CommonInfoWork *m_commonWork{nullptr};
    CommonInfoModel *m_commonModel{nullptr};
    CommonInfoWidget *m_commonWidget{nullptr}; // main widget
    BootWidget *m_bootWidget{nullptr}; // for bootmenu
    UserExperienceProgramWidget *m_ueProgramWidget{nullptr}; // for user experience program
    MainWindow *m_pMainWindow{nullptr};
    // 以下内容为平板模式做预留
    //TabletModeWidget* mTabletModeWidget;
};

}// namespace commoninfo
}// namespace DCC_NAMESPACE
