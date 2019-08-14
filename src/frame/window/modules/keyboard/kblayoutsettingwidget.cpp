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
#include "kblayoutsettingwidget.h"
#include "checkitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"
#include "widgets/settingshead.h"
#include "modules/keyboard/checkitem.h"
#include "modules/keyboard/keylabel.h"
#include "modules/keyboard/keyboardmodel.h"

#include <DFloatingButton>
#include <DAnchors>

#include <QStringList>
#include <QVBoxLayout>
#include <QDebug>
#include <QList>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc;
using namespace dcc::keyboard;
using namespace dcc::widgets;

KBLayoutSettingWidget::KBLayoutSettingWidget(QWidget *parent)
    : ContentWidget(parent)
    , m_bEdit(false)
{
    TranslucentFrame *content = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(20);
    layout->setMargin(0);

    m_group = new SettingsGroup();
    m_head = new SettingsHead();
    m_head->setTitle(tr("Keyboard Layout"));
    m_head->setEditEnable(false);

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

    QMap<int, QStringList> shortCutMap;
    shortCutMap.insert(1, QStringList() << "Ctrl" << "Shift");
    shortCutMap.insert(2, QStringList() << "Alt" << "Shift");
    shortCutMap.insert(4, QStringList() << "Super" << "Space");

    QMap<int, QStringList>::iterator iter;
    for (iter = shortCutMap.begin(); iter != shortCutMap.end(); ++iter) {
        qDebug() << "key: " << iter.key() << "value: " << iter.value();
        CheckItem *shortCutItem = new CheckItem;
        shortCutItem->setKeyLabelEffect(iter.value());
        shortCutItem->setMultipleMode(true);
        m_switchKBLayout->appendItem(shortCutItem);
        m_switchCheckItem.insert(shortCutItem, iter.key());
        connect(shortCutItem, &CheckItem::checkedChanged, this, &KBLayoutSettingWidget::onSwitchKBChanged);
    }
    content->setLayout(layout);
    setContent(content);

    DFloatingButton *addLayout = new DFloatingButton(QString("+"), this);
    DAnchors<DFloatingButton> anchors(addLayout);
    anchors.setAnchor(Qt::AnchorBottom, this, Qt::AnchorBottom);
    anchors.setBottomMargin(2);
    anchors.setAnchor(Qt::AnchorHorizontalCenter, this, Qt::AnchorHorizontalCenter);

    connect(addLayout, &DFloatingButton::clicked, this, &KBLayoutSettingWidget::onLayoutAdded);
    connect(m_head, &SettingsHead::editChanged, this, &KBLayoutSettingWidget::onEdit);
}

void KBLayoutSettingWidget::setModel(KeyboardModel *model)
{
    m_model = model;

    connect(model, &KeyboardModel::userLayoutChanged, this, &KBLayoutSettingWidget::onAddKeyboard);
    connect(model, &KeyboardModel::curLayoutChanged, this, &KBLayoutSettingWidget::onDefault);
    connect(model, &KeyboardModel::kbSwitchChanged, this, &KBLayoutSettingWidget::onSwitchKB);

    QMap<QString, QString> map = model->userLayout();

    for (auto i(map.begin()); i != map.end(); ++i) {
        onAddKeyboard(i.key(), i.value());
    }

    m_switchKBLayout->setVisible(m_maps.count() > 1);
    onSwitchKB(model->kbSwitch());
}

void KBLayoutSettingWidget::onAddKeyboard(const QString &id, const QString &value)
{
    if (m_maps.contains(id))
        return;

    CheckItem *checkItem = new CheckItem();
    connect(m_head, &SettingsHead::editChanged, checkItem, &CheckItem::onEditMode);
    connect(checkItem, &CheckItem::checkedChanged, this, &KBLayoutSettingWidget::requestCurLayoutAdded);
    connect(checkItem, &CheckItem::destroySelf, this, &KBLayoutSettingWidget::onRemoveLayout);

    checkItem->setTitle(value);
    checkItem->onEditMode(m_bEdit);

    m_group->appendItem(checkItem);
    m_maps[id] = checkItem;

    m_head->setEditEnable(m_maps.size() > 1);

    onDefault(m_model->curLayout());
    m_switchKBLayout->setVisible(m_maps.count() > 1);
}

void KBLayoutSettingWidget::onEdit(bool value)
{
    m_bEdit = value;
}

void KBLayoutSettingWidget::onRemoveLayout(CheckItem *item)
{
    if (item) {
        m_group->removeItem(item);
        Q_EMIT delUserLayout(item->title());
        m_maps.remove(m_model->userLayout().key(item->title()));
        item->deleteLater();
    }

    if (m_maps.size() < 2) {
        m_head->setEditEnable(false);
    }

    m_switchKBLayout->setVisible(m_maps.count() > 1);
}

void KBLayoutSettingWidget::onDefault(const QString &value)
{
    for (auto i(m_maps.begin()); i != m_maps.end(); ++i) {
        CheckItem *item = i.value();
        item->setChecked(item->title() == value);
    }
}

void KBLayoutSettingWidget::onSwitchKBChanged()
{
    CheckItem *item = qobject_cast<CheckItem *>(sender());
    Q_ASSERT(item);
    int median = 0;
    for (QMap<CheckItem *, int>::const_iterator it(m_switchCheckItem.begin()); it != m_switchCheckItem.end(); ++it) {
        if (it.key()->checked()) {
            median = it.value() | median;
        }
    }

    Q_EMIT requestSwitchKBLayout(median);
}

void KBLayoutSettingWidget::onSwitchKB(int kbSwitch)
{
    for (auto it(m_switchCheckItem.begin()); it != m_switchCheckItem.end(); ++it) {
        it.key()->setChecked((kbSwitch & it.value()) != 0);
    }
}

void KBLayoutSettingWidget::onLayoutAdded()
{
    Q_EMIT layoutAdded(m_maps.keys());
}
