// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include "interface/moduleinterface.h"

QT_BEGIN_NAMESPACE
#include <QObject>
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class MainWindow;
namespace commoninfo {
class CommonInfoModel;
class CommonInfoWork;
class CommonInfoWidget;
class BootWidget;
class DeveloperModeWidget;
class UserExperienceProgramWidget;
// 以下内容为平板模式做预留
//class TabletModeWidget;

class CommonInfoModule : public QObject
    , public ModuleInterface
{
    Q_OBJECT
public:
    explicit CommonInfoModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~CommonInfoModule();
    void preInitialize(bool sync, FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal);
    //　初始化模块
    virtual void initialize() override;
    // 返回模块名
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    // 当模块第一次被点击时会被调用
    virtual void active() override;
    virtual void deactive() override;
    // 当搜索到相关字段后，load会被调用
    virtual int load(const QString &path) override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

public Q_SLOTS:
    void onShowBootWidget();       // for bootmenu
    void onShowDeveloperWidget();  // for developer mode
    void onShowUEPlanWidget();     // for user exprience program
    void onShowTabletModeWidget(); // for tablet mode

private:
    void initBootWidget();
    void initUeProgramWidget(); // for user experience program
    void initSearchData() override;

private:
    CommonInfoWork *m_commonWork {nullptr};
    CommonInfoModel *m_commonModel {nullptr};
    CommonInfoWidget *m_commonWidget {nullptr};               // main widget
    BootWidget *m_bootWidget {nullptr};                       // for bootmenu
    UserExperienceProgramWidget *m_ueProgramWidget {nullptr}; // for user experience program
    MainWindow *m_pMainWindow;
    // 以下内容为平板模式做预留
    //TabletModeWidget* mTabletModeWidget;
};

} // namespace commoninfo
} // namespace DCC_NAMESPACE
