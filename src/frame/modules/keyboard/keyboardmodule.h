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

#ifndef KEYBOARDMODULE_H
#define KEYBOARDMODULE_H

#include <QObject>
#include "interfaces/moduleinterface.h"
#include "keyboardwidget.h"
#include "keyboardmodel.h"
#include "shortcutmodel.h"
#include "keyboardwork.h"
#include "langwidget.h"
#include "shortcutwidget.h"
#include "keyboardcontrol.h"
#include "shortcutcontent.h"
#include "customcontent.h"

#ifndef DCC_DISABLE_KBLAYOUT
#include "keyboarddetails.h"
#endif

namespace dcc {
namespace keyboard{
class CustomEdit;
class KeyboardModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit KeyboardModule(FrameProxyInterface* frame, QObject* parent = 0);

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();

    ModuleWidget *moduleWidget();

    const QString name() const;

    void contentPopped(ContentWidget * const w);

    ShortcutInfo* checkConflict(const QString& shortcut, QStringList &list);
    QString converKey(const QString &key);

public Q_SLOTS:
#ifndef DCC_DISABLE_KBLAYOUT
    void onPushKeyboard(const QStringList &kblist);
    void onPushKBDetails();
#endif
#ifndef DCC_DISABLE_LANGUAGE
    void onPushLanguage();
    void onSetLocale(const QModelIndex &index);
#endif
    void onPushShortcut();
    void onPushCustomShortcut();
    void onPushConflict(ShortcutInfo *info, const QString &shortcut);
    void setCurrentLayout(const QString &value);
    void setCapsLock(bool value);

    void onDelay(int value);
    void onSpeed(int value);

    void onShortcutEdit(ShortcutInfo* info);

private Q_SLOTS:
    void onSetFrameAutoHide(const bool autoHide);

private:
    ~KeyboardModule();

private:
    bool m_loaded;
    KeyboardWorker* m_work;
    ShortcutModel* m_shortcutModel;
    KeyboardModel* m_model;
    KeyboardWidget* m_keyboardWidget;
#ifndef DCC_DISABLE_KBLAYOUT
    KeyboardDetails* m_kbDetails;
    KeyboardLayoutWidget* m_kbLayoutWidget;
#endif
    ShortcutWidget* m_shortcutWidget;
#ifndef DCC_DISABLE_LANGUAGE
    LangWidget* m_langWidget;
#endif
    ShortcutContent* m_scContent;
    CustomContent* m_customContent;
    CustomEdit    *m_customEdit;
};
}
}
#endif // KEYBOARDMODULE_H
