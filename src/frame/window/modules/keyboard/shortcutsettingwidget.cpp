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
    m_searchInter = new SearchInter("com.deepin.daemon.Search",
                                    "/com/deepin/daemon/Search",
                                    QDBusConnection::sessionBus(), this);
    m_searchDelayTimer = new QTimer(this);
    m_searchDelayTimer->setInterval(300);
    m_searchDelayTimer->setSingleShot(true);

    m_searchText = QString();
    //~ contents_path /keyboard/Shortcuts
    m_systemGroup = new SettingsGroup(tr("System"));
    //~ contents_path /keyboard/Shortcuts
    m_windowGroup = new SettingsGroup(tr("Window"));
    //~ contents_path /keyboard/Shortcuts
    m_workspaceGroup = new SettingsGroup(tr("Workspace"));
    m_customGroup = new SettingsGroup();
    m_searchGroup = new SettingsGroup();
    m_searchInput = new SearchInput();

    m_head = new SettingsHead();
    m_head->setEditEnable(true);
    m_head->setVisible(false);
    //~ contents_path /keyboard/Shortcuts/Custom Shortcut
    m_head->setTitle(tr("Custom Shortcut"));
    m_customGroup->insertItem(0, m_head);

    QVBoxLayout *vlayout = new QVBoxLayout();

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setAlignment(Qt::AlignTop);
    topLayout->addSpacing(10);
    topLayout->addWidget(m_searchInput);
    topLayout->addSpacing(10);
    vlayout->addLayout(topLayout);

    vlayout->setMargin(0);
    vlayout->setSpacing(10);
    vlayout->addSpacing(10);

    m_layout = new QVBoxLayout;
    m_layout->addWidget(m_systemGroup);
    m_layout->addWidget(m_windowGroup);
    m_layout->addWidget(m_workspaceGroup);
    m_layout->addWidget(m_customGroup);

    //~ contents_path /keyboard/Shortcuts
    QPushButton *resetBtn = new QPushButton(tr("Restore Defaults"));

    m_layout->addWidget(resetBtn);
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
    setLayout(vlayout);

    connect(m_addCustomShortcut, &DFloatingButton::clicked, this, &ShortCutSettingWidget::customShortcut);
    connect(resetBtn, &QPushButton::clicked, this, &ShortCutSettingWidget::requestReset);
    connect(m_searchInput, &QLineEdit::textChanged, this, &ShortCutSettingWidget::onSearchTextChanged);
    connect(m_searchDelayTimer, &QTimer::timeout, this, &ShortCutSettingWidget::prepareSearchKeys);
    //~ contents_path /keyboard/Shortcuts
    setWindowTitle(tr("Shortcut"));

    connect(m_model, &ShortcutModel::addCustomInfo, this, &ShortCutSettingWidget::onCustomAdded);
    connect(m_model, &ShortcutModel::listChanged, this, &ShortCutSettingWidget::addShortcut);
    connect(m_model, &ShortcutModel::shortcutChanged, this, &ShortCutSettingWidget::onShortcutChanged);
    connect(m_model, &ShortcutModel::keyEvent, this, &ShortCutSettingWidget::onKeyEvent);

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
        m_searchGroup->show();
        m_layout->removeWidget(m_customGroup);
        m_layout->removeWidget(m_workspaceGroup);
        m_layout->removeWidget(m_windowGroup);
        m_layout->removeWidget(m_systemGroup);
        m_layout->insertWidget(0, m_searchGroup);
    } else {
        m_customGroup->show();
        m_workspaceGroup->show();
        m_windowGroup->show();
        m_systemGroup->show();
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
    if (m_searchText.length() == 0 || text.length() == 0)
        modifyStatus(text.length() > 0);
    m_searchText = text;

    if (text.length() > 0)
        m_searchDelayTimer->start();
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

void ShortCutSettingWidget::onSearchKeysFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    watch->deleteLater();

    const QString key = reply.value();

    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_searchInter->SearchString(m_searchText, key), this);

    connect(result, &QDBusPendingCallWatcher::finished, this, &ShortCutSettingWidget::onSearchStringFinish, Qt::QueuedConnection);
}

void ShortCutSettingWidget::onSearchStringFinish(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QStringList> reply = *watch;
    watch->deleteLater();
    for (int i(0); i != m_searchGroup->itemCount(); ++i)
        m_searchGroup->getItem(i)->deleteLater();
    m_searchGroup->clear();
    QStringList list = reply.value();
    for (int i = 0; i < list.count(); i++) {
        ShortcutInfo *info = m_searchInfos[list.at(i)];
        ShortcutItem *item = new ShortcutItem;
        connect(item, &ShortcutItem::requestUpdateKey, this, &ShortCutSettingWidget::requestUpdateKey);
        item->setShortcutInfo(info);
        item->setTitle(info->name);
        m_searchGroup->appendItem(item);
    }
}

void ShortCutSettingWidget::prepareSearchKeys()
{
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_searchInter->NewSearchWithStrList(m_searchInfos.keys()), this);

    connect(result, &QDBusPendingCallWatcher::finished, this, &ShortCutSettingWidget::onSearchKeysFinished, Qt::QueuedConnection);
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

