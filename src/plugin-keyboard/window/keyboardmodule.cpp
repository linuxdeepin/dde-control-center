//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "keyboardmodule.h"
#include "customcontentdialog.h"
#include "generalkbsettingwidget.h"
#include "kblayoutsettingwidget.h"
#include "keyboardlayoutdialog.h"
#include "shortcutsettingwidget.h"
#ifndef DCC_DISABLE_LANUGAGE
#include "systemlanguagewidget.h"
#include "systemlanguagesettingdialog.h"
#endif
#include "widgets/settingshead.h"
#include "operation/keyboardwork.h"
#include "operation/keyboardmodel.h"
#include "operation/shortcutmodel.h"
#include "customedit.h"
#include "shortcutcontentdialog.h"
#include "widgets/widgetmodule.h"
#include "interface/pagemodule.h"

#include <DFloatingButton>
#include <DIconTheme>

#include <QApplication>

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class KeyboardFloatingButton : public DFloatingButton
{
public:
    explicit KeyboardFloatingButton()
        : DFloatingButton(nullptr) {}
};

QString KeyboardPlugin::name() const
{
    return QStringLiteral("keyboard");
}

ModuleObject *KeyboardPlugin::module()
{
    //一级菜单--键盘与语言
    KeyboardModule *moduleInterface = new KeyboardModule();
    moduleInterface->setName("keyboard");
    moduleInterface->setDisplayName(tr("Keyboard and Language"));
    moduleInterface->setIcon(DIconTheme::findQIcon("dcc_nav_keyboard"));

    //二级菜单--键盘
    ModuleObject *moduleKeyBoard = new PageModule("keyboardGeneral", tr("Keyboard"));

    //为二级菜单-键盘添加children
    GeneralSettingModule *generalSettingModule = new GeneralSettingModule(moduleInterface->model(), moduleInterface->worker());
    generalSettingModule->setName("keyboardSetting");
    generalSettingModule->setDescription(tr("Keyboard Settings"));
    generalSettingModule->addContentText(tr("Keyboard Settings"));
    moduleKeyBoard->appendChild(generalSettingModule);
    //键盘布局
    KBLayoutSettingModule *kBLayoutSettingModule = new KBLayoutSettingModule(moduleInterface->model(), moduleInterface->worker());
    kBLayoutSettingModule->setName("keyboardLayout");
    kBLayoutSettingModule->setDisplayName(tr("keyboard Layout"));
    kBLayoutSettingModule->setDescription(tr("Keyboard Layout"));
    kBLayoutSettingModule->addContentText(tr("Keyboard Layout"));

    moduleKeyBoard->appendChild(kBLayoutSettingModule);
    moduleInterface->appendChild(moduleKeyBoard);

#ifndef DCC_DISABLE_LANUGAGE
    //二级菜单--系统语言
    ModuleObject *moduleSystemLanguageSetting = new PageModule("keyboardLanguage", tr("Language"));
    SystemLanguageSettingModule *systemLanguageSettingModule = new SystemLanguageSettingModule(moduleInterface->model(), moduleInterface->worker());
    moduleSystemLanguageSetting->appendChild(systemLanguageSettingModule);
    moduleInterface->appendChild(moduleSystemLanguageSetting);
#endif

    //二级菜单--快捷键
    ShortCutSettingMenuModule *moduleShortCutSetting = new ShortCutSettingMenuModule("keyboardShortcuts", tr("Shortcuts"));
    ShortCutSettingModule *shortCutSettingModule = new ShortCutSettingModule(moduleInterface->model(), moduleInterface->worker(), moduleInterface->shortcutModel());
    moduleShortCutSetting->appendChild(shortCutSettingModule);
    ModuleObject *customShortcutModule = new WidgetModule<KeyboardFloatingButton>("AddCustomShortCut","AddCustomShortCut",[shortCutSettingModule](KeyboardFloatingButton *customShortcut){
        customShortcut->setIcon(DStyle::SP_IncreaseElement);
        customShortcut->setObjectName("AddCustomShortCut");
        connect(customShortcut, &DFloatingButton::clicked, shortCutSettingModule, &ShortCutSettingModule::onPushCustomShortcut);
    });
    customShortcutModule->setExtra();
    moduleShortCutSetting->appendChild(customShortcutModule);
    moduleInterface->appendChild(moduleShortCutSetting);

    return moduleInterface;
}

QString KeyboardPlugin::location() const
{
    return "15";
}

KeyboardModule::KeyboardModule(QObject *parent)
    : HListModule(parent)
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
    KeyboardLayoutDialog *kbLayoutDialog = new KeyboardLayoutDialog();
    auto dataControll = [ = ](QList<MetaData> datas) {
        for (auto it(datas.begin()); it != datas.end();) {
            const MetaData &data = *it;
            if (kblist.contains(data.key()))
                it = datas.erase(it);
            else
                ++it;
        }

        kbLayoutDialog->setMetaData(datas);
    };

    connect(m_worker, &KeyboardWorker::onDatasChanged, kbLayoutDialog, dataControll);
    connect(m_worker, &KeyboardWorker::onLettersChanged, kbLayoutDialog, &KeyboardLayoutDialog::setLetters);

    dataControll(m_worker->getDatas());
    kbLayoutDialog->setLetters(m_worker->getLetters());

    connect(kbLayoutDialog, &KeyboardLayoutDialog::layoutSelected, m_worker, &KeyboardWorker::addUserLayout);
    kbLayoutDialog->setAttribute(Qt::WA_DeleteOnClose);
    kbLayoutDialog->exec();

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

#ifndef DCC_DISABLE_LANUGAGE
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
    SystemLanguageSettingDialog *systemLanguageSettingDialog = new SystemLanguageSettingDialog(m_model);
    connect(systemLanguageSettingDialog, &SystemLanguageSettingDialog::click, this, &SystemLanguageSettingModule::onAddLocale);
    systemLanguageSettingDialog->setAttribute(Qt::WA_DeleteOnClose);
    systemLanguageSettingDialog->exec();
}

void SystemLanguageSettingModule::onAddLocale(const QModelIndex &index)
{
    QVariant var = index.data(SystemLanguageSettingDialog::KeyRole);
    m_worker->addLang(var.toString());
}
#endif

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
    CustomContentDialog *content = new CustomContentDialog(m_shortcutModel, qobject_cast<QWidget *>(sender()));
    connect(content, &CustomContentDialog::requestUpdateKey, m_worker, &KeyboardWorker::updateKey);
    connect(content, &CustomContentDialog::requestAddKey, m_worker, &KeyboardWorker::addCustomShortcut);
    connect(content, &CustomContentDialog::requestForceSubs, m_worker, &KeyboardWorker::onDisableShortcut);
    content->setAttribute(Qt::WA_DeleteOnClose);
    content->exec();
}

void ShortCutSettingModule::onPushConflict(ShortcutInfo *info, const QString &shortcut)
{
   ShortcutContentDialog *scContentDialog = new ShortcutContentDialog(m_shortcutModel);

    scContentDialog->setInfo(info);
    scContentDialog->setShortcut(shortcut);
    scContentDialog->setBottomTip(m_shortcutModel->getInfo(shortcut));

    connect(scContentDialog, &ShortcutContentDialog::requestSaveShortcut, m_worker, &KeyboardWorker::modifyShortcutEdit);
    connect(scContentDialog, &ShortcutContentDialog::requestUpdateKey, m_worker, &KeyboardWorker::updateKey);
    connect(scContentDialog, &ShortcutContentDialog::requestDisableShortcut, m_worker, &KeyboardWorker::onDisableShortcut);

    scContentDialog->setAttribute(Qt::WA_DeleteOnClose);
    scContentDialog->exec();
}

void ShortCutSettingModule::onShortcutEdit(ShortcutInfo *info)
{
    CustomEdit *customEdit = new CustomEdit(m_shortcutModel, qobject_cast<QWidget *>(sender()));
    customEdit->setVisible(false);
    customEdit->setShortcut(info);

    ShortCutSettingWidget *shortcutWidget = qobject_cast<ShortCutSettingWidget *>(sender());
    SettingsHead *head = shortcutWidget->getHead();

    connect(customEdit, &CustomEdit::requestUpdateKey, m_worker, &KeyboardWorker::updateKey);
    connect(customEdit, &CustomEdit::requestSaveShortcut, head, &SettingsHead::toCancel);
    connect(customEdit, &CustomEdit::requestSaveShortcut, m_worker, &KeyboardWorker::modifyCustomShortcut);

    customEdit->setFocus();
    customEdit->exec();
    head->toCancel();
}

