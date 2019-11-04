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

#include "shortcutsettingwidget.h"
#include "window/utils.h"
#include "modules/keyboard/shortcutmodel.h"
#include "modules/keyboard/shortcutitem.h"
#include "widgets/settingshead.h"
#include "widgets/translucentframe.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/searchinput.h"

#include <DAnchors>

#include <QLineEdit>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc;
using namespace dcc::keyboard;

ShortCutSettingWidget::ShortCutSettingWidget(ShortcutModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
{
    m_searchDelayTimer = new QTimer(this);
    m_searchDelayTimer->setInterval(300);
    m_searchDelayTimer->setSingleShot(true);

    m_searchText = QString();
    SettingsHead *systemHead = new SettingsHead();
    //~ contents_path /keyboard/Shortcuts
    systemHead->setTitle(tr("System"));
    m_systemGroup = new SettingsGroup();
    m_systemGroup->appendItem(systemHead, SettingsGroup::NoneBackground);

    SettingsHead *windowHead = new SettingsHead();
    //~ contents_path /keyboard/Shortcuts
    windowHead->setTitle(tr("Window"));
    m_windowGroup = new SettingsGroup();
    m_windowGroup->appendItem(windowHead, SettingsGroup::NoneBackground);

    SettingsHead *workspaceHead = new SettingsHead();
    //~ contents_path /keyboard/Shortcuts
    workspaceHead->setTitle(tr("Workspace"));
    m_workspaceGroup = new SettingsGroup();
    m_workspaceGroup->appendItem(workspaceHead, SettingsGroup::NoneBackground);

    m_customGroup = new SettingsGroup();
    m_searchGroup = new SettingsGroup();
    m_searchInput = new SearchInput();

    m_head = new SettingsHead();
    m_head->setEditEnable(true);
    m_head->setVisible(false);
    //~ contents_path /keyboard/Shortcuts/Custom Shortcut
    m_head->setTitle(tr("Custom Shortcut"));
    m_customGroup->appendItem(m_head, SettingsGroup::NoneBackground);

    QVBoxLayout *vlayout = new QVBoxLayout();

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setAlignment(Qt::AlignTop);
    topLayout->addSpacing(10);
    topLayout->addWidget(m_searchInput);
    topLayout->addSpacing(10);
    vlayout->addLayout(topLayout);

    vlayout->setMargin(0);
    vlayout->setSpacing(10);
    //vlayout->addSpacing(10);

    m_layout = new QVBoxLayout;
    m_layout->addWidget(m_systemGroup);
    m_layout->addSpacing(List_Interval);
    m_layout->addWidget(m_windowGroup);
    m_layout->addSpacing(List_Interval);
    m_layout->addWidget(m_workspaceGroup);
    m_layout->addSpacing(List_Interval);
    m_layout->addWidget(m_customGroup);

    //~ contents_path /keyboard/Shortcuts
    m_resetBtn = new QPushButton(tr("Restore Defaults"));
    m_layout->setMargin(10);
    m_layout->addWidget(m_resetBtn);
    m_layout->addSpacing(10);
    m_layout->addStretch();

    QWidget *widget = new QWidget(this);
    widget->setLayout(m_layout);
    ContentWidget *m_contentWidget = new ContentWidget(this);
    m_contentWidget->setContent(widget);
    vlayout->addWidget(m_contentWidget);

    m_addCustomShortcut = new DFloatingButton(DStyle::SP_IncreaseElement, this);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    btnLayout->addWidget(m_addCustomShortcut);
    vlayout->addLayout(btnLayout);
    vlayout->setContentsMargins(ThirdPageContentsMargins);
    setLayout(vlayout);

    connect(m_addCustomShortcut, &DFloatingButton::clicked, this, &ShortCutSettingWidget::customShortcut);
    connect(m_resetBtn, &QPushButton::clicked, this, &ShortCutSettingWidget::requestReset);
    connect(m_searchInput, &QLineEdit::textChanged, this, &ShortCutSettingWidget::onSearchTextChanged);
    connect(m_searchDelayTimer, &QTimer::timeout, this, &ShortCutSettingWidget::prepareSearchKeys);
    setWindowTitle(tr("Shortcut"));

    connect(m_model, &ShortcutModel::addCustomInfo, this, &ShortCutSettingWidget::onCustomAdded);
    connect(m_model, &ShortcutModel::listChanged, this, &ShortCutSettingWidget::addShortcut);
    connect(m_model, &ShortcutModel::shortcutChanged, this, &ShortCutSettingWidget::onShortcutChanged);
    connect(m_model, &ShortcutModel::keyEvent, this, &ShortCutSettingWidget::onKeyEvent);
    connect(m_model, &ShortcutModel::searchFinished, this, &ShortCutSettingWidget::onSearchStringFinish);

    addShortcut(m_model->systemInfo(), ShortcutModel::System);
    addShortcut(m_model->windowInfo(), ShortcutModel::Window);
    addShortcut(m_model->workspaceInfo(), ShortcutModel::Workspace);
    addShortcut(m_model->customInfo(), ShortcutModel::Custom);
}

void ShortCutSettingWidget::showCustomShotcut()
{
    m_addCustomShortcut->click();
}

void ShortCutSettingWidget::addShortcut(QList<ShortcutInfo *> list, ShortcutModel::InfoType type)
{
    QMap<ShortcutModel::InfoType, QList<ShortcutItem *>*> InfoMap {
        {ShortcutModel::System, &m_systemList},
        {ShortcutModel::Window, &m_windowList},
        {ShortcutModel::Workspace, &m_workspaceList},
        {ShortcutModel::Custom, &m_customList}
    };

    QMap<ShortcutModel::InfoType, SettingsGroup *> GroupMap {
        {ShortcutModel::System, m_systemGroup},
        {ShortcutModel::Window, m_windowGroup},
        {ShortcutModel::Workspace, m_workspaceGroup},
        {ShortcutModel::Custom, m_customGroup}
    };

    QList<ShortcutItem *> *itemList{ InfoMap[type] };
    auto group = GroupMap[type];
    for (auto it = itemList->begin(); it != itemList->end();) {
        ShortcutItem *item = *it;

        group->removeItem(item);
        m_allList.removeOne(item);
        it = itemList->erase(it);
        item->deleteLater();
    }

    QList<ShortcutInfo *>::iterator it = list.begin();
    for (; it != list.end(); ++it) {
        ShortcutInfo *info = *it;
        ShortcutItem *item = new ShortcutItem();
        connect(item, &ShortcutItem::requestUpdateKey, this, &ShortCutSettingWidget::requestUpdateKey);
        item->setShortcutInfo(info);
        item->setTitle(info->name);
        info->item = item;
        m_searchInfos[info->toString()] = info;

        m_allList << item;

        switch (type) {
        case ShortcutModel::System:
            m_systemGroup->appendItem(item);
            m_systemList.append(item);
            break;
        case ShortcutModel::Window:
            m_windowGroup->appendItem(item);
            m_windowList.append(item);
            break;
        case ShortcutModel::Workspace:
            m_workspaceGroup->appendItem(item);
            m_workspaceList.append(item);
            break;
        case ShortcutModel::Custom:
            connect(m_head, &SettingsHead::editChanged, item, &ShortcutItem::onEditMode);
            m_customGroup->appendItem(item);
            m_customList.append(item);

            if (m_customGroup->itemCount() > 1)
                m_head->setVisible(true);

            connect(item, &ShortcutItem::requestRemove, this, &ShortCutSettingWidget::onDestroyItem);
            connect(item, &ShortcutItem::shortcutEditChanged, this, &ShortCutSettingWidget::shortcutEditChanged);
            break;
        default:
            break;
        }
    }
}

SettingsHead *ShortCutSettingWidget::getHead() const
{
    return m_head;
}

void ShortCutSettingWidget::modifyStatus(bool status)
{
    if (status) {
        m_customGroup->hide();
        m_workspaceGroup->hide();
        m_windowGroup->hide();
        m_systemGroup->hide();
        m_resetBtn->hide();
        m_searchGroup->show();
        m_layout->removeWidget(m_customGroup);
        m_layout->removeWidget(m_workspaceGroup);
        m_layout->removeWidget(m_windowGroup);
        m_layout->removeWidget(m_systemGroup);
        m_layout->insertWidget(0, m_searchGroup, 0, Qt::AlignTop);
    } else {
        m_customGroup->show();
        m_workspaceGroup->show();
        m_windowGroup->show();
        m_systemGroup->show();
        m_resetBtn->show();
        m_searchGroup->hide();
        m_layout->insertWidget(0, m_customGroup);
        m_layout->insertWidget(0, m_workspaceGroup);
        m_layout->insertWidget(0, m_windowGroup);
        m_layout->insertWidget(0, m_systemGroup);
        m_layout->removeWidget(m_searchGroup);
    }
}

void ShortCutSettingWidget::onSearchTextChanged(const QString &text)
{
    if (m_searchText.length() == 0 || text.length() == 0) {
        modifyStatus(text.length() > 0);
    }
    m_searchText = text;
    qDebug() << "search text is " << m_searchText;
    if (text.length() > 0) {
        m_searchDelayTimer->start();
    }
}

void ShortCutSettingWidget::onCustomAdded(ShortcutInfo *info)
{
    if (info) {
        ShortcutItem *item = new ShortcutItem();
        connect(item, &ShortcutItem::requestUpdateKey, this, &ShortCutSettingWidget::requestUpdateKey);
        item->setShortcutInfo(info);
        item->setTitle(info->name);
        info->item = item;

        m_searchInfos[info->toString()] = info;

        m_allList << item;

        m_head->setVisible(true);
        connect(m_head, &SettingsHead::editChanged, item, &ShortcutItem::onEditMode);
        m_customGroup->appendItem(item);
        m_customList.append(item);

        connect(item, &ShortcutItem::requestRemove, this, &ShortCutSettingWidget::onDestroyItem);
        connect(item, &ShortcutItem::shortcutEditChanged, this, &ShortCutSettingWidget::shortcutEditChanged);
    }
}

void ShortCutSettingWidget::onDestroyItem(ShortcutInfo *info)
{
    m_head->toCancel();
    ShortcutItem *item = info->item;
    m_customGroup->removeItem(item);
    if (m_customGroup->itemCount() == 1) {
        m_head->setVisible(false);
    }
    m_searchInfos.remove(item->curInfo()->toString());
    m_customList.removeOne(item);
    m_allList.removeOne(item);
    Q_EMIT delShortcutInfo(item->curInfo());
    item->deleteLater();
}

void ShortCutSettingWidget::onSearchInfo(ShortcutInfo *info, const QString &key)
{
    if (m_searchInfos.keys().contains(key)) {
        m_searchInfos.remove(key);
        m_searchInfos[info->toString()] = info;
    }
}

void ShortCutSettingWidget::onSearchStringFinish(const QList<ShortcutInfo*> searchList)
{
    for (int i = 0; i != m_searchGroup->itemCount(); ++i) {
        m_searchGroup->getItem(i)->deleteLater();
    }
    m_searchGroup->clear();
    QList<ShortcutInfo *> list = searchList;
    qDebug() << "searchList count is " << searchList.count();
    for (int i = 0; i < list.count(); i++) {
        ShortcutItem *item = new ShortcutItem;
        connect(item, &ShortcutItem::requestUpdateKey, this, &ShortCutSettingWidget::requestUpdateKey);
        item->setShortcutInfo(list[i]);
        item->setTitle(list[i]->name);
        item->setFixedHeight(36);
        m_searchGroup->appendItem(item);
    }
}

void ShortCutSettingWidget::prepareSearchKeys()
{
    Q_EMIT requestSearch(m_searchText);
}

void ShortCutSettingWidget::onRemoveItem(const QString &id, int type)
{
    Q_UNUSED(type)

    for (auto item(m_customList.begin()); item != m_customList.end(); ++item) {
        ShortcutItem *it = *item;
        Q_ASSERT(it);
        if (it->curInfo()->id == id) {
            m_customGroup->removeItem(it);
            m_customList.removeOne(it);
            m_allList.removeOne(it);
            it->deleteLater();
            return;
        }
    }
}

void ShortCutSettingWidget::onShortcutChanged(ShortcutInfo *info)
{
    for (ShortcutItem *item : m_allList) {
        if (item->curInfo()->id == info->id) {
            item->setShortcutInfo(info);
            break;
        }
    }
}

void ShortCutSettingWidget::onKeyEvent(bool press, const QString &shortcut)
{
    ShortcutInfo *current = m_model->currentInfo();

    if (!current)
        return;

    ShortcutInfo *conflict = m_model->getInfo(shortcut);

    if (conflict == current && conflict->accels == current->accels) {
        current->item->setShortcut(current->accels);
        return;
    }

    if (!press) {
        if (shortcut.isEmpty()) {
            current->item->setShortcut(current->accels);
            return;
        }

        if (shortcut == "BackSpace" || shortcut == "Delete") {
            current->item->setShortcut("");
            Q_EMIT requestDisableShortcut(current);
        } else {
            if (conflict) {
                // have conflict
                Q_EMIT requestShowConflict(current, shortcut);
                current->item->setShortcut(current->accels);
            } else {
                // save
                current->accels = shortcut;
                Q_EMIT requestSaveShortcut(current);
            }
        }
        return;
    }

    // update shortcut to item
    current->item->setShortcut(shortcut);
}

