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
#include "generalkbsettingwidget.h"
#include "kblayoutsettingwidget.h"
#include "shortcutsettingwidget.h"
#include "systemlanguagesettingwidget.h"
#include "modules/keyboard/keyboardwork.h"
#include "modules/keyboard/keyboardmodel.h"
#include "modules/keyboard/shortcutmodel.h"

using namespace dcc;
using namespace dcc::keyboard;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;

KeyboardModule::KeyboardModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
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

QWidget *KeyboardModule::moduleWidget()
{
    KeyboardWidget *keyboardWidget = new KeyboardWidget;
    connect(keyboardWidget, &KeyboardWidget::showGeneralSetting, this, &KeyboardModule::showGeneralSetting);
    connect(keyboardWidget, &KeyboardWidget::showKBLayoutSetting, this, &KeyboardModule::showKBLayoutSetting);
    connect(keyboardWidget, &KeyboardWidget::showSystemLanguageSetting, this, &KeyboardModule::showSystemLanguageSetting);
    connect(keyboardWidget, &KeyboardWidget::showShortCutSetting, this, &KeyboardModule::showShortCutSetting);

    return keyboardWidget;
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

void KeyboardModule::showKBLayoutSetting()
{
    m_kbLayoutSettingWidget = new KBLayoutSettingWidget;
}

void KeyboardModule::showSystemLanguageSetting()
{
    m_work->refreshLang();
    m_systemLanguageSettingWidget = new SystemLanguageSettingWidget(m_model);
    connect(m_systemLanguageSettingWidget, SIGNAL(click(QModelIndex)), this, SLOT(onSetLocale(QModelIndex)));

    m_frameProxy->pushWidget(this, m_systemLanguageSettingWidget);
}

void KeyboardModule::onSetLocale(const QModelIndex &index)
{
    QVariant var = index.data();
    MetaData md = var.value<MetaData>();
    m_work->setLang(md.key());
}

void KeyboardModule::showShortCutSetting()
{
    m_shortcutSettingWidget = new ShortCutSettingWidget;
}
