// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "interface/moduleinterface.h"

#include <QObject>

namespace dcc {
namespace keyboard {
class KeyboardModel;
class ShortcutModel;
class KeyboardWorker;
class KeyboardLayoutWidget;
struct ShortcutInfo;
class CustomEdit;
class ShortcutContent;
}
}

namespace DCC_NAMESPACE {
class MainWindow;
namespace keyboard {
class KeyboardWidget;
class GeneralKBSettingWidget;
class KBLayoutSettingWidget;
class SystemLanguageWidget;
class SystemLanguageSettingWidget;
class ShortCutSettingWidget;
class CustomContent;
class KeyboardModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit KeyboardModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~KeyboardModule();

    void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    void initialize() override;
    void reset() override;
    void active() override;
    const QString name() const override;
    const QString displayName() const override;
    void contentPopped(QWidget *const w) override;
    int load(const QString &path) override;
    QStringList availPage() const override;
    void addChildPageTrans() const override;

protected:
    void initSearchData() override;

public Q_SLOTS:
    void showGeneralSetting();
    void showKBLayoutSetting();
    void showSystemLanguageSetting();
    void showShortCutSetting();
    void onAddLocale(const QModelIndex &index);
    void onPushKeyboard(const QStringList &kblist);
    void onPushSystemLanguageSetting();
    void setCurrentLayout(const QString &value);
    void onPushCustomShortcut();
    void onPushConflict(dcc::keyboard::ShortcutInfo *info, const QString &shortcut);
    void onShortcutEdit(dcc::keyboard::ShortcutInfo *info);

private:
    KeyboardWidget *m_keyboardWidget = nullptr;
    GeneralKBSettingWidget *m_generalSettingWidget = nullptr;
    KBLayoutSettingWidget *m_kbLayoutSettingWidget = nullptr;
    SystemLanguageWidget *m_systemLanguageWidget = nullptr;
    SystemLanguageSettingWidget *m_systemLanguageSettingWidget = nullptr;
    ShortCutSettingWidget *m_shortcutSettingWidget = nullptr;

    dcc::keyboard::KeyboardLayoutWidget *m_kbLayoutWidget = nullptr;
    dcc::keyboard::KeyboardModel  *m_model = nullptr;
    dcc::keyboard::ShortcutModel *m_shortcutModel = nullptr;
    dcc::keyboard::KeyboardWorker *m_work = nullptr;
    CustomContent *m_customContent = nullptr;
    dcc::keyboard::CustomEdit    *m_customEdit = nullptr;
    dcc::keyboard::ShortcutContent *m_scContent = nullptr;
    MainWindow *m_pMainWindow;
};
}
}
