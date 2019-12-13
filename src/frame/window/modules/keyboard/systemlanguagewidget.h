/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
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
#ifndef SYSTEMLANGUAGEWIDGET_H
#define SYSTEMLANGUAGEWIDGET_H

#include "window/namespace.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace keyboard {
class KeyboardModel;
class MetaData;
}

namespace widgets {
class TranslucentFrame;
class SettingsGroup;
class SettingsHead;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class SystemLanguageSettingWidget;
class CheckItem;
class SystemLanguageWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit SystemLanguageWidget(dcc::keyboard::KeyboardModel *model, QWidget *parent = nullptr);
Q_SIGNALS:
    void languageAdded();
    void onSystemLanguageAdded();
    void delLocalLang(const QString &localLang);
    void setCurLang(const QString &curLang);
public Q_SLOTS:
    void onEdit(bool value);
    void onAddLanguage(const QString &localeLang);
    void onDefault(const QString &curLang);
    void setCurLangChecked(const QString &curLang);
    void onRemoveLang(CheckItem *item);
    void onSetCurLang(int value);
private:
    dcc::keyboard::KeyboardModel *m_model;
    dcc::widgets::SettingsGroup *m_group;
    dcc::widgets::SettingsHead *m_head;
    QMap<QString, CheckItem *> m_localMaps;
    SystemLanguageSettingWidget *m_settingWidget;
    bool m_bEdit{false};
};
}
}
#endif // SYSTEMLANGUAGEWIDGET_H
