/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "customedit.h"
#include "widgets/translucentframe.h"
#include "customitem.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

keyboard::CustomEdit::CustomEdit(ShortcutModel *model, QWidget *parent):
    ContentWidget(parent),
    m_model(model),
    m_commandGroup(new SettingsGroup),
    m_name(new LineEditWidget),
    m_command(new LineEditWidget),
    m_short(new CustomItem(this)),
    m_tip(new QLabel),
    m_conflict(nullptr)
{
    m_tip->setVisible(false);
    m_tip->setWordWrap(true);

    setTitle(tr("Shortcuts"));
    TranslucentFrame *widget = new TranslucentFrame;
    QVBoxLayout *mainlayout = new QVBoxLayout;
    QHBoxLayout *buttonlayout = new QHBoxLayout;

    mainlayout->setMargin(0);
    buttonlayout->setMargin(0);
    mainlayout->setSpacing(0);
    buttonlayout->setSpacing(1);

    m_command->setPlaceholderText(tr("Required"));
    DImageButton *pushbutton = new DImageButton;
    pushbutton->setNormalPic(":/keyboard/themes/dark/icons/loadfile_normal.svg");
    pushbutton->setHoverPic(":/keyboard/themes/dark/icons/loadfile_hover.svg");
    pushbutton->setPressPic(":/keyboard/themes/dark/icons/loadfile_press.svg");
    m_command->addRightWidget(pushbutton);

    m_commandGroup->appendItem(m_name);
    m_commandGroup->appendItem(m_command);
    m_commandGroup->appendItem(m_short);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Save"));

    buttonlayout->addWidget(cancelButton);
    buttonlayout->addWidget(okButton);

    mainlayout->addSpacing(10);
    mainlayout->addWidget(m_commandGroup);
    mainlayout->addWidget(m_tip);
    mainlayout->addSpacing(10);
    mainlayout->addLayout(buttonlayout);

    widget->setLayout(mainlayout);

    setContent(widget);

    connect(cancelButton, &QPushButton::clicked, this, &CustomEdit::back);
    connect(pushbutton, &DImageButton::clicked, this, &CustomEdit::onOpenFile);
    connect(m_short, &CustomItem::requestUpdateKey, this, &CustomEdit::onUpdateKey);
    connect(okButton, &QPushButton::clicked, this, &CustomEdit::onSaveAccels);

    connect(model, &ShortcutModel::keyEvent, this, &CustomEdit::keyEvent);
}

void keyboard::CustomEdit::setShortcut(keyboard::ShortcutInfo *info)
{
    m_info = info;

    m_short->setTitle(tr("Shortcuts"));
    m_short->setShortcut(info->accels);

    m_name->setTitle(tr("Name"));
    m_command->setTitle(tr("Command"));

    m_name->setText(m_info->name);
    m_command->setText(m_info->command);
}

void keyboard::CustomEdit::setBottomTip(keyboard::ShortcutInfo *conflict)
{
    m_conflict = conflict;
    if (conflict) {
        QString accels = conflict->accels;
        accels = accels.replace("<", "[");
        accels = accels.replace(">", "+");
        accels = accels.replace("_L","");
        accels = accels.replace("_R", "");
        accels = accels.replace("Control", "Ctrl");

        QString str = tr("This shortcut conflicts with %1, click on Add to make this shortcut effective immediately")
                .arg(QString("<span style=\"color: rgba(255, 90, 90, 1);\">%1 %2</span>").arg(conflict->name).arg(QString("[%1]").arg(accels)));
        m_tip->setText(str);
        m_tip->setVisible(true);
    } else {
        m_tip->setVisible(false);
    }
}

void keyboard::CustomEdit::keyEvent(bool press, const QString &shortcut)
{
    m_short->setShortcut(shortcut);

    if (!press) {

        if (shortcut.isEmpty()) {
            m_short->setShortcut(m_info->accels);
            setBottomTip(nullptr);
            return;
        }

        if (shortcut == "BackSpace" || shortcut == "Delete") {
            m_short->setShortcut("");
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
    }
}

void keyboard::CustomEdit::onOpenFile()
{
    Q_EMIT requestFrameAutoHide(false);

    QString file = QFileDialog::getOpenFileName(this, tr("Choose File"), "/usr/bin");
    m_command->setText(file);

    Q_EMIT requestFrameAutoHide(true);
}

void keyboard::CustomEdit::onSaveAccels()
{
    if (m_name->text().isEmpty())
        m_name->setIsErr(true);

    if (m_command->text().isEmpty())
        m_command->setIsErr(true);

    if (m_name->text().isEmpty() || m_command->text().isEmpty() || m_short->text().isEmpty())
        return;

    if (m_conflict) {
        m_info->replace = m_conflict;
    }

    m_info->name = m_name->text();
    m_info->command = m_command->text();
    m_info->accels = m_short->text();

    Q_EMIT requestSaveShortcut(m_info);

    Q_EMIT back();
}

void keyboard::CustomEdit::onUpdateKey()
{
    Q_EMIT requestUpdateKey(nullptr);
}
