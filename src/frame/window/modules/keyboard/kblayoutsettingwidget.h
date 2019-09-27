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
#ifndef KBLAYOUTSETTINGWIDGET_H
#define KBLAYOUTSETTINGWIDGET_H

#include "window/namespace.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace keyboard {
class KeyboardModel;
class MetaData;
}

namespace widgets {
class SearchInput;
class TranslucentFrame;
class SettingsGroup;
class SettingsHead;
class ComboxWidget;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class CheckItem;
class KBLayoutSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit KBLayoutSettingWidget(QWidget *parent = nullptr);
    void setModel(dcc::keyboard::KeyboardModel *model);

Q_SIGNALS:
    void layoutAdded(const QStringList &kblist);
    void requestCurLayoutAdded(const QString &value);
    void curLang(const QString &value);
    void delUserLayout(const QString &value);
    void requestSwitchKBLayout(int value);
    void onSwitchKBLayoutScope(const int index);

public Q_SLOTS:
    void onAddKeyboard(const QString &id, const QString &value);
    void onEdit(bool value);
    void onRemoveLayout(CheckItem *item);
    void onDefault(const QString &value);
    void onSwitchKBChanged();
    void onSwitchKB(int kbSwitch);
    void onLayoutAdded();
    void onLayoutScope(const int value);

private:
    bool m_bEdit;
    dcc::widgets::SettingsGroup *m_group;
    dcc::widgets::SettingsHead *m_head;
    QList<dcc::keyboard::MetaData> m_datas;
    QMap<QString, CheckItem *> m_maps;
    dcc::keyboard::KeyboardModel *m_model;
    dcc::widgets::SettingsGroup *m_switchKBLayout;
    QMap<CheckItem *, int> m_switchCheckItem;
    dcc::widgets::ComboxWidget *m_comboWidget;
};
}
}
#endif // KBLAYOUTSETTINGWIDGET_H
