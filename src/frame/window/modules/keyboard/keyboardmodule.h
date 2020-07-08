
/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

    void initialize() override;
    void reset() override;
    void active() override;
    const QString name() const override;
    const QString displayName() const override;
    void contentPopped(QWidget *const w) override;
    int load(const QString &path) override;
    QStringList availPage() const override;

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
};
}
}
