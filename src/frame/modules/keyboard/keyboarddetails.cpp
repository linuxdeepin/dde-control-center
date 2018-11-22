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

#include "keyboarddetails.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingshead.h"
#include "checkitem.h"
#include "widgets/translucentframe.h"
#include <QVBoxLayout>
#include <QDebug>

using namespace dcc;

namespace dcc {
namespace keyboard{
KeyboardDetails::KeyboardDetails(QWidget *parent)
    :ContentWidget(parent),
      m_bEdit(false)
{
    TranslucentFrame* content = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(20);
    layout->setMargin(0);

    m_group = new SettingsGroup();
    m_head = new SettingsHead();
    m_head->setTitle(tr("Keyboard Layout"));
    m_head->setEditEnable(false);
    setTitle(tr("Keyboard Layout"));

    m_group->appendItem(m_head);

    layout->addSpacing(10);
    layout->addWidget(m_group);

    m_switchKBLayout = new SettingsGroup;
    SettingsHead *head = new SettingsHead;
    head->setTitle(tr("Switch Layouts (Multiple)"));
    head->setEditEnable(false);

    m_switchKBLayout->appendItem(head);

    layout->addSpacing(10);
    layout->addWidget(m_switchKBLayout);

    CheckItem *ctrlShift = new CheckItem;
    ctrlShift->setTitle("Ctrl+Shift");
    ctrlShift->setMultipleMode(true);

    CheckItem *altShift = new CheckItem;
    altShift->setTitle("Alt+Shift");
    altShift->setMultipleMode(true);

    CheckItem *superSpace = new CheckItem;
    superSpace->setTitle("Super+Space");
    superSpace->setMultipleMode(true);

    m_switchKBLayout->appendItem(ctrlShift);
    m_switchKBLayout->appendItem(altShift);
    m_switchKBLayout->appendItem(superSpace);

    m_switchCheckItem.insert(ctrlShift, 1);
    m_switchCheckItem.insert(altShift, 2);
    m_switchCheckItem.insert(superSpace, 4);

    QPushButton* addBtn = new QPushButton(tr("Add Keyboard Layout"));
    layout->addWidget(addBtn);

    content->setLayout(layout);

    setContent(content);

    connect(addBtn, &QPushButton::clicked, this, &KeyboardDetails::onLayoutAdded);
    connect(m_head, SIGNAL(editChanged(bool)), this ,SLOT(onEdit(bool)));

    connect(ctrlShift, &CheckItem::checkedChanged, this, &KeyboardDetails::onSwitchKBChanged);
    connect(altShift, &CheckItem::checkedChanged, this, &KeyboardDetails::onSwitchKBChanged);
    connect(superSpace, &CheckItem::checkedChanged, this, &KeyboardDetails::onSwitchKBChanged);
}

void KeyboardDetails::setModel(KeyboardModel *model)
{
    m_model = model;

#ifndef DCC_DISABLE_KBLAYOUT
    connect(model, &KeyboardModel::userLayoutChanged, this, &KeyboardDetails::onAddKeyboard);
    connect(model, &KeyboardModel::curLayoutChanged, this, &KeyboardDetails::onDefault);
    connect(model, &KeyboardModel::kbSwitchChanged, this, &KeyboardDetails::onSwitchKB);
#endif

    QMap<QString, QString> map = model->userLayout();

    for (auto i(map.begin()); i != map.end(); ++i) {
        onAddKeyboard(i.key(), i.value());
    }

    m_switchKBLayout->setVisible(m_maps.count() > 1);
    onSwitchKB(model->kbSwitch());
}

void KeyboardDetails::onAddKeyboard(const QString &id, const QString &value)
{
    if(m_maps.contains(id))
        return;

    CheckItem* checkItem = new CheckItem();
    connect(m_head, SIGNAL(editChanged(bool)), checkItem, SLOT(onEditMode(bool)));
    connect(checkItem, &CheckItem::checkedChanged, this, &KeyboardDetails::requestCurLayoutAdded);
    connect(checkItem, SIGNAL(destroySelf(CheckItem*)), this, SLOT(onRemoveLayout(CheckItem*)));

    checkItem->setTitle(value);
    checkItem->onEditMode(m_bEdit);

    m_group->appendItem(checkItem);
    m_maps[id] = checkItem;

    m_head->setEditEnable(m_maps.size() > 1);

    onDefault(m_model->curLayout());
    m_switchKBLayout->setVisible(m_maps.count() > 1);
}

void KeyboardDetails::onEdit(bool value)
{
    m_bEdit = value;
}

void KeyboardDetails::onRemoveLayout(CheckItem *item)
{
    if(item)
    {
        m_group->removeItem(item);
        Q_EMIT delUserLayout(item->title());
        m_maps.remove(m_model->userLayout().key(item->title()));
        item->deleteLater();
    }

    if (m_maps.size() < 2)
    {
        m_head->setEditEnable(false);
    }

    m_switchKBLayout->setVisible(m_maps.count() > 1);
}

void KeyboardDetails::onDefault(const QString &value)
{
    for (auto i(m_maps.begin()); i != m_maps.end(); ++i) {
       CheckItem *item = i.value();
       item->setChecked(item->title() == value);
    }
}

void KeyboardDetails::onSwitchKBChanged()
{
    CheckItem *item = static_cast<CheckItem*>(sender());
    Q_ASSERT(item);
    QList<int> i;
    for (auto it(m_switchCheckItem.begin()); it != m_switchCheckItem.end(); ++it) {
        if (it.key()->checked()) {
            i.append(it.value());
        }
    }
    int median = 0;
    for (int v : i) {
        median = v|median;
    }
    Q_EMIT requestSwitchKBLayout(median);
}

void KeyboardDetails::onSwitchKB(int kbSwitch)
{
    for (auto it(m_switchCheckItem.begin()); it != m_switchCheckItem.end(); ++it) {
            it.key()->setChecked((kbSwitch & it.value()) != 0);
    }
}

void KeyboardDetails::onLayoutAdded()
{
    Q_EMIT layoutAdded(m_maps.keys());
}

}
}
