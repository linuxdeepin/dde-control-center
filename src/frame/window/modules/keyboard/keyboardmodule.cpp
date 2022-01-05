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
#include "keyboardlayoutwidget.h"
#include "shortcutsettingwidget.h"
#include "systemlanguagewidget.h"
#include "systemlanguagesettingwidget.h"
#include "widgets/settingshead.h"
#include "window/gsettingwatcher.h"
#include "modules/keyboard/keyboardwork.h"
#include "modules/keyboard/keyboardmodel.h"
#include "modules/keyboard/shortcutmodel.h"
#include "modules/keyboard/customedit.h"
#include "modules/keyboard/shortcutcontent.h"
#include "window/mainwindow.h"

using namespace dcc;
using namespace dcc::keyboard;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;

KeyboardModule::KeyboardModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_keyboardWidget(nullptr)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
    GSettingWatcher::instance()->insertState("keyboardGeneral");
    GSettingWatcher::instance()->insertState("keyboardLayout");
    GSettingWatcher::instance()->insertState("keyboardLanguage");
    GSettingWatcher::instance()->insertState("keyboardShortcuts");
}

KeyboardModule::~KeyboardModule()
{
    GSettingWatcher::instance()->erase("keyboardShortcut");
}

void KeyboardModule::preInitialize(bool sync, FrameProxyInterface::PushType)
{
    Q_UNUSED(sync)
    //公共功能可能泄露。在分配指针“m_model”之前未释放它,因此加了这个处理
    if (m_model) {
        delete m_model;
    }
    m_model = new KeyboardModel();
    m_shortcutModel = new ShortcutModel();
    m_work = new KeyboardWorker(m_model);

    addChildPageTrans();
    initSearchData();
}

void KeyboardModule::initialize()
{
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
    m_keyboardWidget->setVisible(false);
    connect(m_keyboardWidget, &KeyboardWidget::showGeneralSetting, this, &KeyboardModule::showGeneralSetting);
    connect(m_keyboardWidget, &KeyboardWidget::showKBLayoutSetting, this, &KeyboardModule::showKBLayoutSetting);
    connect(m_keyboardWidget, &KeyboardWidget::showSystemLanguageSetting, this, &KeyboardModule::showSystemLanguageSetting);
    connect(m_keyboardWidget, &KeyboardWidget::showShortCutSetting, this, &KeyboardModule::showShortCutSetting);
    m_frameProxy->pushWidget(this, m_keyboardWidget);
    m_keyboardWidget->setVisible(true);
    m_keyboardWidget->setDefaultWidget();

    connect(m_keyboardWidget, &KeyboardWidget::requestUpdateSecondMenu, this, [ = ](const bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop) {
            m_frameProxy->popWidget(this);
        }
        m_keyboardWidget->setDefaultWidget();
    });
}

int KeyboardModule::load(const QString &path)
{
    QStringList pathList = path.split("/");
    QString loadPath = pathList.at(0);

    return m_keyboardWidget->showPath(loadPath);
}

QStringList KeyboardModule::availPage() const
{
    QStringList sl;
    sl << "General"
       << "Keyboard Layout"
       << "Keyboard Layout/Add Keyboard Layout"
       << "System Language"
       << "System Language/Add System Language"
       << "Shortcuts"
       << "Shortcuts/Add Custom Shortcut"
       << "Input Methods";
    return sl;
}

void KeyboardModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //keyboard
        m_frameProxy->addChildPageTrans("Shortcuts", tr("Shortcuts"));
        m_frameProxy->addChildPageTrans("Keyboard Layout", tr("Keyboard Layout"));
        m_frameProxy->addChildPageTrans("System Language", tr("System Language"));
    }
}

void KeyboardModule::initSearchData()
{
    const QString& module = displayName();
    const QString& general = tr("General");
    const QString& keyboardLayout = tr("Keyboard Layout");
    const QString& shortcuts = tr("Shortcuts");
    const QString& systemLanguage = tr("System Language");
    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [=](const QString &gsettings, bool state = true) {
        if ("" == gsettings) {
            return false;
        }

        bool ret = false;
        if (state) {
            ret = GSettingWatcher::instance()->get(gsettings).toBool();
        } else {
            ret = GSettingWatcher::instance()->get(gsettings).toString() != "Hidden";
        }

        gsettingsMap.insert(gsettings, ret);
        return ret;
    };

    auto func_general_changed = [ = ]() {
        bool bGeneral = func_is_visible("keyboardGeneral");
        m_frameProxy->setWidgetVisible(module, general, bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Repeat Delay"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Test here"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Repeat Rate"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Numeric Keypad"), bGeneral);
        m_frameProxy->setDetailVisible(module, general, tr("Caps Lock Prompt"), bGeneral);
    };

    auto func_keyboard_changed = [ = ]() {
        bool bKeyLayout = func_is_visible("keyboardLayout");
        m_frameProxy->setWidgetVisible(module, keyboardLayout, bKeyLayout);
        m_frameProxy->setDetailVisible(module, keyboardLayout, tr("Keyboard Layout"), bKeyLayout);
        m_frameProxy->setDetailVisible(module, keyboardLayout, tr("Add Keyboard Layout"), bKeyLayout);
    };

    auto func_shortcuts_changed = [ = ]() {
        bool bShortcuts = func_is_visible("keyboardShortcuts") && func_is_visible("keyboardShortcut", false);
        m_frameProxy->setWidgetVisible(module, shortcuts, bShortcuts);
        m_frameProxy->setDetailVisible(module, shortcuts, tr("Add Custom Shortcut"), bShortcuts);
        m_frameProxy->setDetailVisible(module, shortcuts, tr("Custom Shortcut"), bShortcuts);
        m_frameProxy->setDetailVisible(module, shortcuts, tr("System"), bShortcuts);
        m_frameProxy->setDetailVisible(module, shortcuts, tr("Window"), bShortcuts);
        m_frameProxy->setDetailVisible(module, shortcuts, tr("Workspace"), bShortcuts);
    };

    auto func_syslanguage_changed = [ = ]() {
        bool bKeyboardLanguage = func_is_visible("keyboardLanguage");
        m_frameProxy->setWidgetVisible(module, systemLanguage, bKeyboardLanguage);
        m_frameProxy->setDetailVisible(module, systemLanguage, tr("Add System Language"), bKeyboardLanguage);
        m_frameProxy->setDetailVisible(module, systemLanguage, tr("Language List"), bKeyboardLanguage);
    };

    auto func_process_all = [ = ]() {
        func_general_changed();

        func_keyboard_changed();

        func_shortcuts_changed();

        func_syslanguage_changed();
    };
    //keyboardGeneral, keyboardLayout,keyboardLanguage,keyboardShortcuts
    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        if ("" == gsetting || !gsettingsMap.contains(gsetting)) {
            return;
        }

        if ("keyboardShortcut" == gsetting) {
            if (gsettingsMap.value(gsetting) == (state != "Hidden")) {
                return;
            }
        } else {
            if (gsettingsMap.value(gsetting) == GSettingWatcher::instance()->get(gsetting).toBool()) {
                return;
            }
        }

        if ("keyboardGeneral" == gsetting) {
            func_general_changed();
        } else if ("keyboardLayout" == gsetting) {
            func_keyboard_changed();
        } else if ("keyboardLanguage" == gsetting) {
            func_syslanguage_changed();
        } else if ("keyboardShortcuts" == gsetting || "keyboardShortcut" == gsetting) {
            func_shortcuts_changed();
        } else {
            qDebug() << " not contains the gsettings : " << gsetting << state;
            return;
        }

        qWarning() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}

const QString KeyboardModule::name() const
{
    return QStringLiteral("keyboard");
}

const QString KeyboardModule::displayName() const
{
    return tr("Keyboard and Language");
}

void KeyboardModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void KeyboardModule::showGeneralSetting()
{
    m_generalSettingWidget = new GeneralKBSettingWidget(m_model);
    m_generalSettingWidget->setVisible(false);
    connect(m_generalSettingWidget, &GeneralKBSettingWidget::requestKBDelayChanged, m_work, &KeyboardWorker::setRepeatDelay);
    connect(m_generalSettingWidget, &GeneralKBSettingWidget::requestKBSpeedChanged, m_work, &KeyboardWorker::setRepeatInterval);
    connect(m_generalSettingWidget, &GeneralKBSettingWidget::requestNumLockChanged, m_work, &KeyboardWorker::setNumLock);
    connect(m_generalSettingWidget, &GeneralKBSettingWidget::requestCapsLockChanged, m_work, &KeyboardWorker::setCapsLock);

    m_frameProxy->pushWidget(this, m_generalSettingWidget);
    m_generalSettingWidget->setVisible(true);
}

void KeyboardModule::onPushKeyboard(const QStringList &kblist)
{
    m_work->onPinyin();
    m_kbLayoutWidget = new KeyboardLayoutWidget();
    m_kbLayoutWidget->setVisible(false);
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
    m_kbLayoutWidget->setVisible(true);
}

void KeyboardModule::setCurrentLayout(const QString &value)
{
    m_work->setLayout(m_model->userLayout().key(value));
}

void KeyboardModule::showKBLayoutSetting()
{
    m_work->onRefreshKBLayout();
    m_kbLayoutSettingWidget = new KBLayoutSettingWidget;
    m_kbLayoutSettingWidget->setVisible(false);
    m_kbLayoutSettingWidget->setModel(m_model);

    connect(m_kbLayoutSettingWidget, &KBLayoutSettingWidget::layoutAdded, this, &KeyboardModule::onPushKeyboard);
    connect(m_kbLayoutSettingWidget, &KBLayoutSettingWidget::requestCurLayoutAdded, this, &KeyboardModule::setCurrentLayout);
    connect(m_kbLayoutSettingWidget, &KBLayoutSettingWidget::delUserLayout, m_work, &KeyboardWorker::delUserLayout);

    m_frameProxy->pushWidget(this, m_kbLayoutSettingWidget);
    m_kbLayoutSettingWidget->setVisible(true);
    m_kbLayoutSettingWidget->setFocus();
}

void KeyboardModule::showSystemLanguageSetting()
{
    m_work->refreshLang();
    m_systemLanguageWidget = new SystemLanguageWidget(m_model);
    m_systemLanguageWidget->setVisible(false);
    connect(m_systemLanguageWidget, &SystemLanguageWidget::onSystemLanguageAdded, this, &KeyboardModule::onPushSystemLanguageSetting);
    connect(m_systemLanguageWidget, &SystemLanguageWidget::delLocalLang, m_work, &KeyboardWorker::deleteLang);
    connect(m_systemLanguageWidget, &SystemLanguageWidget::setCurLang, m_work, &KeyboardWorker::setLang);
    connect(m_model, &KeyboardModel::onSetCurLangFinish, m_systemLanguageWidget, &SystemLanguageWidget::onSetCurLang);
    m_frameProxy->pushWidget(this, m_systemLanguageWidget);
    m_systemLanguageWidget->setVisible(true);
}

void KeyboardModule::onAddLocale(const QModelIndex &index)
{
    QVariant var = index.data(SystemLanguageSettingWidget::KeyRole);
    m_work->addLang(var.toString());
}

void KeyboardModule::showShortCutSetting()
{
    m_work->refreshShortcut();
    m_shortcutSettingWidget = new ShortCutSettingWidget(m_shortcutModel);
    GSettingWatcher::instance()->bind("keyboardShortcut", m_shortcutSettingWidget);  // 使用GSettings来控制显示状态
    m_shortcutSettingWidget->setVisible(false);
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
    connect(m_work, &KeyboardWorker::onResetFinished, m_shortcutSettingWidget, &ShortCutSettingWidget::onResetFinished);

    m_frameProxy->pushWidget(this, m_shortcutSettingWidget);
    m_shortcutSettingWidget->setVisible(GSettingWatcher::instance()->getStatus("keyboardShortcut") != "Hidden");
    m_shortcutSettingWidget->setFocus();
}

void KeyboardModule::onPushSystemLanguageSetting()
{
    m_systemLanguageSettingWidget = new SystemLanguageSettingWidget(m_model);
    m_systemLanguageSettingWidget->setVisible(false);
    connect(m_systemLanguageSettingWidget, &SystemLanguageSettingWidget::click, this, &KeyboardModule::onAddLocale);
    connect(m_systemLanguageSettingWidget, &SystemLanguageSettingWidget::back, this, &KeyboardModule::showSystemLanguageSetting);
    m_frameProxy->pushWidget(this, m_systemLanguageSettingWidget);
    m_systemLanguageSettingWidget->setVisible(true);
}

void KeyboardModule::onPushCustomShortcut()
{
    m_customContent = new CustomContent(m_shortcutModel);
    m_customContent->setVisible(false);
    m_customContent->setAccessibleName(tr("Custom Shortcut"));
    connect(m_customContent, &CustomContent::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
    connect(m_customContent, &CustomContent::requestAddKey, m_work, &KeyboardWorker::addCustomShortcut);
    connect(m_customContent, &CustomContent::requestForceSubs, m_work, &KeyboardWorker::onDisableShortcut);
    connect(m_customContent, &CustomContent::back, this, &KeyboardModule::showShortCutSetting);

    m_frameProxy->pushWidget(this, m_customContent);
    m_customContent->setVisible(true);
    m_customContent->setFocus();
}

void KeyboardModule::onPushConflict(ShortcutInfo *info, const QString &shortcut)
{
    m_scContent = new ShortcutContent(m_shortcutModel);
    m_scContent->setVisible(false);

    connect(m_scContent, &ShortcutContent::requestSaveShortcut, m_work, &KeyboardWorker::modifyShortcutEdit);
    connect(m_scContent, &ShortcutContent::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
    connect(m_scContent, &ShortcutContent::requestDisableShortcut, m_work, &KeyboardWorker::onDisableShortcut);
    connect(m_scContent, &ShortcutContent::back, this, &KeyboardModule::showShortCutSetting);

    m_scContent->setInfo(info);
    m_scContent->setShortcut(shortcut);
    m_scContent->setBottomTip(m_shortcutModel->getInfo(shortcut));

    m_frameProxy->pushWidget(this, m_scContent);
    m_scContent->setVisible(true);
    m_scContent->setFocus();
}

void KeyboardModule::onShortcutEdit(ShortcutInfo *info)
{
    m_customEdit = new CustomEdit(m_shortcutModel);
    m_customEdit->setVisible(false);
    m_customEdit->setShortcut(info);

    ShortCutSettingWidget *shortcutWidget = qobject_cast<ShortCutSettingWidget *>(sender());
    SettingsHead *head = shortcutWidget->getHead();

    connect(m_customEdit, &CustomEdit::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
    connect(m_customEdit, &CustomEdit::requestSaveShortcut, head, &SettingsHead::toCancel);
    connect(m_customEdit, &CustomEdit::requestSaveShortcut, m_work, &KeyboardWorker::modifyCustomShortcut);
    connect(m_customEdit, &CustomEdit::back, this, &KeyboardModule::showShortCutSetting);

    m_frameProxy->pushWidget(this, m_customEdit);
    m_customEdit->setVisible(true);
}
