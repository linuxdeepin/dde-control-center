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
ShortcutContent::ShortcutContent(ShortcutModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(model)
    , m_conflict(NULL)
    , m_buttonTuple(new ButtonTuple)
{
    TranslucentFrame* widget = new TranslucentFrame();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addSpacing(10);

    SettingsGroup* group = new SettingsGroup();
    m_item = new TitleButtonItem();
    m_item->setValue(tr("Please Reset Shortcut"));
    group->appendItem(m_item);
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

    connect(ok, &QPushButton::clicked, this, &ShortcutContent::onReplace);
    connect(cancel, &QPushButton::clicked, this, &ShortcutContent::back);
    connect(m_item, &TitleButtonItem::click, this, &ShortcutContent::onUpdateKey);
    connect(model, &ShortcutModel::keyEvent, this, &ShortcutContent::keyEvent);
}

void ShortcutContent::setBottomTip(ShortcutInfo *conflict)
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

void ShortcutContent::setInfo(ShortcutInfo *info)
{
    m_info = info;
    m_item->setTitle(info->name);
}

void ShortcutContent::setShortcut(const QString &shortcut)
{
    m_shortcut = shortcut;
}

void ShortcutContent::keyEvent(bool press, const QString &shortcut)
{
    if (!press) {

        if (shortcut.isEmpty()) {
            setBottomTip(nullptr);
            return;
        }

        if(shortcut == "BackSpace" || shortcut == "Delete") {
            m_shortcut.clear();
            setBottomTip(nullptr);
            return;
        }

        // check conflict
        ShortcutInfo *info = m_model->getInfo(shortcut);
        if (info && info != m_info && info->accels != m_info->accels) {
            setBottomTip(info);
            return;
        }
        setBottomTip(nullptr);
        m_shortcut = shortcut;
    }
}

void ShortcutContent::onReplace()
{
    if (m_info->accels != m_shortcut) {
        if (m_shortcut.isEmpty()) {
            emit requestDisableShortcut(m_info);
        } else {
            m_info->accels = m_shortcut;
            emit requestSaveShortcut(m_info);
        }
    }

    emit back();
}

void ShortcutContent::onUpdateKey()
{
    emit requestUpdateKey(nullptr);
}

}
}
