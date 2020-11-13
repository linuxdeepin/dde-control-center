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
#include "window/utils.h"

#include <DAnchors>

#include <QLineEdit>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc;
using namespace dcc::keyboard;

ShortCutSettingWidget::ShortCutSettingWidget(ShortcutModel *model, QWidget *parent)
    : QWidget(parent)
    , m_assistiveToolsGroup(nullptr)
    , m_model(model)
    , m_shiftKey(false)
    , m_controlKey(false)
    , m_superKey(false)
    , m_altKey(false)
{
    m_searchDelayTimer = new QTimer(this);
    m_searchDelayTimer->setInterval(300);
    m_searchDelayTimer->setSingleShot(true);

    m_searchText = QString();
    m_systemGroup = new SettingsGroup();
    m_systemGroup->getLayout()->setMargin(0);
    SettingsHead *systemHead = new SettingsHead(m_systemGroup);
    systemHead->setEditEnable(false);
    systemHead->setTitle(tr("System"));
    m_systemGroup->appendItem(systemHead, SettingsGroup::NoneBackground);

    QHBoxLayout *systemLayout = new QHBoxLayout();
    systemLayout->addWidget(systemHead);
    systemLayout->setContentsMargins(15,0,0,0);

    m_windowGroup = new SettingsGroup();
    m_windowGroup->getLayout()->setMargin(0);
    SettingsHead *windowHead = new SettingsHead(m_windowGroup);
    windowHead->setEditEnable(false);
    windowHead->setTitle(tr("Window"));
    m_windowGroup->appendItem(windowHead, SettingsGroup::NoneBackground);

    m_workspaceGroup = new SettingsGroup();
    SettingsHead *workspaceHead = new SettingsHead(m_workspaceGroup);
    workspaceHead->setEditEnable(false);
    workspaceHead->setTitle(tr("Workspace"));
    m_workspaceGroup->appendItem(workspaceHead, SettingsGroup::NoneBackground);

    if (!DCC_NAMESPACE::IsServerSystem) {
        m_assistiveToolsGroup = new SettingsGroup();
        SettingsHead *speechHead = new SettingsHead(m_assistiveToolsGroup);
        speechHead->setTitle(tr("Assistive Tools"));
        speechHead->setEditEnable(false);
        m_assistiveToolsGroup->appendItem(speechHead, SettingsGroup::NoneBackground);
        //TODO：klu将辅助功移除， 快捷键模块辅助功能屏蔽
        speechHead->setVisible(false);
        m_assistiveToolsGroup->setVisible(false);
    }

    m_customGroup = new SettingsGroup();
    m_searchGroup = new SettingsGroup();
    m_searchInput = new SearchInput(this);
    m_searchInput->setContentsMargins(0, 0, 0, 0);
    m_searchInput->setAccessibleName("KEYBOARD_LINEEDIT");

    m_head = new SettingsHead(m_customGroup);
    m_head->setEditEnable(true);
    m_head->setVisible(false);
    m_head->setTitle(tr("Custom Shortcut"));
    m_customGroup->appendItem(m_head, SettingsGroup::NoneBackground);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout(this);
    topLayout->setMargin(0);
    topLayout->setAlignment(Qt::AlignTop);
    topLayout->addWidget(m_searchInput);
    vlayout->addLayout(topLayout);

    vlayout->setMargin(0);
    vlayout->setSpacing(10);
    //vlayout->addSpacing(10);

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(ScrollAreaMargins);
    m_layout->addLayout(systemLayout);
    m_layout->addSpacing(3);
    m_layout->addWidget(m_systemGroup);
    m_layout->addSpacing(List_Interval);
    m_layout->addWidget(m_windowGroup);
    m_layout->addSpacing(List_Interval);
    m_layout->addWidget(m_workspaceGroup);
    if (m_assistiveToolsGroup) {
        m_layout->addSpacing(List_Interval);
        m_layout->addWidget(m_assistiveToolsGroup);
    }
    m_layout->addSpacing(List_Interval);
    m_layout->addWidget(m_customGroup);

    m_resetBtn = new QPushButton(tr("Restore Defaults"), this);
    //m_layout->setMargin(10);
    m_layout->addWidget(m_resetBtn);
    m_layout->addSpacing(10);
    m_layout->addStretch();

    QWidget *widget = new QWidget(this);
    widget->setLayout(m_layout);
    ContentWidget *contentWidget = new ContentWidget(this);
    contentWidget->setContent(widget);
    vlayout->addWidget(contentWidget);

    m_addCustomShortcut = new DFloatingButton(DStyle::SP_IncreaseElement, this);

    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->setMargin(0);
    btnLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    btnLayout->addWidget(m_addCustomShortcut);
    vlayout->addLayout(btnLayout);
    vlayout->setContentsMargins(ThirdPageContentsMargins);
    setLayout(vlayout);

    connect(m_addCustomShortcut, &DFloatingButton::clicked, this, &ShortCutSettingWidget::customShortcut);

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
}

ShortCutSettingWidget::~ShortCutSettingWidget()
{
    if (m_systemGroup != nullptr) {
        delete  m_systemGroup;
        m_systemGroup = nullptr;
    }
    if (m_windowGroup != nullptr) {
        delete  m_windowGroup;
        m_windowGroup = nullptr;
    }
    if (m_workspaceGroup != nullptr) {
        delete  m_workspaceGroup;
        m_workspaceGroup = nullptr;
    }
    if (m_assistiveToolsGroup != nullptr) {
        delete  m_assistiveToolsGroup;
        m_assistiveToolsGroup = nullptr;
    }
    if (m_customGroup != nullptr) {
        delete  m_customGroup;
        m_customGroup = nullptr;
    }
    if (m_searchGroup != nullptr) {
        delete  m_searchGroup;
        m_searchGroup = nullptr;
    }
}

void ShortCutSettingWidget::showCustomShotcut()
{
    m_addCustomShortcut->click();
}

void ShortCutSettingWidget::addShortcut(QList<QPair<ShortcutInfoList, ShortcutModel::InfoType>> shotcutInfos)
{
    setGroupSettingVisible(false);

    for (auto pair : shotcutInfos) {
        QList<ShortcutInfo *> list = pair.first;
        ShortcutModel::InfoType type = pair.second;

        if ((m_assistiveToolsGroup == nullptr) && (type == ShortcutModel::AssistiveTools)) {
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
            connect(item, &ShortcutItem::requestUpdateKey, this, &ShortCutSettingWidget::requestUpdateKey);
            item->setShortcutInfo(info);
            item->setTitle(info->name);
            info->item = item;
            m_searchInfos[info->toString()] = info;

            m_allList << item;
            switch (type) {
            case ShortcutModel::System:
                if (info->id == "wm-switcher") {
                    if (item) {
                        delete item;
                    }
                    continue;
                }
                m_systemGroup->appendItem(item);
                item->setParent(m_systemGroup);
                m_systemList.append(item);
                break;
            case ShortcutModel::Window:
                m_windowGroup->appendItem(item);
                item->setParent(m_windowGroup);
                m_windowList.append(item);
                break;
            case ShortcutModel::Workspace:
                m_workspaceGroup->appendItem(item);
                item->setParent(m_workspaceGroup);
                m_workspaceList.append(item);
                break;
            case ShortcutModel::AssistiveTools:
                m_assistiveToolsGroup->appendItem(item);
                item->setParent(m_assistiveToolsGroup);
                m_assistiveToolsList.append(item);
                break;
            case ShortcutModel::Custom:
                connect(m_head, &SettingsHead::editChanged, item, &ShortcutItem::onEditMode);
                m_customGroup->appendItem(item);
                item->setParent(m_customGroup);
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

    setGroupSettingVisible(true);
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
        m_workspaceGroup->hide();
        m_windowGroup->hide();
        m_systemGroup->hide();
        m_resetBtn->hide();
        m_searchGroup->show();
        m_layout->removeWidget(m_customGroup);
        if (m_assistiveToolsGroup) {
            m_layout->removeWidget(m_assistiveToolsGroup);
        }
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
        m_layout->removeWidget(m_searchGroup);
        m_layout->insertWidget(0, m_systemGroup);
        m_layout->insertWidget(2, m_windowGroup);
        m_layout->insertWidget(4, m_workspaceGroup);
        if (m_assistiveToolsGroup) {
            m_layout->insertWidget(6, m_assistiveToolsGroup);
            m_layout->insertWidget(8, m_customGroup);
        } else {
            m_layout->insertWidget(6, m_customGroup);
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
        ShortcutItem *item = new ShortcutItem(m_customGroup);
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
        ShortcutItem *item = new ShortcutItem(m_searchGroup);
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

void ShortCutSettingWidget::keyPressEvent(QKeyEvent *event)
{
    QString A_Z_Def[26] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
    QString F1_F12_Def[12] = {"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12"};
    QString Key_Other_Def[20] = {"comma", "minus", "period", "slash", "apostrophe", "semicolon", "bracketleft", "bracketright", "Escape",
                                 "grave", "Tab", "Delete", "equal", "BackSpace", "backslash", "Return", "space"};
    QString ShiftModifier[3] = {"<Shift>", "Shift_L", "Shift_R"};
    QString ControlModifier[3] = {"<Control>", "Control_L", "Control_R"};
    QString AltModifier[3] = {"<Alt>", "Alt_L", "Alt_R"};
    QString SuperModifier[3] = {"<Super>", "Super_L", "Super_R"};
    if (Qt::Key_Shift == event->key()) {
        m_shiftKey = true;
    } else if (Qt::Key_Control == event->key()) {
        m_controlKey = true;
    } else if (Qt::Key_Alt == event->key()) {
        m_altKey = true;
    } else if (Qt::Key_Super_L == event->key()) {
        m_superKey = true;
    }

    QString shortcut;
    if (m_shiftKey) {
        shortcut += ShiftModifier[0];
    }
    if (m_controlKey) {
        shortcut += ControlModifier[0];
    }
    if (m_altKey) {
        shortcut += AltModifier[0];
    }
    if (m_superKey) {
        shortcut += SuperModifier[0];
    }

    if ((event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z)                                                                                                    ||
        (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)                                                                                                    ||
        (event->key() >= Qt::Key_F1 && event->key() <= Qt::Key_F12)                                                                                                 ||
        (event->key() == Qt::Key_Comma || event->key() == Qt::Key_Less) || (event->key() == Qt::Key_Minus || event->key() == Qt::Key_Underscore)                    ||
        (event->key() == Qt::Key_Period || event->key() == Qt::Key_Greater) || (event->key() == Qt::Key_Slash || event->key() == Qt::Key_Question)                  ||
        (event->key() == Qt::Key_Apostrophe || event->key() == Qt::Key_QuoteDbl) || (event->key() == Qt::Key_Semicolon || event->key() == Qt::Key_Colon)            ||
        (event->key() == Qt::Key_BracketLeft || event->key() == Qt::Key_BraceLeft) || (event->key() == Qt::Key_BracketRight || event->key() == Qt::Key_BraceRight)  ||
        (event->key() == Qt::Key_Escape) || (event->key() == Qt::Key_QuoteLeft || event->key() == Qt::Key_AsciiTilde)                                               ||
        (event->key() == Qt::Key_Tab) || (event->key() == Qt::Key_Delete)                                                                                           ||
        (event->key() == Qt::Key_Equal || event->key() == Qt::Key_Plus) || (event->key() == Qt::Key_Backspace)                                                      ||
        (event->key() == Qt::Key_Backslash || event->key() == Qt::Key_Bar) || (event->key() == Qt::Key_Return) || (event->key() == Qt::Key_Space)                          ) {
        if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) {
            shortcut += A_Z_Def[event->key() - Qt::Key_A];
        } else if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {
            shortcut += event->text();
        } else if (event->key() >= Qt::Key_F1 && event->key() <= Qt::Key_F12) {
            shortcut += F1_F12_Def[event->key() - Qt::Key_F1];
        } else if (event->key() == Qt::Key_Comma || event->key() == Qt::Key_Less) {
            shortcut += Key_Other_Def[0];
        } else if (event->key() == Qt::Key_Minus || event->key() == Qt::Key_Underscore) {
            shortcut += Key_Other_Def[1];
        } else if (event->key() == Qt::Key_Period || event->key() == Qt::Key_Greater) {
            shortcut += Key_Other_Def[2];
        } else if (event->key() == Qt::Key_Slash || event->key() == Qt::Key_Question) {
            shortcut += Key_Other_Def[3];
        } else if (event->key() == Qt::Key_Apostrophe  || event->key() == Qt::Key_QuoteDbl) {
            shortcut += Key_Other_Def[4];
        } else if (event->key() == Qt::Key_Semicolon || event->key() == Qt::Key_Colon) {
            shortcut += Key_Other_Def[5];
        } else if (event->key() == Qt::Key_BracketLeft || event->key() == Qt::Key_BraceLeft) {
            shortcut += Key_Other_Def[6];
        } else if (event->key() == Qt::Key_BracketRight || event->key() == Qt::Key_BraceRight) {
            shortcut += Key_Other_Def[7];
        } else if (event->key() == Qt::Key_Escape) {
            shortcut += Key_Other_Def[8];
        } else if (event->key() == Qt::Key_QuoteLeft || event->key() == Qt::Key_AsciiTilde) {
            shortcut += Key_Other_Def[9];
        } else if (event->key() == Qt::Key_Tab) {
            shortcut += Key_Other_Def[10];
        } else if (event->key() == Qt::Key_Delete) {
            shortcut += Key_Other_Def[11];
        } else if (event->key() == Qt::Key_Equal || event->key() == Qt::Key_Plus) {
            shortcut += Key_Other_Def[12];
        } else if (event->key() == Qt::Key_Backspace) {
            shortcut += Key_Other_Def[13];
        } else if (event->key() == Qt::Key_Backslash || event->key() == Qt::Key_Bar) {
            shortcut += Key_Other_Def[14];
        } else if (event->key() == Qt::Key_Return) {
            shortcut += Key_Other_Def[15];
        } else if (event->key() == Qt::Key_Space) {
            shortcut += Key_Other_Def[16];
        }

        if ((!(true == m_shiftKey && false == m_controlKey && false == m_altKey && false == m_superKey) && true == m_controlKey) ||
            (!(true == m_shiftKey && false == m_controlKey && false == m_altKey && false == m_superKey) && true == m_altKey)     ||
            (!(true == m_shiftKey && false == m_controlKey && false == m_altKey && false == m_superKey) && true == m_superKey))  {
            onKeyEvent(false, shortcut);
            return ;
        }
    }

    onKeyEvent(true, shortcut);
}

void ShortCutSettingWidget::keyReleaseEvent(QKeyEvent *event)
{
    QString Key_Other_Def[4] = {"Up", "Down ", "Left", "Right"};
    QString ShiftModifier[3] = {"<Shift>", "Shift_L", "Shift_R"};
    QString ControlModifier[3] = {"<Control>", "Control_L", "Control_R"};
    QString AltModifier[3] = {"<Alt>", "Alt_L", "Alt_R"};
    QString SuperModifier[3] = {"<Super>", "Super_L", "Super_R"};
    QString shortcut;
    if (m_shiftKey) {
        shortcut += ShiftModifier[0];
    }
    if (m_controlKey) {
        shortcut += ControlModifier[0];
    }
    if (m_altKey) {
        shortcut += AltModifier[0];
    }
    if (m_superKey) {
        shortcut += SuperModifier[0];
    }

    if ((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_Down) || (event->key() == Qt::Key_Left) || (event->key() == Qt::Key_Right)) {
        if (event->key() == Qt::Key_Up) {
            shortcut += Key_Other_Def[0];
        } else if (event->key() == Qt::Key_Down) {
            shortcut += Key_Other_Def[1];
        } else if (event->key() == Qt::Key_Left) {
            shortcut += Key_Other_Def[2];
        } else if (event->key() == Qt::Key_Right) {
            shortcut += Key_Other_Def[3];
        }
        if ((!(true == m_shiftKey && false == m_controlKey && false == m_altKey && false == m_superKey) && true == m_controlKey) ||
            (!(true == m_shiftKey && false == m_controlKey && false == m_altKey && false == m_superKey) && true == m_altKey)     ||
            (!(true == m_shiftKey && false == m_controlKey && false == m_altKey && false == m_superKey) && true == m_superKey))  {
            onKeyEvent(false, shortcut);
            return ;
        }
    }

    if (Qt::Key_Shift == event->key()) {
        m_shiftKey = false;
    } else if (Qt::Key_Control == event->key()) {
        m_controlKey = false;
    } else if (Qt::Key_Alt == event->key()) {
        m_altKey = false;
    } else if (Qt::Key_Super_L == event->key()) {
        m_superKey = false;
    }

    onKeyEvent(false, "");
}

void ShortCutSettingWidget::onKeyEvent(bool press, const QString &shortcut)
{
    qDebug() << " press " << press << " shortcut " << shortcut;
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

/**
 * @brief ShortCutSettingWidget::setGroupSettingVisible
 * @param enable
 * @details SettingsGroup大量频繁添加item时会因布局计算未完成而在屏幕上闪现item挤在一起的样子
 *          添加此函数以便在更新添加item时规避此问题
 */
void ShortCutSettingWidget::setGroupSettingVisible(bool enable)
{
    m_systemGroup->setVisible(enable);
    m_windowGroup->setVisible(enable);
    m_workspaceGroup->setVisible(enable);
    m_customGroup->setVisible(enable);
    m_searchGroup->setVisible(enable);
}

