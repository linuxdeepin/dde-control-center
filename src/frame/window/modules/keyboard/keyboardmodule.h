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

#include "window/namespace.h"
#include "window/interface/moduleinterface.h"

namespace dcc {
namespace keyboard {
class KeyboardModel;
class ShortcutModel;
class KeyboardWorker;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class GeneralKBSettingWidget;
class KBLayoutSettingWidget;
class SystemLanguageSettingWidget;
class ShortCutSettingWidget;
class KeyboardModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit KeyboardModule(FrameProxyInterface *frame, QObject *parent = nullptr);

    void initialize() override;
    void reset() override;
    QWidget *moduleWidget() override;
    const QString name() const override;
    void contentPopped(QWidget *const w) override;

public Q_SLOTS:
    void showGeneralSetting();
    void showKBLayoutSetting();
    void showSystemLanguageSetting();
    void showShortCutSetting();
    void onSetLocale(const QModelIndex &index);
private:
    GeneralKBSettingWidget *m_generalSettingWidget = nullptr;
    KBLayoutSettingWidget *m_kbLayoutSettingWidget = nullptr;
    SystemLanguageSettingWidget *m_systemLanguageSettingWidget = nullptr;
    ShortCutSettingWidget *m_shortcutSettingWidget = nullptr;

    dcc::keyboard::KeyboardModel  *m_model = nullptr;
    dcc::keyboard::ShortcutModel *m_shortcutModel = nullptr;
    dcc::keyboard::KeyboardWorker *m_work = nullptr;
};
}
}
