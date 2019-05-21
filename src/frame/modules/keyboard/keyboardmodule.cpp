/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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
#include "keyitem.h"
#include "keyboardmodel.h"
#include "customedit.h"

namespace dcc {
namespace keyboard{

KeyboardModule::KeyboardModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
      m_loaded(false),
      m_keyboardWidget(nullptr),
#ifndef DCC_DISABLE_KBLAYOUT
      m_kbDetails(nullptr),
      m_kbLayoutWidget(nullptr),
#endif
      m_shortcutWidget(nullptr),
#ifndef DCC_DISABLE_LANGUAGE
      m_langWidget(nullptr),
#endif
      m_scContent(nullptr),
      m_customContent(nullptr),
      m_customEdit(nullptr)
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

    connect(m_work, &KeyboardWorker::requestSetAutoHide, this, &KeyboardModule::onSetFrameAutoHide);
}

void KeyboardModule::moduleActive()
{
    m_work->active();
}

void KeyboardModule::moduleDeactive()
{
    m_work->deactive();
}

void KeyboardModule::reset()
{

}

ModuleWidget *KeyboardModule::moduleWidget()
{
    if(!m_keyboardWidget)
    {
        m_keyboardWidget = new KeyboardWidget(m_model);

#ifndef DCC_DISABLE_KBLAYOUT
        connect(m_keyboardWidget, SIGNAL(keyoard()), this, SLOT(onPushKBDetails()));
#endif
#ifndef DCC_DISABLE_LANGUAGE
        connect(m_keyboardWidget, SIGNAL(language()), this, SLOT(onPushLanguage()));
#endif
        connect(m_keyboardWidget, SIGNAL(shortcut()), this, SLOT(onPushShortcut()));
        connect(m_keyboardWidget, &KeyboardWidget::delayChanged, m_work, &KeyboardWorker::setRepeatDelay);
        connect(m_keyboardWidget, &KeyboardWidget::speedChanged, m_work, &KeyboardWorker::setRepeatInterval);
        connect(m_keyboardWidget, &KeyboardWidget::numLockChanged, m_work, &KeyboardWorker::setNumLock);
        connect(m_keyboardWidget, &KeyboardWidget::capsLockChanged, m_work, &KeyboardWorker::setCapsLock);
    }

    return m_keyboardWidget;
}

const QString KeyboardModule::name() const
{
    return QStringLiteral("keyboard");
}

void KeyboardModule::contentPopped(ContentWidget * const w)
{
    if(w == m_shortcutWidget)
        m_shortcutWidget = nullptr;
#ifndef DCC_DISABLE_KBLAYOUT
    else if(w == m_kbDetails)
        m_kbDetails = nullptr;
    else if(w == m_kbLayoutWidget)
        m_kbLayoutWidget = nullptr;
#endif
#ifndef DCC_DISABLE_LANGUAGE
    else if(w == m_langWidget)
        m_langWidget = nullptr;
#endif
    else if(w == m_scContent)
        m_scContent = nullptr;
    else if(w == m_customContent)
        m_customContent = nullptr;

    w->deleteLater();
}

QString KeyboardModule::converKey(const QString &key)
{
     QString converkey = ModelKeycode.value(key);
     return converkey.isEmpty() ? key.toLower() : converkey;
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardModule::onPushKeyboard(const QStringList &kblist)
{
    if(!m_kbLayoutWidget)
    {
        m_work->onPinyin();
        m_kbLayoutWidget = new KeyboardLayoutWidget();

        auto dataControll = [=] (QList<MetaData> datas) {
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
    }
    m_frameProxy->pushWidget(this, m_kbLayoutWidget);
}

void KeyboardModule::onPushKBDetails()
{
    if(!m_kbDetails)
    {
        m_work->onRefreshKBLayout();
        m_kbDetails = new KeyboardDetails();
        m_kbDetails->setModel(m_model);
        connect(m_kbDetails, &KeyboardDetails::layoutAdded, this, &KeyboardModule::onPushKeyboard);
        connect(m_kbDetails, SIGNAL(requestCurLayoutAdded(QString)), this, SLOT(setCurrentLayout(QString)));
        connect(m_kbDetails, SIGNAL(delUserLayout(QString)), m_work, SLOT(delUserLayout(QString)));
        connect(m_kbDetails, &KeyboardDetails::requestSwitchKBLayout, m_work, &KeyboardWorker::onSetSwitchKBLayout);
    }

    m_frameProxy->pushWidget(this, m_kbDetails);
}
#endif

#ifndef DCC_DISABLE_LANGUAGE
void KeyboardModule::onPushLanguage()
{
    if(!m_langWidget)
    {
        m_work->refreshLang();
        m_langWidget = new LangWidget(m_model);
        connect(m_langWidget, SIGNAL(click(QModelIndex)), this, SLOT(onSetLocale(QModelIndex)));
    }

    m_frameProxy->pushWidget(this, m_langWidget);
}

void KeyboardModule::onSetLocale(const QModelIndex &index)
{
    QVariant var = index.data();
    MetaData md = var.value<MetaData>();

    m_work->setLang(md.key());
}
#endif

void KeyboardModule::onPushShortcut()
{
    if(!m_shortcutWidget)
    {
        m_work->refreshShortcut();
        m_shortcutWidget = new ShortcutWidget(m_shortcutModel);
        connect(m_shortcutWidget, &ShortcutWidget::customShortcut, this, &KeyboardModule::onPushCustomShortcut);
        connect(m_shortcutWidget, &ShortcutWidget::delShortcutInfo, m_work, &KeyboardWorker::delShortcut);
        connect(m_shortcutWidget, &ShortcutWidget::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
        connect(m_shortcutWidget, &ShortcutWidget::requestShowConflict, this, &KeyboardModule::onPushConflict);
        connect(m_shortcutWidget, &ShortcutWidget::requestSaveShortcut, m_work, &KeyboardWorker::modifyShortcutEdit);
        connect(m_shortcutWidget, &ShortcutWidget::requestDisableShortcut, m_work, &KeyboardWorker::onDisableShortcut);
        connect(m_shortcutWidget, &ShortcutWidget::shortcutEditChanged, this, &KeyboardModule::onShortcutEdit);
        connect(m_shortcutWidget, &ShortcutWidget::requestReset, m_work, &KeyboardWorker::resetAll);
        connect(m_work, &KeyboardWorker::removed, m_shortcutWidget, &ShortcutWidget::onRemoveItem);
        connect(m_work, SIGNAL(searchChangd(ShortcutInfo*,QString)), m_shortcutWidget, SLOT(onSearchInfo(ShortcutInfo*,QString)));
    }
    m_frameProxy->pushWidget(this, m_shortcutWidget);
}

void KeyboardModule::onPushCustomShortcut()
{
    if(!m_customContent)
    {
        m_customContent = new CustomContent(m_shortcutModel);
        connect(m_customContent, &CustomContent::requestFrameAutoHide, this, &KeyboardModule::onSetFrameAutoHide);
        connect(m_customContent, &CustomContent::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
        connect(m_customContent, &CustomContent::requestAddKey, m_work, &KeyboardWorker::addCustomShortcut);
        connect(m_customContent, &CustomContent::requestForceSubs, m_work, &KeyboardWorker::onDisableShortcut);
    }

    m_frameProxy->pushWidget(this, m_customContent);
}

void KeyboardModule::onPushConflict(ShortcutInfo *info, const QString &shortcut)
{
    if (!m_scContent) {
        m_scContent = new ShortcutContent(m_shortcutModel);

        connect(m_scContent, &ShortcutContent::requestSaveShortcut, m_work, &KeyboardWorker::modifyShortcutEdit);
        connect(m_scContent, &ShortcutContent::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
        connect(m_scContent, &ShortcutContent::requestDisableShortcut, m_work, &KeyboardWorker::onDisableShortcut);

        m_scContent->setInfo(info);
        m_scContent->setShortcut(shortcut);
        m_scContent->setBottomTip(m_shortcutModel->getInfo(shortcut));

        m_frameProxy->pushWidget(this, m_scContent);
    }
}

void KeyboardModule::setCurrentLayout(const QString& value)
{
    m_work->setLayout(m_model->userLayout().key(value));
}

void KeyboardModule::onDelay(int value)
{
    m_work->setRepeatDelay(value);
}

void KeyboardModule::onSpeed(int value)
{
    m_work->setRepeatInterval(value);
}

void KeyboardModule::onShortcutEdit(ShortcutInfo *info)
{
    m_customEdit = new CustomEdit(m_shortcutModel);
    m_customEdit->setShortcut(info);

    ShortcutWidget *shortcutWidget = static_cast<ShortcutWidget*>(sender());
    SettingsHead *head = shortcutWidget->getHead();

    connect(m_customEdit, &CustomEdit::requestFrameAutoHide, this, &KeyboardModule::onSetFrameAutoHide);
    connect(m_customEdit, &CustomEdit::requestUpdateKey, m_work, &KeyboardWorker::updateKey);
    connect(m_customEdit, &CustomEdit::requestSaveShortcut, head, &SettingsHead::toCancel);
    connect(m_customEdit, &CustomEdit::requestSaveShortcut, m_work, &KeyboardWorker::modifyCustomShortcut);

    m_frameProxy->pushWidget(this, m_customEdit);
}

void KeyboardModule::onSetFrameAutoHide(const bool autoHide)
{
    m_frameProxy->setFrameAutoHide(this, autoHide);
}

void KeyboardModule::setCapsLock(bool value)
{
    m_work->setCapsLock(value);
}

KeyboardModule::~KeyboardModule()
{
    m_work->deleteLater();
    m_model->deleteLater();
    m_shortcutModel->deleteLater();
}

}
}
