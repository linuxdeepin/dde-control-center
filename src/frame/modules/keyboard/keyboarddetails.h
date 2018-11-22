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

#ifndef KEYBOARDDETAILS_H
#define KEYBOARDDETAILS_H

#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingshead.h"
#include "checkitem.h"
#include "indexmodel.h"
#include "keyboardmodel.h"

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{
class KeyboardDetails : public ContentWidget
{
    Q_OBJECT

public:
    explicit KeyboardDetails(QWidget* parent = 0);
    void setModel(KeyboardModel * model);

Q_SIGNALS:
    void layoutAdded(const QStringList &kblist);
    void requestCurLayoutAdded(const QString& value);
    void curLang(const QString& value);
    void delUserLayout(const QString& value);
    void requestSwitchKBLayout(int value);

public Q_SLOTS:
    void onAddKeyboard(const QString &id, const QString &value);
    void onEdit(bool value);
    void onRemoveLayout(CheckItem* item);
    void onDefault(const QString &value);
    void onSwitchKBChanged();
    void onSwitchKB(int kbSwitch);
    void onLayoutAdded();

private:
    bool m_bEdit;
    SettingsGroup* m_group;
    SettingsHead* m_head;
    QList<MetaData> m_datas;
    QMap<QString, CheckItem*> m_maps;
    KeyboardModel *m_model;
    SettingsGroup *m_switchKBLayout;
    QMap<CheckItem*, int> m_switchCheckItem;
};
}
}
#endif // KEYBOARDDETAILS_H
