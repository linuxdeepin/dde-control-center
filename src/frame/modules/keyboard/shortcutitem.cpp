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

#include "shortcutitem.h"
#include "shortcutmodel.h"
#include <QLabel>
#include <QMouseEvent>
#include <QLineEdit>
#include <QApplication>
#include <QPainter>

#include "keylabel.h"

using namespace dcc;

namespace dcc {
namespace keyboard{

ShortcutItem::ShortcutItem(QFrame *parent)
    :SettingsItem(parent),
      m_info(NULL)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    setMinimumHeight(36);

    setMouseTracking(true);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(20,2,10,2);
    layout->setSpacing(2);

    m_title = new QLabel();
    m_title->setText("");
    m_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_title->setWordWrap(true);
    m_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);

    m_editBtn = new DImageButton;
    m_editBtn->setNormalPic(":/keyboard/themes/common/icons/edit_normal.svg");
    m_editBtn->setPressPic(":/keyboard/themes/common/icons/edit_press.svg");
    m_editBtn->setHoverPic(":/keyboard/themes/common/icons/edit_hover.svg");
    m_editBtn->hide();
    layout->addWidget(m_editBtn, 1, Qt::AlignLeft);

    m_checkBtn = new DImageButton();
    m_checkBtn->setNormalPic(":/keyboard/themes/dark/icons/list_delete_normal.svg");
    m_checkBtn->setHoverPic(":/keyboard/themes/dark/icons/list_delete_hover.svg");
    m_checkBtn->setPressPic(":/keyboard/themes/dark/icons/list_delete_press.svg");

    layout->addStretch();
    layout->addWidget(m_checkBtn);
    layout->setAlignment(m_checkBtn, Qt::AlignVCenter | Qt::AlignRight);
    m_checkBtn->hide();

    m_key = new ShortcutKey;
    layout->addWidget(m_key);

    m_shortcutEdit = new QLineEdit;
    m_shortcutEdit->setReadOnly(true);
    layout->addWidget(m_shortcutEdit, 0, Qt::AlignVCenter | Qt::AlignRight);
    m_shortcutEdit->setPlaceholderText(tr("Please enter a new shortcut"));
    m_shortcutEdit->hide();

    setLayout(layout);

    connect(m_checkBtn, &DImageButton::clicked, this, &ShortcutItem::onRemoveClick);
    connect(m_editBtn, &DImageButton::clicked, this, &ShortcutItem::onShortcutEdit);
}

void ShortcutItem::setShortcutInfo(ShortcutInfo *info)
{
    m_info = info;

    m_title->setText(info->name);

    setShortcut(info->accels);
}

ShortcutInfo *ShortcutItem::curInfo()
{
    return m_info;
}

void ShortcutItem::setTitle(const QString &title)
{
    m_title->setText(title);
    QTimer::singleShot(0, this, &ShortcutItem::updateTitleSize);
}

void ShortcutItem::setShortcut(const QString &shortcut)
{
    m_shortcutEdit->hide();
    m_key->show();

    QString accels = shortcut;
    accels = accels.replace("<", "");
    accels = accels.replace(">", "-");
    accels = accels.replace("_L","");
    accels = accels.replace("_R", "");
    accels = accels.replace("Control", "Ctrl");

    m_key->setTextList(accels.split("-"));
}

void ShortcutItem::onEditMode(bool value)
{
    if(value)
    {
        m_checkBtn->show();
        m_editBtn->show();
        m_key->hide();
    }
    else
    {
        m_checkBtn->hide();
        m_editBtn->hide();
        m_key->show();
    }
    update();
}

void ShortcutItem::onRemoveClick()
{
    Q_EMIT requestRemove(m_info);
}

void ShortcutItem::onShortcutEdit()
{
    Q_EMIT shortcutEditChanged(m_info);
}

void ShortcutItem::updateTitleSize()
{
    show();

    int v = width() - m_key->width() - 32;

    if (m_title->fontMetrics().width(m_title->text()) > v)
        m_title->setFixedWidth(v / 2);
}

void ShortcutItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (m_checkBtn->isVisible())
        return;

    if(!m_shortcutEdit->isVisible() && m_key->rect().contains(m_key->mapFromParent(e->pos())))
    {
        m_key->hide();
        m_shortcutEdit->show();
        m_info->item = this;

        Q_EMIT requestUpdateKey(m_info);
    }
    else
    {
        m_shortcutEdit->hide();
        m_key->show();
    }
}

}
}
