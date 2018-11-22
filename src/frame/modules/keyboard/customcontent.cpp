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

#include "customcontent.h"
#include "keyboardcontrol.h"
#include "keyboardwork.h"
#include "shortcutmodel.h"
#include "widgets/translucentframe.h"
#include "keyboardmodel.h"
#include "dimagebutton.h"
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

DWIDGET_USE_NAMESPACE

namespace dcc
{
namespace keyboard
{

CustomContent::CustomContent(ShortcutModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_conflict(nullptr)
    , m_model(model)
    , m_buttonTuple(new ButtonTuple)
{
    setTitle(tr("Shortcuts"));
    TranslucentFrame *widget = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(10);
    layout->addSpacing(10);

    m_commandGroup = new SettingsGroup();
    m_name = new LineEditWidget();
    m_name->setTitle(tr("Name"));
    m_name->setPlaceholderText(tr("Required"));

    m_command = new LineEditWidget();
    m_command->setTitle(tr("Command"));
    m_command->setPlaceholderText(tr("Required"));

    DImageButton *pushbutton = new DImageButton;
    pushbutton->setNormalPic(":/keyboard/themes/dark/icons/loadfile_normal.svg");
    pushbutton->setHoverPic(":/keyboard/themes/dark/icons/loadfile_hover.svg");
    pushbutton->setPressPic(":/keyboard/themes/dark/icons/loadfile_press.svg");
    m_command->addRightWidget(pushbutton);

    m_shortcut = new CustomItem;
    m_shortcut->setShortcut("");

    m_commandGroup->appendItem(m_name);
    m_commandGroup->appendItem(m_command);
    m_commandGroup->appendItem(m_shortcut);

    layout->addWidget(m_commandGroup);

    QPushButton *cancel = m_buttonTuple->leftButton();
    cancel->setText(tr("Cancel"));
    QPushButton *ok = m_buttonTuple->rightButton();
    ok->setText(tr("Add"));

    m_bottomTip = new QLabel();
    m_bottomTip->setWordWrap(true);
    m_bottomTip->hide();

    layout->addWidget(m_buttonTuple);
    layout->addWidget(m_bottomTip);
    layout->addStretch();

    widget->setLayout(layout);
    setContent(widget);

    connect(cancel, SIGNAL(clicked()), this, SIGNAL(back()));
    connect(ok, SIGNAL(clicked()), this, SLOT(onShortcut()));
    connect(pushbutton, &DImageButton::clicked, this, &CustomContent::onOpenFile);
    connect(m_shortcut, &CustomItem::requestUpdateKey, this, &CustomContent::updateKey);
    connect(model, &ShortcutModel::keyEvent, this, &CustomContent::keyEvent);
}

void CustomContent::setBottomTip(ShortcutInfo *conflict)
{
    m_conflict = conflict;
    if (conflict) {
        QString accels = conflict->accels;
        accels = accels.replace("<", "");
        accels = accels.replace(">", "+");
        accels = accels.replace("_L","");
        accels = accels.replace("_R", "");
        accels = accels.replace("Control", "Ctrl");

        QString str = tr("This shortcut conflicts with %1, click on Add to make this shortcut effective immediately")
                .arg(QString("<span style=\"color: rgba(255, 90, 90, 1);\">%1 %2</span>").arg(conflict->name).arg(QString("[%1]").arg(accels)));
        m_bottomTip->setText(str);
        m_bottomTip->show();
    } else {
        m_bottomTip->clear();
        m_bottomTip->hide();
    }
}

void CustomContent::onShortcut()
{
    if (m_name->text().isEmpty())
        m_name->setIsErr(true);

    if (m_command->text().isEmpty())
        m_command->setIsErr(true);

    if (m_name->text().isEmpty() || m_command->text().isEmpty() || m_shortcut->text().isEmpty())
        return;

    if (m_conflict)
        Q_EMIT requestForceSubs(m_conflict);

    Q_EMIT requestAddKey(m_name->text(), m_command->text(), m_shortcut->text());

    Q_EMIT back();
}

void CustomContent::keyEvent(bool press, const QString &shortcut)
{
    if (!press) {
        if (shortcut.isEmpty() || shortcut == "BackSpace" || shortcut == "Delete") {
            m_shortcut->setShortcut("");
            setBottomTip(nullptr);
            return;
        }

        // check conflict
        ShortcutInfo *conflict = m_model->getInfo(shortcut);
        setBottomTip(conflict);
    }

    m_shortcut->setShortcut(shortcut);
}

void CustomContent::updateKey()
{
    Q_EMIT requestUpdateKey(nullptr);
}

void CustomContent::onOpenFile()
{
    Q_EMIT requestFrameAutoHide(false);

    QString file = QFileDialog::getOpenFileName(this, tr("Choose File"), "/usr/bin");
    m_command->setText(file);

    Q_EMIT requestFrameAutoHide(true);
}

}
}
