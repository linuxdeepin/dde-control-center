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

#include "shortcutwidget.h"
#include "shortcutitem.h"
#include "widgets/settingshead.h"
#include "shortcutmodel.h"
#include "widgets/translucentframe.h"
#include "widgets/settingsheaderitem.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>

using namespace dcc;

namespace dcc {
namespace keyboard{

ShortcutWidget::ShortcutWidget(ShortcutModel *model, QWidget *parent)
    :ContentWidget(parent),
      m_model(model)
{
    m_searchInter = new SearchInter("com.deepin.daemon.Search",
                                    "/com/deepin/daemon/Search",
                                    QDBusConnection::sessionBus(), this);
    m_searchDelayTimer = new QTimer(this);
    m_searchDelayTimer->setInterval(300);
    m_searchDelayTimer->setSingleShot(true);

    m_widget = new TranslucentFrame();
    m_searchText = QString();

    m_systemGroup = new SettingsGroup(tr("System"));
    m_windowGroup = new SettingsGroup(tr("Window"));
    m_workspaceGroup = new SettingsGroup(tr("WorkSpace"));
    m_customGroup = new SettingsGroup();
    m_searchGroup = new SettingsGroup();
    m_searchInput = new SearchInput();

    m_head = new SettingsHead();
    m_head->setEditEnable(true);
    m_head->setVisible(false);
    m_head->setTitle(tr("Custom Shortcut"));
    m_customGroup->insertItem(0, m_head);

    m_layout = new QVBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(10);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_searchInput);
    m_layout->addWidget(m_systemGroup);
    m_layout->addWidget(m_windowGroup);
    m_layout->addWidget(m_workspaceGroup);
    m_layout->addWidget(m_customGroup);

    m_addCustom = new QPushButton(tr("Add Custom Shortcut"));

    QPushButton* resetBtn = new QPushButton(tr("Restore Defaults"));

    m_layout->addWidget(m_addCustom);
    m_layout->addSpacing(10);
    m_layout->addWidget(resetBtn);
    m_layout->addSpacing(10);
    m_widget->setLayout(m_layout);

    setContent(m_widget);
    connect(m_addCustom, SIGNAL(clicked()), this, SIGNAL(customShortcut()));
    connect(resetBtn, &QPushButton::clicked, this, &ShortcutWidget::requestReset);
    connect(m_searchInput, &QLineEdit::textChanged, this, &ShortcutWidget::onSearchTextChanged);
    connect(m_searchDelayTimer, &QTimer::timeout, this, &ShortcutWidget::prepareSearchKeys);
    setTitle(tr("Shortcuts"));

    connect(m_model, &ShortcutModel::addCustomInfo, this, &ShortcutWidget::onCustomAdded);
    connect(m_model, &ShortcutModel::listChanged, this, &ShortcutWidget::addShortcut);
    connect(m_model, &ShortcutModel::shortcutChanged, this, &ShortcutWidget::onShortcutChanged);
    connect(m_model, &ShortcutModel::keyEvent, this, &ShortcutWidget::onKeyEvent);

    addShortcut(m_model->systemInfo(), ShortcutModel::System);
    addShortcut(m_model->windowInfo(), ShortcutModel::Window);
    addShortcut(m_model->workspaceInfo(), ShortcutModel::Workspace);
    addShortcut(m_model->customInfo(), ShortcutModel::Custom);
}

void ShortcutWidget::addShortcut(QList<ShortcutInfo *> list, ShortcutModel::InfoType type)
{

    if(type == ShortcutModel::System) {
        for (ShortcutItem *item : m_systemList) {
            if (item) {
                m_systemGroup->removeItem(item);
                item->deleteLater();
            }
        }
        m_systemList.clear();
    }
    else if(type == ShortcutModel::Window) {
        for (ShortcutItem *item : m_windowList) {
            if (item) {
                m_windowGroup->removeItem(item);
                item->deleteLater();
            }
        }
        m_windowList.clear();
    }
    else if(type == ShortcutModel::Workspace) {
        for (ShortcutItem *item : m_workspaceList) {
            if (item) {
                m_workspaceGroup->removeItem(item);
                item->deleteLater();
            }
        }
        m_workspaceList.clear();
    }
    else if(type == ShortcutModel::Custom)
    {
        for (ShortcutItem *item : m_customList) {
            if (item) {
                m_customGroup->removeItem(item);
                item->deleteLater();
            }
        }
        m_customList.clear();
    }

    QList<ShortcutInfo*>::iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        ShortcutItem* item = new ShortcutItem();
        connect(item, &ShortcutItem::requestUpdateKey, this, &ShortcutWidget::requestUpdateKey);
        item->setShortcutInfo((*it));
        item->setTitle((*it)->name);
        (*it)->item = item;
        m_searchInfos[(*it)->toString()] = (*it);

        m_allList << item;

        if(type == ShortcutModel::System) {
            m_systemGroup->appendItem(item);
            m_systemList.append(item);
        }
        else if(type == ShortcutModel::Window) {
            m_windowGroup->appendItem(item);
            m_windowList.append(item);
        }
        else if(type == ShortcutModel::Workspace) {
            m_workspaceGroup->appendItem(item);
            m_workspaceList.append(item);
        }
        else if(type == ShortcutModel::Custom)
        {
           connect(m_head, SIGNAL(editChanged(bool)), item, SLOT(onEditMode(bool)));
            m_customGroup->appendItem(item);
            m_customList.append(item);

            if(m_customGroup->itemCount() > 1)
                m_head->setVisible(true);

            connect(item, &ShortcutItem::requestRemove, this, &ShortcutWidget::onDestroyItem);
            connect(item, &ShortcutItem::shortcutEditChanged, this, &ShortcutWidget::shortcutEditChanged);
        }
    }
}

SettingsHead *ShortcutWidget::getHead()
{
    return m_head;
}

void ShortcutWidget::modifyStatus(bool status)
{
    if(status)
    {
        m_addCustom->hide();
        m_customGroup->hide();
        m_workspaceGroup->hide();
        m_windowGroup->hide();
        m_systemGroup->hide();
        m_searchGroup->show();
        m_layout->removeWidget(m_addCustom);
        m_layout->removeWidget(m_customGroup);
        m_layout->removeWidget(m_workspaceGroup);
        m_layout->removeWidget(m_windowGroup);
        m_layout->removeWidget(m_systemGroup);
        m_layout->insertWidget(2, m_searchGroup);
    }
    else
    {
        m_addCustom->show();
        m_customGroup->show();
        m_workspaceGroup->show();
        m_windowGroup->show();
        m_systemGroup->show();
        m_searchGroup->hide();
        m_layout->insertWidget(2, m_addCustom);
        m_layout->insertWidget(2, m_customGroup);
        m_layout->insertWidget(2, m_workspaceGroup);
        m_layout->insertWidget(2, m_windowGroup);
        m_layout->insertWidget(2, m_systemGroup);
        m_layout->removeWidget(m_searchGroup);
    }
}

void ShortcutWidget::onSearchTextChanged(const QString &text)
{
    if(m_searchText.length() == 0 || text.length() == 0)
        modifyStatus(text.length() > 0);
    m_searchText = text;

    if(text.length() > 0)
        m_searchDelayTimer->start();
}

void ShortcutWidget::onCustomAdded(ShortcutInfo *info)
{
   if(info)
   {
       ShortcutItem* item = new ShortcutItem();
       connect(item, &ShortcutItem::requestUpdateKey, this, &ShortcutWidget::requestUpdateKey);
       item->setShortcutInfo(info);
       item->setTitle(info->name);
       info->item = item;

       m_searchInfos[info->toString()] = info;

       m_allList << item;

       m_head->setVisible(true);
       connect(m_head, SIGNAL(editChanged(bool)), item, SLOT(onEditMode(bool)));
       m_customGroup->appendItem(item);
       m_customList.append(item);

       connect(item, &ShortcutItem::requestRemove, this, &ShortcutWidget::onDestroyItem);
       connect(item, &ShortcutItem::shortcutEditChanged, this, &ShortcutWidget::shortcutEditChanged);
   }
}

void ShortcutWidget::onDestroyItem(ShortcutInfo *info)
{
    m_head->toCancel();

    ShortcutItem* item = info->item;

    m_customGroup->removeItem(item);
    if(m_customGroup->itemCount() == 1)
        m_head->setVisible(false);

    m_searchInfos.remove(item->curInfo()->toString());
    m_customList.removeOne(item);
    m_allList.removeOne(item);

    Q_EMIT delShortcutInfo(item->curInfo());
    item->deleteLater();
}

void ShortcutWidget::onSearchInfo(ShortcutInfo *info, const QString &key)
{
    if(m_searchInfos.keys().contains(key))
    {
        m_searchInfos.remove(key);
        m_searchInfos[info->toString()] = info;
    }
}

void ShortcutWidget::onSearchKeysFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    watch->deleteLater();

    const QString key = reply.value();

    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_searchInter->SearchString(m_searchText, key), this);

    connect(result, &QDBusPendingCallWatcher::finished, this, &ShortcutWidget::onSearchStringFinish, Qt::QueuedConnection);
}

void ShortcutWidget::onSearchStringFinish(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QStringList> reply = *watch;
    watch->deleteLater();

    for (int i(0); i != m_searchGroup->itemCount(); ++i)
        m_searchGroup->getItem(i)->deleteLater();
    m_searchGroup->clear();

    QStringList list = reply.value();
    for(int i = 0; i<list.count(); i++)
    {
        ShortcutInfo* info = m_searchInfos[list.at(i)];
        ShortcutItem* item = new ShortcutItem;

        connect(item, &ShortcutItem::requestUpdateKey, this, &ShortcutWidget::requestUpdateKey);

        item->setShortcutInfo(info);
        item->setTitle(info->name);

        m_searchGroup->appendItem(item);
    }
}

void ShortcutWidget::prepareSearchKeys()
{
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_searchInter->NewSearchWithStrList(m_searchInfos.keys()), this);

    connect(result, &QDBusPendingCallWatcher::finished, this, &ShortcutWidget::onSearchKeysFinished, Qt::QueuedConnection);
}

void ShortcutWidget::onRemoveItem(const QString &id, int type)
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

void ShortcutWidget::onShortcutChanged(ShortcutInfo *info)
{
    for (ShortcutItem *item : m_allList) {
        if (item->curInfo()->id == info->id) {
            item->setShortcutInfo(info);
            break;
        }
    }
}

void ShortcutWidget::onKeyEvent(bool press, const QString &shortcut)
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

        if(shortcut == "BackSpace" || shortcut == "Delete"){
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

}
}
