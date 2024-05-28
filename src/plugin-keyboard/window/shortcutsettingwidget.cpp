//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "shortcutsettingwidget.h"
#include "operation/shortcutmodel.h"
#include "shortcutitem.h"
#include "widgets/settingshead.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/settingsgroup.h"
#include "searchinput.h"

#include <DAnchors>
#include <DSysInfo>

#include <QLineEdit>
#include <QTimer>
#include <QPushButton>

DCORE_USE_NAMESPACE
using namespace DCC_NAMESPACE;
ShortCutSettingWidget::ShortCutSettingWidget(ShortcutModel *model, QWidget *parent)
    : QWidget(parent)
    , m_workspaceGroup(nullptr)
    , m_assistiveToolsGroup(nullptr)
    , m_model(model)
{
    setAccessibleName("ShortCutSettingWidget");
    m_searchDelayTimer = new QTimer(this);
    m_searchDelayTimer->setInterval(300);
    m_searchDelayTimer->setSingleShot(true);

    m_searchText = QString();
    SettingsHead *systemHead = new SettingsHead();
    systemHead->setEditEnable(false);
    systemHead->setTitle(tr("System Management"));
    systemHead->layout()->setContentsMargins(10, 0, 10, 0);
    m_systemGroup = new SettingsGroup();
    m_systemGroup->getLayout()->setMargin(0);
    m_systemGroup->appendItem(systemHead, SettingsGroup::NoneBackground);

    SettingsHead *windowHead = new SettingsHead();
    windowHead->setEditEnable(false);
    windowHead->setTitle(tr("Window"));
    windowHead->layout()->setContentsMargins(10, 0, 10, 0);
    m_windowGroup = new SettingsGroup();
    m_windowGroup->getLayout()->setMargin(0);
    m_windowGroup->appendItem(windowHead, SettingsGroup::NoneBackground);

    if (DSysInfo::UosServer != DSysInfo::uosType()) {
        m_workspaceHead = new SettingsHead();
        m_workspaceHead->setEditEnable(false);
        m_workspaceHead->setTitle(tr("Workspace"));
        m_workspaceHead->layout()->setContentsMargins(10, 0, 10, 0);
        m_workspaceGroup = new SettingsGroup();
        m_workspaceGroup->appendItem(m_workspaceHead, SettingsGroup::NoneBackground);
    }

    if (DSysInfo::UosServer != DSysInfo::uosType() && DSysInfo::UosCommunity != DSysInfo::uosEditionType()) {
        SettingsHead *speechHead = new SettingsHead();
        speechHead->setTitle(tr("Assistive Tools"));
        speechHead->setEditEnable(false);
        speechHead->layout()->setContentsMargins(10, 0, 10, 0);
        m_assistiveToolsGroup = new SettingsGroup();
        m_assistiveToolsGroup->appendItem(speechHead, SettingsGroup::NoneBackground);
    }

    m_customGroup = new SettingsGroup();
    m_searchGroup = new SettingsGroup();
    m_searchInput = new SearchInput();
    m_searchInput->setAccessibleName("KEYBOARD_LINEEDIT");
    m_searchInput->setObjectName("KeyboardLineEdit");

    m_head = new SettingsHead();
    m_head->setEditEnable(true);
    m_head->setVisible(false);
    m_head->setTitle(tr("Custom Shortcut"));
    m_head->layout()->setContentsMargins(10, 0, 10, 0);
    m_customGroup->appendItem(m_head, SettingsGroup::NoneBackground);

    QVBoxLayout *vlayout = new QVBoxLayout();
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setMargin(0);
    topLayout->setAlignment(Qt::AlignTop);
    topLayout->addWidget(m_searchInput);
    vlayout->addLayout(topLayout);

    vlayout->setMargin(0);
    vlayout->setSpacing(10);
    //vlayout->addSpacing(10);

    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addSpacing(3);
    m_layout->addWidget(m_systemGroup);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_windowGroup);
    if (m_workspaceGroup) {
        m_layout->addSpacing(10);
        m_layout->addWidget(m_workspaceGroup);
    }

    if (m_assistiveToolsGroup) {
        m_layout->addSpacing(10);
        m_layout->addWidget(m_assistiveToolsGroup);
    }
    m_layout->addSpacing(10);
    m_layout->addWidget(m_customGroup);

    m_resetBtn = new QPushButton(tr("Restore Defaults"));
    //m_layout->setMargin(10);
    m_layout->addWidget(m_resetBtn);
    m_layout->addSpacing(10);
    m_layout->addStretch();

    QWidget *widget = new QWidget(this);
    widget->setAccessibleName("ShortCutSettingWidget_widget");
    widget->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(m_layout);
    vlayout->addWidget(widget);

    widget->hide();
    m_searchInput->hide();

    vlayout->setContentsMargins(0, 10, 0, 5);
    setLayout(vlayout);

    connect(m_resetBtn, &QPushButton::clicked, this, [ = ] {
        if (!m_bIsResting) {
            m_bIsResting = true;
            Q_EMIT requestReset();
        }
    });

    connect(m_searchInput, &QLineEdit::textChanged, this, &ShortCutSettingWidget::onSearchTextChanged);
    connect(m_searchDelayTimer, &QTimer::timeout, this, &ShortCutSettingWidget::prepareSearchKeys);
    setWindowTitle(tr("Shortcut"));

    connect(m_model, &ShortcutModel::addCustomInfo, this, &ShortCutSettingWidget::onCustomAdded);
    //每次页面点击时会通过m_work->refreshShortcut()时,model会发出listChanged信号，对界面进行初始化
    connect(m_model, &ShortcutModel::listChanged, this, &ShortCutSettingWidget::addShortcut);
    connect(m_model, &ShortcutModel::shortcutChanged, this, &ShortCutSettingWidget::onShortcutChanged);
    connect(m_model, &ShortcutModel::keyEvent, this, &ShortCutSettingWidget::onKeyEvent);
    connect(m_model, &ShortcutModel::searchFinished, this, &ShortCutSettingWidget::onSearchStringFinish);

    QTimer::singleShot(10, this, [=] {
        widget->show();
        m_searchInput->show();
    });
}

void ShortCutSettingWidget::addShortcut(QList<ShortcutInfo *> list, ShortcutModel::InfoType type)
{
    if ((m_assistiveToolsGroup == nullptr) && (type == ShortcutModel::AssistiveTools)) {
        m_assistiveToolsIdList.clear();
        QList<ShortcutInfo *>::iterator it = list.begin();
        for (; it != list.end(); ++it) {
            ShortcutInfo *assistiveToolsinfo = *it;
            m_assistiveToolsIdList << assistiveToolsinfo->id;
        }
        return;
    }

    if ((m_workspaceGroup == nullptr) && (type == ShortcutModel::Workspace)) {
        m_workspaceIdList.clear();
        QList<ShortcutInfo *>::iterator it = list.begin();
        for (; it != list.end(); ++it) {
            ShortcutInfo *workspaceIdListlsinfo = *it;
            m_workspaceIdList << workspaceIdListlsinfo->id;
        }
        return;
    }
    QMap<ShortcutModel::InfoType, QList<ShortcutItem *>*> InfoMap {
        {ShortcutModel::System, &m_systemList},
        {ShortcutModel::Window, &m_windowList},
        {ShortcutModel::Workspace, &m_workspaceList},
        {ShortcutModel::AssistiveTools, &m_assistiveToolsList},
        {ShortcutModel::Custom, &m_customList}
    };

    QMap<ShortcutModel::InfoType, SettingsGroup *> GroupMap {
        {ShortcutModel::System, m_systemGroup},
        {ShortcutModel::Window, m_windowGroup},
        {ShortcutModel::Workspace, m_workspaceGroup},
        {ShortcutModel::AssistiveTools, m_assistiveToolsGroup},
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
        item->setAccessibleName(info->name);
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

            if (m_workspaceGroup->itemCount() > 1)
                m_workspaceHead->setVisible(true);
            break;
        case ShortcutModel::AssistiveTools:
            m_assistiveToolsGroup->appendItem(item);
            m_assistiveToolsList.append(item);
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
        if (m_assistiveToolsGroup) {
            m_assistiveToolsGroup->hide();
        }
        if (m_workspaceGroup) {
            m_workspaceGroup->hide();
        }

        m_windowGroup->hide();
        m_systemGroup->hide();
        m_resetBtn->hide();
        m_searchGroup->show();
        m_layout->removeWidget(m_customGroup);
        if (m_assistiveToolsGroup) {
            m_layout->removeWidget(m_assistiveToolsGroup);
        }
        if (m_workspaceGroup) {
            m_layout->removeWidget(m_workspaceGroup);
        }

        m_layout->removeWidget(m_windowGroup);
        m_layout->removeWidget(m_systemGroup);
        m_layout->insertWidget(0, m_searchGroup, 0, Qt::AlignTop);
    } else {
        m_customGroup->show();
        if (m_assistiveToolsGroup) {
            m_assistiveToolsGroup->show();
        }
        if (m_workspaceGroup) {
            m_workspaceGroup->show();
        }

        m_windowGroup->show();
        m_systemGroup->show();
        m_resetBtn->show();
        m_searchGroup->hide();
        m_layout->removeWidget(m_searchGroup);
        m_layout->insertWidget(0, m_systemGroup);
        m_layout->insertWidget(2, m_windowGroup);

        if (m_workspaceGroup) {
            m_layout->insertWidget(4, m_workspaceGroup);
            if (m_assistiveToolsGroup) {
                m_layout->insertWidget(6, m_assistiveToolsGroup);
                m_layout->insertWidget(8, m_customGroup);
            } else {
                m_layout->insertWidget(6, m_customGroup);
            }
        } else {
            if (m_assistiveToolsGroup) {
                m_layout->insertWidget(4, m_assistiveToolsGroup);
                m_layout->insertWidget(6, m_customGroup);
            } else {
                m_layout->insertWidget(4, m_customGroup);
            }
        }

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
        if (m_assistiveToolsGroup == nullptr && m_assistiveToolsIdList.contains(list[i]->id))
            continue;

        if (m_workspaceGroup == nullptr && m_workspaceIdList.contains(list[i]->id))
            continue;

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

void ShortCutSettingWidget::onResetFinished()
{
    m_bIsResting = false;
}

