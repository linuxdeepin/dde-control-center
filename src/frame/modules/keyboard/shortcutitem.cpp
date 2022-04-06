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
#include "keylabel.h"

#include <DStyle>

#include <QLabel>
#include <QMouseEvent>
#include <QLineEdit>
#include <QTimer>

using namespace dcc;
DWIDGET_USE_NAMESPACE
using namespace dcc::keyboard;

ShortcutItem::ShortcutItem(QFrame *parent)
    : SettingsItem(parent)
    , m_info(nullptr)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    setMinimumHeight(36);

    setMouseTracking(true);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(20, 2, 10, 2);
    layout->setSpacing(2);

    m_title = new QLabel();
    m_title->setText("");
    m_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_title->setWordWrap(false);
    m_title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);

    m_editBtn = new DIconButton(this);
    m_editBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_editBtn->hide();
    m_editBtn->setFixedSize(16, 16);
    m_editBtn->setAccessibleName("EDIT_SHORTCUT_BUTTON");
    layout->addWidget(m_editBtn, Qt::AlignLeft);

    layout->addStretch();

    m_delBtn = new DIconButton(this);
    m_delBtn->setIcon(DStyle::StandardPixmap::SP_DeleteButton);
    m_delBtn->hide();
    m_delBtn->setFixedSize(16, 16);
    m_delBtn->setAccessibleName("DEL_SHORTCUT_BUTTON");
    layout->addWidget(m_delBtn, Qt::AlignLeft);

    m_key = new ShortcutKey;
    layout->addWidget(m_key);

    m_shortcutEdit = new QLineEdit;
    m_shortcutEdit->setReadOnly(true);
    layout->addWidget(m_shortcutEdit, 0, Qt::AlignVCenter | Qt::AlignRight);
    m_shortcutEdit->setPlaceholderText(tr("Enter a new shortcut"));
    m_shortcutEdit->hide();

    setLayout(layout);

    connect(m_editBtn, &DIconButton::clicked, this, &ShortcutItem::onShortcutEdit);
    connect(m_delBtn, &DIconButton::clicked, this, &ShortcutItem::onRemoveClick);
}

void ShortcutItem::setShortcutInfo(ShortcutInfo *info)
{
    m_info = info;
    setTitle(m_info->name);
    setShortcut(info->accels);
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
    accels = accels.replace("_L", "");
    accels = accels.replace("_R", "");
    accels = accels.replace("Control", "Ctrl");

    QStringList alist = accels.split("-");
    if (alist.count() > 1) {
        if (alist[alist.count() - 1] == "") {
            alist.removeAt(alist.count() - 1);
        }
    }
    m_key->setTextList(alist);
}

void ShortcutItem::onEditMode(bool value)
{
    if (value) {
        m_delBtn->show();
        m_editBtn->show();
        m_key->hide();
    } else {
        m_delBtn->hide();
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

    if (m_title->fontMetrics().width(m_title->text()) > v) {
        QFontMetrics fontWidth(m_title->font());
        QString elideNote = fontWidth.elidedText(m_title->text(), Qt::ElideRight, v / 2);
        m_title->setText(elideNote);
    }

}

void ShortcutItem::mouseReleaseEvent(QMouseEvent *e)
{
    // TODO：klu暂时屏蔽修改快捷键功能
    Q_UNUSED(e)
    return;

    if (m_delBtn->isVisible())
        return;

    if (!m_shortcutEdit->isVisible() && m_key->rect().contains(m_key->mapFromParent(e->pos()))) {
        m_key->hide();
        m_shortcutEdit->show();
        m_info->item = this;

        Q_EMIT requestUpdateKey(m_info);
    } else {
        m_shortcutEdit->hide();
        m_key->show();
    }
}

void ShortcutItem::resizeEvent(QResizeEvent *event)
{
    SettingsItem::resizeEvent(event);

    int v = width() - m_key->width() - 32;

    if (m_title->fontMetrics().width(m_title->text()) <= v) {
        QTimer::singleShot(0, this, [=](){
            m_title->setText(m_info->name);
        });
    } else {
        QTimer::singleShot(0, this, &ShortcutItem::updateTitleSize);
    }

}