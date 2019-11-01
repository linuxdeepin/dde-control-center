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

#include "keyboardmodule.h"
#include "keyboardwidget.h"
#include "customcontent.h"
#include "generalkbsettingwidget.h"
#include "kblayoutsettingwidget.h"
#include "shortcutsettingwidget.h"
#include "systemlanguagewidget.h"
#include "systemlanguagesettingwidget.h"
#include "widgets/settingshead.h"
#include "modules/keyboard/keyboardwork.h"
#include "modules/keyboard/keyboardmodel.h"
#include "modules/keyboard/shortcutmodel.h"
#include "modules/keyboard/customedit.h"
#include "modules/keyboard/shortcutcontent.h"
#include "modules/keyboard/keyboardlayoutwidget.h"

using namespace dcc;
using namespace dcc::keyboard;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;

KeyboardModule::KeyboardModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_keyboardWidget(nullptr)
{
}

void KeyboardModule::initialize()
{
    m_model = new KeyboardModel();
    m_shortcutModel = new ShortcutModel();
    m_work = new KeyboardWorker(m_model);
    m_work->setShortcutModel(m_shortcutModel);

    m_model->moveToThread(qApp->thread());
    m_shortcutModel->moveToThread(qApp->thread());
    m_work->moveToThread(qApp->thread());
}

void KeyboardModule::reset()
{

}

void KeyboardModule::active()
{
    m_work->active();
    m_keyboardWidget = new KeyboardWidget;
    connect(m_keyboardWidget, &KeyboardWidget::showGeneralSetting, this, &KeyboardModule::showGeneralSetting);
    connect(m_keyboardWidget, &KeyboardWidget::showKBLayoutSetting, this, &KeyboardModule::showKBLayoutSetting);
    connect(m_keyboardWidget, &KeyboardWidget::showSystemLanguageSetting, this, &KeyboardModule::showSystemLanguageSetting);
    connect(m_keyboardWidget, &KeyboardWidget::showShortCutSetting, this, &KeyboardModule::showShortCutSetting);
    m_frameProxy->pushWidget(this, m_keyboardWidget);
    showGeneralSetting();
}

void KeyboardModule::load(QString path)
{
    QStringList pathList = path.split("/");
    QString loadPath = pathList.at(0);
    if (loadPath == QStringLiteral("General")) {
        m_keyboardWidget->initSetting(0);
    } else if (loadPath == QStringLiteral("Keyboard Layout")) {
        m_keyboardWidget->initSetting(1);
    } else if (loadPath == QStringLiteral("System Language")) {
        m_keyboardWidget->initSetting(2);
    } else if (loadPath == QStringLiteral("Shortcuts")) {
        if ((pathList.length() > 1) && (pathList.at(1) == QStringLiteral("Custom Shortcut"))) {
            showShortCutSetting();
            m_shortcutSettingWidget->showCustomShotcut();
        } else {
            m_keyboardWidget->initSetting(3);
        }
    }
}

const QString KeyboardModule::name() const
{
    return QStringLiteral("keyboard");
}

void KeyboardModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void KeyboardModule::showGeneralSetting()
{
    m_generalSettingWidget = new GeneralKBSettingWidget(m_model);

    connect(m_generalSettingWidget, &GeneralKBSettingWidget::requestKBDelayChanged, m_work, &KeyboardWorker::setRepeatDelay);
    connect(m_generalSettingWidget, &GeneralKBSettingWidget::requestKBSpeedChanged, m_work, &KeyboardWorker::setRepeatInterval);
    connect(m_generalSettingWidget, &GeneralKBSettingWidget::requestNumLockChanged, m_work, &KeyboardWorker::setNumLock);
    connect(m_generalSettingWidget, &GeneralKBSettingWidget::requestCapsLockChanged, m_work, &KeyboardWorker::setCapsLock);

    m_frameProxy->pushWidget(this, m_generalSettingWidget);
}

void KeyboardModule::onPushKeyboard(const QStringList &kblist)
{
    m_work->onPinyin();
    m_kbLayoutWidget = new KeyboardLayoutWidget();

    auto dataControll = [ = ](QList<MetaData> datas) {
        for (auto it(datas.begin()); it != datas.end();) {
            const MetaData &data = *it;
            if (kblist.contains(data.key()))
                it = datas.erase(it);
            else
                ++it;
        }

        m_kbLayoutWidget->setMetaData(datas);
    };

    connect(m_work, &KeyboardWorker::onDatasChanged, m_kbLayoutWidget, dataControll);
    connect(m_work, &KeyboardWorker::onLettersChanged, m_kbLayoutWidget, &KeyboardLayoutWidget::setLetters);

    dataControll(m_work->getDatas());
    m_kbLayoutWidget->setLetters(m_work->getLetters());

    connect(m_kbLayoutWidget, &KeyboardLayoutWidget::layoutSelected, m_work, &KeyboardWorker::addUserLayout);
    connect(m_kbLayoutWidget, &KeyboardLayoutWidget::back, this, &KeyboardModule::showKBLayoutSetting);

    m_frameProxy->pushWidget(this, m_kbLayoutWidget);
}

void KeyboardModule::setCurrentLayout(const QString &value)
{
    m_work->setLayout(m_model->userLayout().key(value));
}

void KeyboardModule::showKBLayoutSetting()
{
    m_work->onRefreshKBLayout();
    m_kbLayoutSettingWidget = new KBLayoutSettingWidget;
    m_kbLayoutSettingWidget->setModel(m_model);

    connect(m_kbLayoutSettingWidget, &KBLayoutSettingWidget::layoutAdded, this, &KeyboardModule::onPushKeyboard);
    connect(m_kbLayoutSettingWidget, &KBLayoutSettingWidget::requestCurLayoutAdded, this, &KeyboardModule::setCurrentLayout);
    connect(m_kbLayoutSettingWidget, &KBLayoutSettingWidget::delUserLayout, m_work, &KeyboardWorker::delUserLayout);
    connect(m_kbLayoutSettingWidget, &KBLayoutSettingWidget::requestSwitchKBLayout, m_work, &KeyboardWorker::onSetSwitchKBLayout);
    connect(m_kbLayoutSettingWidget, &KBLayoutSettingWidget::onSwitchKBLayoutScope, m_work, &KeyboardWorker::setLayoutScope);

    m_frameProxy->pushWidget(this, m_kbLayoutSettingWidget);
}

void KeyboardModule::showSystemLanguageSetting()
{
    m_work->refreshLang();
    m_systemLanguageWidget = new SystemLanguageWidget(m_model);
    connect(m_systemLanguageWidget, &SystemLanguageWidget::onSystemLanguageAdded, this, &KeyboardModule::onPushSystemLanguageSetting);
    connect(m_systemLanguageWidget, &SystemLanguageWidget::delLocalLang, m_work, &KeyboardWorker::deleteLang);
    connect(m_systemLanguageWidget, &SystemLanguageWidget::setCurLang, m_work, &KeyboardWorker::setLang);
    m_frameProxy->pushWidget(this, m_systemLanguageWidget);
}

void KeyboardModule::onAddLocale(const QModelIndex &index)
{
    QVariant var = index.data();
    MetaData md = var.value<MetaData>();
    m_work->addLang(md.key());
}

void KeyboardModule::showShortCutSetting()
{
    m_work->refreshShortcut();
    m_shortcutSettingWidget = new ShortCutSettingWidget(m_shortcutModel);
    connect(m_shortcutSettingWidget, &ShortCutSettingWidget::customShortcut, this, &KeyboardModule::onPushCustomShortcut);
    connect(m_shortcutSettingWidget, &ShortCutSettingWidget::delShortcutInfo, m_work, &KeyboardWorker::delShortcut);
    connect(m_shortcutSettingWidget, &ShortCutSettingWidget::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
    connect(m_shortcutSettingWidget, &ShortCutSettingWidget::requestShowConflict, this, &KeyboardModule::onPushConflict);
    connect(m_shortcutSettingWidget, &ShortCutSettingWidget::requestSaveShortcut, m_work, &KeyboardWorker::modifyShortcutEdit);
    connect(m_shortcutSettingWidget, &ShortCutSettingWidget::requestDisableShortcut, m_work, &KeyboardWorker::onDisableShortcut);
    connect(m_shortcutSettingWidget, &ShortCutSettingWidget::shortcutEditChanged, this, &KeyboardModule::onShortcutEdit);
    connect(m_shortcutSettingWidget, &ShortCutSettingWidget::requestReset, m_work, &KeyboardWorker::resetAll);
    connect(m_shortcutSettingWidget, &ShortCutSettingWidget::requestSearch, m_work, &KeyboardWorker::onSearchShortcuts);
    connect(m_work, &KeyboardWorker::removed, m_shortcutSettingWidget, &ShortCutSettingWidget::onRemoveItem);
    connect(m_work, &KeyboardWorker::searchChangd, m_shortcutSettingWidget, &ShortCutSettingWidget::onSearchInfo);

    m_frameProxy->pushWidget(this, m_shortcutSettingWidget);
}

void KeyboardModule::onPushSystemLanguageSetting()
{
    m_systemLanguageSettingWidget = new SystemLanguageSettingWidget(m_model);
    connect(m_systemLanguageSettingWidget, &SystemLanguageSettingWidget::click, this, &KeyboardModule::onAddLocale);
    connect(m_systemLanguageSettingWidget, &SystemLanguageSettingWidget::back, this, &KeyboardModule::showSystemLanguageSetting);
    m_frameProxy->pushWidget(this, m_systemLanguageSettingWidget);
}

void KeyboardModule::onPushCustomShortcut()
{
    m_customContent = new CustomContent(m_shortcutModel);
    m_customContent->setAccessibleName(tr("Custom Shortcut"));
    connect(m_customContent, &CustomContent::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
    connect(m_customContent, &CustomContent::requestAddKey, m_work, &KeyboardWorker::addCustomShortcut);
    connect(m_customContent, &CustomContent::requestForceSubs, m_work, &KeyboardWorker::onDisableShortcut);
    connect(m_customContent, &CustomContent::back, this, &KeyboardModule::showShortCutSetting);

    m_frameProxy->pushWidget(this, m_customContent);
}

void KeyboardModule::onPushConflict(ShortcutInfo *info, const QString &shortcut)
{
    m_scContent = new ShortcutContent(m_shortcutModel);

    connect(m_scContent, &ShortcutContent::requestSaveShortcut, m_work, &KeyboardWorker::modifyShortcutEdit);
    connect(m_scContent, &ShortcutContent::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
    connect(m_scContent, &ShortcutContent::requestDisableShortcut, m_work, &KeyboardWorker::onDisableShortcut);
    connect(m_scContent, &ShortcutContent::back, this, &KeyboardModule::showShortCutSetting);

    m_scContent->setInfo(info);
    m_scContent->setShortcut(shortcut);
    m_scContent->setBottomTip(m_shortcutModel->getInfo(shortcut));

    m_frameProxy->pushWidget(this, m_scContent);
}

void KeyboardModule::onShortcutEdit(ShortcutInfo *info)
{
    m_customEdit = new CustomEdit(m_shortcutModel);
    m_customEdit->setShortcut(info);

    ShortCutSettingWidget *shortcutWidget = qobject_cast<ShortCutSettingWidget *>(sender());
    SettingsHead *head = shortcutWidget->getHead();

    connect(m_customEdit, &CustomEdit::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
    connect(m_customEdit, &CustomEdit::requestSaveShortcut, head, &SettingsHead::toCancel);
    connect(m_customEdit, &CustomEdit::requestSaveShortcut, m_work, &KeyboardWorker::modifyCustomShortcut);
    connect(m_customEdit, &CustomEdit::back, this, &KeyboardModule::showShortCutSetting);

    m_frameProxy->pushWidget(this, m_customEdit);
}
