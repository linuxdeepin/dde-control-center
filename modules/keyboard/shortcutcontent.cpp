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

#include "shortcutcontent.h"
#include "shortcutmodel.h"
#include "keyboardwork.h"
#include "shortcutitem.h"
#include "keyboardcontrol.h"
#include "translucentframe.h"
#include "keyboardmodel.h"
#include <QVBoxLayout>

namespace dcc {
namespace keyboard{
ShortcutContent::ShortcutContent(KeyboardWork *work, QWidget *parent)
    :ContentWidget(parent),
      m_work(work),
      m_conflict(NULL),
      m_curInfo(NULL),
      m_buttonTuple(new ButtonTuple)
{
    TranslucentFrame* widget = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addSpacing(10);

    SettingsGroup* group = new SettingsGroup();
    m_item = new TitleButtonItem();
    m_item->setValue(tr("Please Reset Shortcut"));
    group->appendItem(m_item);
    m_control = new KeyboardControl();
//    group->appendItem(m_control);
    layout->addWidget(group);

    QPushButton *cancel = m_buttonTuple->leftButton();
    QPushButton *ok = m_buttonTuple->rightButton();

    cancel->setText(tr("Cancel"));
    ok->setText(tr("Replace"));

    m_bottomTip = new QLabel();
    m_bottomTip->setWordWrap(true);

    layout->addWidget(m_buttonTuple);
    layout->addWidget(m_bottomTip);
    layout->addStretch();
    widget->setLayout(layout);

    setContent(widget);

    connect(ok, SIGNAL(clicked()), this, SLOT(onReplace()));
    connect(cancel, SIGNAL(clicked()), this, SIGNAL(back()));
    connect(m_item,SIGNAL(click()), this, SLOT(onClick()));

    connect(m_work, &KeyboardWork::KeyEvent, this, &ShortcutContent::onKeyEvent);
}

void ShortcutContent::setBottomTip(ShortcutInfo* conflict)
{
    m_conflict = conflict;
    if(conflict)
    {
        QString str = tr("This shortcut conflicts with  %1, click on Replace to make this shortcut effective immediately").arg(conflict->name);
        m_bottomTip->setText(str);
        m_bottomTip->show();
    }
    else
    {
        m_bottomTip->clear();
        m_bottomTip->hide();
    }
}

void ShortcutContent::setCurInfo(ShortcutInfo *info)
{
    m_curInfo = info;
    m_item->setTitle(info->name);
}

void ShortcutContent::setConflictString(const QStringList &list)
{
    m_control->setConflictString(list);
}

void ShortcutContent::onClick()
{
    m_work->grabScreen();
    m_control->setFocus();

    m_bottomTip->clear();
    m_bottomTip->hide();

    for (int i = 0; i < ModelKeylist.count(); i++) {
        m_control->setConflicts(ModelKeylist.at(i), false);
    }
}

void ShortcutContent::onReplace()
{
    QString key = m_conflict->accels;
    m_work->modifyShortcut(m_conflict, tr("null"));
    m_work->modifyShortcut(m_curInfo, key);

    ShortcutItem * item = m_conflict->item;
    if (item) {
        m_conflict->item->displayConflict(true);
        m_curInfo->item->displayConflict();
    }
    sendBackSignal();
}

void ShortcutContent::onKeyEvent(const bool state, const QString &keylist)
{
    QString in = keylist;
    in.replace("<", "");
    in.replace(">", "-");
    in.replace("_L", "");
    QStringList value = in.split("-");
    QMap<QString, bool> list;
    for (QString key : ModelKeylist) {
        QStringList t;
        t << value << key;
        list.insert(key, m_work->keyOccupy(t));
    }

    for (int i = 0; i < ModelKeylist.size(); ++i)
        m_control->setPress(ModelKeylist.at(i), false);

    for (int i = 0; i < ModelKeylist.size(); ++i) {
        m_control->setPress(ModelKeylist.at(i), list[ModelKeylist.at(i)]);
    }
    if (!state && !keylist.isEmpty()){
        bool result = m_work->checkAvaliable(keylist);
        emit this->shortcut(keylist);
        if(result)
        {
            m_work->modifyShortcut(m_curInfo, keylist);
            sendBackSignal();
        }
    }
}

}
}
