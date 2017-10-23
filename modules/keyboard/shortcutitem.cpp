/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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
      m_contain(false),
      m_display(false),
      m_info(NULL)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setMinimumHeight(36);
    setFixedWidth(344);

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

    m_inter = new KeybingdingInter("com.deepin.daemon.Keybinding",
                                          "/com/deepin/daemon/Keybinding",
                                          QDBusConnection::sessionBus(), this);

    connect(m_inter, &KeybingdingInter::KeyEvent, this, &ShortcutItem::onKeyEvent);
    connect(m_checkBtn, SIGNAL(clicked()), this, SLOT(onChecked()));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));
    connect(m_editBtn, &DImageButton::clicked, this, &ShortcutItem::onShortcutEdit);
    connect(m_inter, &KeybingdingInter::Changed, this, &ShortcutItem::updateShortcutKeys);
}

void ShortcutItem::setShortcutInfo(ShortcutInfo *info)
{
    m_info = info;

    updateShortcutKeys();
}

void ShortcutItem::displayConflict(bool display)
{
    m_display = display;
}

ShortcutInfo *ShortcutItem::curInfo()
{
    return m_info;
}

void ShortcutItem::setTitle(const QString &title)
{
    m_title->setText(title);

    m_key->adjustSize();

    int v = width() - m_key->width() - 32;
    if (m_title->fontMetrics().width(m_title->text()) > v)
        m_title->setFixedWidth(v);
}

void ShortcutItem::onFocusChanged(QWidget *old, QWidget *now)
{
    if(!old && now != m_shortcutEdit)
        m_shortcutEdit->hide();
}

void ShortcutItem::onKeyEvent(bool press, QString shortcut)
{
    if(!press)
    {
        if(!m_shortcutEdit->isVisible())
            return;

        if(shortcut.isEmpty()){
            m_shortcutEdit->hide();
            return;
        }

        if(shortcut == "BackSpace" || shortcut == "Delete"){
            m_shortcutEdit->hide();
            emit requestDisableShortcut(m_info);
        }else{
            m_shortcutEdit->hide();
            if(m_info->accels != shortcut)
            {
                const QString &result = m_inter->LookupConflictingShortcut(shortcut);

                emit shortcutChangd(result.isEmpty(), m_info, shortcut);
            }
        }
    }
    else
    {
        if(shortcut.isEmpty())
            m_shortcutEdit->hide();
    }

    // update shortcut keys
    updateShortcutKeys();
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

void ShortcutItem::onChecked()
{
    emit destroyed();
}

void ShortcutItem::onShortcutEdit()
{
    emit shortcutEditChanged(m_info);
}

void ShortcutItem::updateShortcutKeys()
{
    QString accels;

    accels = m_info->accels;
    accels = accels.replace("<", "");
    accels = accels.replace(">", "-");

    m_key->show();
    m_key->setTextList(accels.split("-"));
}

//void ShortcutItem::paintEvent(QPaintEvent *e)
//{
//    SettingsItem::paintEvent(e);
//    if(m_shortcutEdit->isVisible() || m_checkBtn->isVisible())
//    {
//       return;
//    }

//    QPainter painter(this);
//    painter.setRenderHint(QPainter::Antialiasing);

//    QFont font = qApp->font();
//    QFontMetrics fm(font);
//    QString accels;
//    accels = m_info->accels;
//    accels = accels.replace("Control", "Ctrl");
//    accels = accels.replace("<", "");
//    accels = accels.replace(">", "-");
//    accels = accels.replace("Above_Tab", "`");
//    accels = accels.replace("Super_L", "Super");
//    accels = accels.replace("Up", "↑");
//    accels = accels.replace("Down", "↓");
//    accels = accels.replace("Left", "←");
//    accels = accels.replace("Right", "→");

//    QString conflict(tr("Conflict"));
//    QStringList splits = accels.split("-");
//    int right = this->rect().right() - 10;
//    for(int i = splits.count() - 1; i>=0; --i)
//    {

//        QString str = splits.at(i);
//        int w;
//        if (str == "null" && m_display) {
//            w = fm.width(conflict);
//        } else {
//            w = fm.width(str);
//        }
//        int h = (height() - fm.height()-6)/2;
//        QRect r(0,0,w+8,height());
//        r.moveRight(right);

//        right = right - w - 12;
//        painter.setBrush(palette().color(QPalette::Window));
//        painter.save();
//        painter.setPen(Qt::NoPen);
//        painter.drawRoundRect(r.adjusted(0,h,0,-h));
//        painter.restore();
//        painter.save();
//        painter.setRenderHint(QPainter::Antialiasing);
//        painter.setOpacity(m_contain ? 1.0 : 0.85);
//        if(str == "null" && m_display)
//        {
//            QPen pen = painter.pen();
//            QColor col = pen.color();
//            pen.setColor(Qt::red);
//            painter.setPen(pen);
//            painter.drawText(r,Qt::AlignCenter,tr("Conflict"));
//            pen.setColor(col);
//            painter.setPen(pen);
//        }
//        else
//            painter.drawText(r,Qt::AlignCenter,str);

//        painter.restore();
//        if(i == 0)
//        {
//            m_rect = QRect(r.topLeft(),this->rect().bottomRight());
//            m_rect = m_rect.adjusted(0,h,0,-h);
//        }
//    }
//}

void ShortcutItem::mousePressEvent(QMouseEvent *e)
{
    if (m_checkBtn->isVisible())
        return;

    if(!m_shortcutEdit->isVisible() && m_key->rect().contains(m_key->mapFromParent(e->pos())))
    {
        m_key->hide();
        m_inter->GrabScreen();
        m_shortcutEdit->setFocus();
        m_shortcutEdit->show();
        m_info->item = this;
    }
    else
    {
        m_shortcutEdit->hide();
        m_key->show();
    }
}

}
}
