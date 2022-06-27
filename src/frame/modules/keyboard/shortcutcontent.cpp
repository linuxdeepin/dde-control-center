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
#include "widgets/translucentframe.h"
#include "keyboardmodel.h"
#include "window/modules/keyboard/waylandgrab.h"
#include "window/utils.h"

#include <QVBoxLayout>
#include <QGuiApplication>
#include <QKeyEvent>

using namespace DCC_NAMESPACE;

namespace dcc {
namespace keyboard {
ShortcutContent::ShortcutContent(ShortcutModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(model)
    , m_conflict(nullptr)
    , m_shortcutItem(new ShortcutItem)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
    , m_waylandGrab(nullptr)
{
    if (QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)) {
        m_waylandGrab = new WaylandGrab(this->topLevelWidget());
    }
    TranslucentFrame *widget = new TranslucentFrame();
    setContentsMargins(10, 10, 10, 10);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    m_shortcutItem->setShortcut(tr("Please Reset Shortcut"));
    m_shortcutItem->addBackground();
    layout->addWidget(m_shortcutItem, 0, Qt::AlignTop);

    layout->addSpacing(10);
    QPushButton *cancel = m_buttonTuple->leftButton();
    QPushButton *ok = m_buttonTuple->rightButton();
    if (m_buttonTuple->layout()) {
        //第二个控件为space
        if (m_buttonTuple->layout()->itemAt(1) != nullptr && m_buttonTuple->layout()->itemAt(1)->spacerItem() != nullptr) {
            int height = m_buttonTuple->layout()->itemAt(1)->spacerItem()->sizeHint().height();
            m_buttonTuple->layout()->itemAt(1)->spacerItem()->changeSize(20, height);
        }
    }

    cancel->setText(tr("Cancel"));
    ok->setText(tr("Replace"));

    setTitle(tr("Shortcuts"));

    m_bottomTip = new QLabel();
    m_bottomTip->setWordWrap(true);

    layout->addWidget(m_buttonTuple);
    layout->addWidget(m_bottomTip);
    layout->addStretch();
    widget->setLayout(layout);

    setContent(widget);

    connect(ok, &QPushButton::clicked, this, [this]{
        if (parentWidget())
            this->parentWidget()->setFocus();
        Q_EMIT onReplace();
    });
    connect(cancel, &QPushButton::clicked, this, [this]{
        if (parentWidget())
            this->parentWidget()->setFocus();
        Q_EMIT back();
    });
    connect(m_shortcutItem, &ShortcutItem::requestUpdateKey, this, &ShortcutContent::onUpdateKey);
    connect(model, &ShortcutModel::keyEvent, this, &ShortcutContent::keyEvent);
}

void ShortcutContent::setBottomTip(ShortcutInfo *conflict)
{
    m_conflict = conflict;

    m_info->replace = conflict;

    if (conflict) {
        QString accels = conflict->accels;
        accels = accels.replace("<", "");
        accels = accels.replace(">", "+");
        accels = accels.replace("_L", "");
        accels = accels.replace("_R", "");
        accels = accels.replace("Control", "Ctrl");

        QString str = tr("This shortcut conflicts with  %1, click on Replace to make this shortcut effective immediately")
                      .arg(QString("<span style=\"color: rgba(255, 90, 90, 1);\">%1 %2</span>").arg(conflict->name).arg(QString("[%1]").arg(accels)));
        m_bottomTip->setText(str);
        m_bottomTip->show();
    } else {
        m_bottomTip->clear();
        m_bottomTip->hide();
    }
}

void ShortcutContent::setInfo(ShortcutInfo *info)
{
    m_info = info;
    m_shortcutItem->setShortcutInfo(info);
}

void ShortcutContent::setShortcut(const QString &shortcut)
{
    m_shortcut = shortcut;
    m_shortcutItem->setShortcut(shortcut);
}

void ShortcutContent::setConflictShortcut(const QString &shortcut)
{
    m_conflictShortcut = shortcut;
}

void ShortcutContent::keyEvent(bool press, const QString &shortcut)
{
    if (!press) {

        if (shortcut.isEmpty()) {
            setBottomTip(m_shortcut == m_conflictShortcut ? m_conflict : nullptr);
            m_shortcutItem->setShortcut(m_shortcut);
            return;
        }

        if (shortcut == "BackSpace" || shortcut == "Delete") {
            m_shortcut.clear();
            setBottomTip(nullptr);
            return;
        }

        m_shortcut = shortcut;

        // check conflict
        ShortcutInfo *info = m_model->getInfo(shortcut);
        if (info && info != m_info && info->accels != m_info->accels) {
            m_shortcutItem->setShortcut(info->accels);
            setBottomTip(info);
            setConflictShortcut(shortcut);
            return;
        }
    }

    if (!shortcut.isEmpty()) {
        setBottomTip(nullptr);
        m_shortcutItem->setShortcut(shortcut);
    }
}

void ShortcutContent::onReplace()
{
    if (m_info->accels != m_shortcut) {
        if (m_shortcut.isEmpty()) {
            Q_EMIT requestDisableShortcut(m_info);
        } else {
            m_info->accels = m_shortcut;
            Q_EMIT requestSaveShortcut(m_info);
        }
    }

    Q_EMIT back();
}

void ShortcutContent::onUpdateKey()
{
    Q_EMIT requestUpdateKey(nullptr);
}

void ShortcutContent::onGrab(ShortcutInfo *info)
{
    if (m_waylandGrab)
        m_waylandGrab->onGrab(info);
}

void ShortcutContent::keyPressEvent(QKeyEvent *ke)
{
    if (!QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)
            || !m_waylandGrab || !m_waylandGrab->getZxgm()) {
        return;
    }
    unsigned int keKey = static_cast<unsigned int>(ke->key());
    m_waylandGrab->setKeyValue(WaylandkeyMap[keKey]);
    QString lastKey = m_waylandGrab->getLastKey();
    QString keyValue = m_waylandGrab->getKeyValue();

    m_waylandGrab->setRecordState(true);
    keyEvent(true, m_waylandGrab->getRecordState() ? lastKey + keyValue : keyValue);
    if (keKey == Qt::Key_Control || keKey == Qt::Key_Alt
            || keKey == Qt::Key_Shift || keKey == Qt::Key_Super_L || keKey == Qt::Key_Super_R) {
        lastKey += ("<" + keyValue.remove(keyValue.indexOf("_"), 2) + ">");
        m_waylandGrab->setLastKey(lastKey);
    }
    return QWidget::keyPressEvent(ke);
}

void ShortcutContent::keyReleaseEvent(QKeyEvent *ke)
{
    if (!QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)
            || !m_waylandGrab || !m_waylandGrab->getZxgm()|| !m_waylandGrab->getRecordState()) {
        return;
    }
    QString lastKey = m_waylandGrab->getLastKey();
    QString keyValue = m_waylandGrab->getKeyValue();
    if (!lastKey.isEmpty()) {
        if (WaylandkeyMap[Qt::Key_Control] == keyValue
                || WaylandkeyMap[Qt::Key_Alt] == keyValue || WaylandkeyMap[Qt::Key_Shift] == keyValue) {
            keyEvent(false, "");
        } else if (WaylandkeyMap[Qt::Key_Super_L] == keyValue || WaylandkeyMap[Qt::Key_Super_R] == keyValue) {
            keyEvent(false, "Super_L");
        } else {
            keyEvent(false, lastKey + keyValue);
        }
    } else {
        keyEvent(false, "");
    }
    m_waylandGrab->setLastKey("");
    m_waylandGrab->setRecordState(false);
    m_waylandGrab->onUnGrab();
    return QWidget::keyReleaseEvent(ke);
}

void ShortcutContent::mousePressEvent(QMouseEvent *e)
{
    if (!QGuiApplication::platformName().startsWith("wayland", Qt::CaseInsensitive)
            || !m_waylandGrab || !m_waylandGrab->getZxgm()) {
        return;
    }
    setFocus();
    if (!m_waylandGrab->getRecordState()) {
        m_waylandGrab->onUnGrab();
    }
    QWidget::mousePressEvent(e);
}

}
}
