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
#include "customcontentdialog.h"
#include "generalkbsettingwidget.h"
#include "kblayoutsettingwidget.h"
#include "keyboardlayoutwidget.h"
#include "shortcutsettingwidget.h"
#include "systemlanguagewidget.h"
#include "systemlanguagesettingwidget.h"
#include "widgets/settingshead.h"
#include "src/plugin-keyboard/operation/keyboardwork.h"
#include "src/plugin-keyboard/operation/keyboardmodel.h"
#include "src/plugin-keyboard/window/shortcutmodel.h"
#include "src/plugin-keyboard/window/customedit.h"
#include "src/plugin-keyboard/window/shortcutcontent.h"

#include <DFloatingButton>

#include <QApplication>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

QString KeyboardPlugin::name() const
{
    return QStringLiteral("keyboard");
}

ModuleObject *KeyboardPlugin::module()
{
    //一级菜单--键盘与语言
    KeyboardModule *moduleInterface = new KeyboardModule(this);
    ModuleData *dataRoot = new ModuleData(this);
    dataRoot->Name = tr("keyboard");
    dataRoot->DisplayName = tr("Keyboard and Language");
    dataRoot->Description = tr("键盘和语言");
    dataRoot->Icon = QIcon::fromTheme("dcc_nav_keyboard");
    moduleInterface->setModuleData(dataRoot);
    moduleInterface->setChildType(ModuleObject::ChildType::HList);

    //二级菜单--键盘
    ModuleObject *moduleKeyBoard = new ModuleObject(tr("键盘"), tr("键盘"), this);
    moduleKeyBoard->setChildType(ModuleObject::ChildType::Page);

    //为二级菜单-键盘添加children
    GeneralSettingModule *generalSettingModule = new GeneralSettingModule(moduleInterface->model(), moduleInterface->worker());
    generalSettingModule->moduleData()->Name = tr("键盘设置");
    generalSettingModule->moduleData()->Description = tr("键盘设置");
    generalSettingModule->moduleData()->ContentText.append(tr("键盘设置"));
    moduleKeyBoard->appendChild(generalSettingModule);
    //键盘布局
    KBLayoutSettingModule *kBLayoutSettingModule = new KBLayoutSettingModule(moduleInterface->model(), moduleInterface->worker());
    kBLayoutSettingModule->moduleData()->Name = tr("键盘布局");
    kBLayoutSettingModule->moduleData()->Description = tr("键盘布局");
    kBLayoutSettingModule->moduleData()->ContentText.append(tr("键盘布局"));
    moduleKeyBoard->appendChild(kBLayoutSettingModule);
    moduleInterface->appendChild(moduleKeyBoard);

    //二级菜单--系统语言
    ModuleObject *moduleSystemLanguageSetting = new ModuleObject(tr("系统语言"), tr("系统语言"), this);
    moduleSystemLanguageSetting->setChildType(ModuleObject::ChildType::Page);
    SystemLanguageSettingModule *systemLanguageSettingModule = new SystemLanguageSettingModule(moduleInterface->model(), moduleInterface->worker());
    moduleSystemLanguageSetting->appendChild(systemLanguageSettingModule);
    moduleInterface->appendChild(moduleSystemLanguageSetting);

    //二级菜单--快捷键
    ShortCutSettingMenuModule *moduleShortCutSetting = new ShortCutSettingMenuModule(tr("快捷键"), tr("快捷键"), this);
    moduleShortCutSetting->setChildType(ModuleObject::ChildType::Page);
    ShortCutSettingModule *shortCutSettingModule = new ShortCutSettingModule(moduleInterface->model(), moduleInterface->worker(), moduleInterface->shortcutModel());
    moduleShortCutSetting->appendChild(shortCutSettingModule);
    moduleInterface->appendChild(moduleShortCutSetting);

    connect(moduleShortCutSetting, &ShortCutSettingMenuModule::extraButtonClicked, shortCutSettingModule, &ShortCutSettingModule::onPushCustomShortcut);

    return moduleInterface;
}

KeyboardModule::KeyboardModule(QObject *parent)
    : ModuleObject(parent)
    , m_model(nullptr)
    , m_shortcutModel(nullptr)
    , m_work(nullptr)
{
    //公共功能可能泄露。在分配指针“m_model”之前未释放它,因此加了这个处理
    if (m_model) {
        delete m_model;
    }
    m_model = new KeyboardModel(this);
    m_shortcutModel = new ShortcutModel(this);
    m_work = new KeyboardWorker(m_model,this);

    //注意初始化的顺序和时机
    m_work->setShortcutModel(m_shortcutModel);
}

KeyboardModule::~KeyboardModule()
{
    delete m_shortcutModel;
    m_model->deleteLater();
    m_work->deleteLater();

    m_shortcutModel = nullptr;
    m_model = nullptr;
    m_work = nullptr;
}

void KeyboardModule::active()
{
    m_work->active();
}

//page
QWidget *GeneralSettingModule::page()
{
    GeneralKBSettingWidget *w = new GeneralKBSettingWidget(m_model);
    connect(w, &GeneralKBSettingWidget::requestKBDelayChanged, m_worker, &KeyboardWorker::setRepeatDelay);
    connect(w, &GeneralKBSettingWidget::requestKBSpeedChanged, m_worker, &KeyboardWorker::setRepeatInterval);
    connect(w, &GeneralKBSettingWidget::requestNumLockChanged, m_worker, &KeyboardWorker::setNumLock);
    connect(w, &GeneralKBSettingWidget::requestCapsLockChanged, m_worker, &KeyboardWorker::setCapsLock);
    return w;
}

void KBLayoutSettingModule::onPushKeyboard(const QStringList &kblist)
{
    m_worker->onPinyin();
    KeyboardLayoutWidget *kbLayoutWidget = new KeyboardLayoutWidget();
    auto dataControll = [ = ](QList<MetaData> datas) {
        for (auto it(datas.begin()); it != datas.end();) {
            const MetaData &data = *it;
            if (kblist.contains(data.key()))
                it = datas.erase(it);
            else
                ++it;
        }

        kbLayoutWidget->setMetaData(datas);
    };

    connect(m_worker, &KeyboardWorker::onDatasChanged, kbLayoutWidget, dataControll);
    connect(m_worker, &KeyboardWorker::onLettersChanged, kbLayoutWidget, &KeyboardLayoutWidget::setLetters);

    dataControll(m_worker->getDatas());
    kbLayoutWidget->setLetters(m_worker->getLetters());

    connect(kbLayoutWidget, &KeyboardLayoutWidget::layoutSelected, m_worker, &KeyboardWorker::addUserLayout);
    connect(kbLayoutWidget, &KeyboardLayoutWidget::requestCloseDlg, m_worker, &KeyboardWorker::onRefreshKBLayout);
    kbLayoutWidget->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
    kbLayoutWidget->setFocus();
    kbLayoutWidget->activateWindow();
    kbLayoutWidget->show();
}

void KBLayoutSettingModule::setCurrentLayout(const QString &value)
{
    m_worker->setLayout(m_model->userLayout().key(value));
}

QWidget *KBLayoutSettingModule::page()
{
    m_worker->onRefreshKBLayout();
    KBLayoutSettingWidget *w = new KBLayoutSettingWidget;
    w->setModel(m_model);
    connect(w, &KBLayoutSettingWidget::layoutAdded, this, &KBLayoutSettingModule::onPushKeyboard);
    connect(w, &KBLayoutSettingWidget::requestCurLayoutAdded, this, &KBLayoutSettingModule::setCurrentLayout);
    connect(w, &KBLayoutSettingWidget::delUserLayout, m_worker, &KeyboardWorker::delUserLayout);
    w->setFocus();
    return w;
}

QWidget *SystemLanguageSettingModule::page()
{
    m_worker->refreshLang();
    SystemLanguageWidget *w = new SystemLanguageWidget(m_model);
    w->setVisible(false);
    connect(w, &SystemLanguageWidget::onSystemLanguageAdded, this, &SystemLanguageSettingModule::onPushSystemLanguageSetting);
    connect(w, &SystemLanguageWidget::delLocalLang, m_worker, &KeyboardWorker::deleteLang);
    connect(w, &SystemLanguageWidget::setCurLang, m_worker, &KeyboardWorker::setLang);
    connect(m_model, &KeyboardModel::onSetCurLangFinish, w, &SystemLanguageWidget::onSetCurLang);
    w->setVisible(true);
    return w;
}

void SystemLanguageSettingModule::onPushSystemLanguageSetting()
{
    SystemLanguageSettingWidget *systemLanguageSettingWidget = new SystemLanguageSettingWidget(m_model);
    connect(systemLanguageSettingWidget, &SystemLanguageSettingWidget::click, this, &SystemLanguageSettingModule::onAddLocale);
    connect(systemLanguageSettingWidget, &SystemLanguageSettingWidget::requestCloseDlg, m_worker, &KeyboardWorker::refreshLang);
    systemLanguageSettingWidget->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
    systemLanguageSettingWidget->setFocus();
    systemLanguageSettingWidget->activateWindow();
    systemLanguageSettingWidget->show();
}

void SystemLanguageSettingModule::onAddLocale(const QModelIndex &index)
{
    QVariant var = index.data(SystemLanguageSettingWidget::KeyRole);
    m_worker->addLang(var.toString());
}

QWidget *ShortCutSettingModule::page()
{
    m_worker->refreshShortcut();
    ShortCutSettingWidget *w = new ShortCutSettingWidget(m_shortcutModel);
    connect(w, &ShortCutSettingWidget::delShortcutInfo, m_worker, &KeyboardWorker::delShortcut);
    connect(w, &ShortCutSettingWidget::requestUpdateKey, m_worker, &KeyboardWorker::updateKey);
    connect(w, &ShortCutSettingWidget::requestShowConflict, this, &ShortCutSettingModule::onPushConflict);
    connect(w, &ShortCutSettingWidget::requestSaveShortcut, m_worker, &KeyboardWorker::modifyShortcutEdit);
    connect(w, &ShortCutSettingWidget::requestDisableShortcut, m_worker, &KeyboardWorker::onDisableShortcut);
    connect(w, &ShortCutSettingWidget::shortcutEditChanged, this, &ShortCutSettingModule::onShortcutEdit);
    connect(w, &ShortCutSettingWidget::requestReset, m_worker, &KeyboardWorker::resetAll);
    connect(w, &ShortCutSettingWidget::requestSearch, m_worker, &KeyboardWorker::onSearchShortcuts);
    connect(m_worker, &KeyboardWorker::removed, w, &ShortCutSettingWidget::onRemoveItem);
    connect(m_worker, &KeyboardWorker::searchChangd, w, &ShortCutSettingWidget::onSearchInfo);
    connect(m_worker, &KeyboardWorker::onResetFinished, w, &ShortCutSettingWidget::onResetFinished);
    w->setFocus();
    return w;
}

void ShortCutSettingModule::onPushCustomShortcut()
{
    CustomContentDialog *content = new CustomContentDialog(m_shortcutModel);
    connect(content, &CustomContentDialog::requestUpdateKey, m_worker, &KeyboardWorker::updateKey);
    connect(content, &CustomContentDialog::requestAddKey, m_worker, &KeyboardWorker::addCustomShortcut);
    connect(content, &CustomContentDialog::requestForceSubs, m_worker, &KeyboardWorker::onDisableShortcut);
    connect(content, &CustomContentDialog::requestCloseDlg, m_worker, &KeyboardWorker::refreshShortcut);
    content->setWindowFlags(Qt::Dialog);
    content->setFocus();
    content->activateWindow();
    content->show();
}

void ShortCutSettingModule::onPushConflict(ShortcutInfo *info, const QString &shortcut)
{
   ShortcutContent *scContent = new ShortcutContent(m_shortcutModel);
    connect(scContent, &ShortcutContent::requestSaveShortcut, m_worker, &KeyboardWorker::modifyShortcutEdit);
    connect(scContent, &ShortcutContent::requestUpdateKey, m_worker, &KeyboardWorker::updateKey);
    connect(scContent, &ShortcutContent::requestDisableShortcut, m_worker, &KeyboardWorker::onDisableShortcut);
    connect(scContent, &ShortcutContent::requestCloseDlg, m_worker, &KeyboardWorker::refreshShortcut);

    scContent->setInfo(info);
    scContent->setShortcut(shortcut);
    scContent->setBottomTip(m_shortcutModel->getInfo(shortcut));

    scContent->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
    scContent->setFocus();
    scContent->activateWindow();
    scContent->show();
}

void ShortCutSettingModule::onShortcutEdit(ShortcutInfo *info)
{
    CustomEdit *customEdit = new CustomEdit(m_shortcutModel);
    customEdit->setVisible(false);
    customEdit->setShortcut(info);

    ShortCutSettingWidget *shortcutWidget = qobject_cast<ShortCutSettingWidget *>(sender());
    SettingsHead *head = shortcutWidget->getHead();

    connect(customEdit, &CustomEdit::requestUpdateKey, m_worker, &KeyboardWorker::updateKey);
    connect(customEdit, &CustomEdit::requestSaveShortcut, head, &SettingsHead::toCancel);
    connect(customEdit, &CustomEdit::requestSaveShortcut, m_worker, &KeyboardWorker::modifyCustomShortcut);

    //返回方式的确认
    //connect(customEdit, &CustomEdit::back, this, &KeyboardModule::showShortCutSetting);
    customEdit->setVisible(true);
    customEdit->setFocus();
}


QWidget *ShortCutSettingMenuModule::extraButton()
{
    QWidget *page = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(page);
    layout->setMargin(0);
    layout->setSpacing(0);
    page->setLayout(layout);

    DFloatingButton *customShortcut = new DFloatingButton(DStyle::SP_IncreaseElement, page);
    customShortcut->setObjectName("AddCustomShortCut");
    layout->addWidget(customShortcut, 0, Qt::AlignCenter);
    connect(customShortcut, &DFloatingButton::clicked, this, &ShortCutSettingMenuModule::extraButtonClicked);

    return page;
}
