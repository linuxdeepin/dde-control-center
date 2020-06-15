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

#include "customcontent.h"
#include "modules/keyboard/keyboardcontrol.h"
#include "modules/keyboard/keyboardwork.h"
#include "modules/keyboard/shortcutmodel.h"
#include "modules/keyboard/keyboardmodel.h"
#include "modules/keyboard/customitem.h"
#include "widgets/translucentframe.h"
#include "widgets/buttontuple.h"
#include "widgets/lineeditwidget.h"
#include "widgets/settingsgroup.h"

#include <dfilechooseredit.h>

#include <DLineEdit>

#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::keyboard;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;

CustomContent::CustomContent(ShortcutModel *model, QWidget *parent)
    : QWidget(parent)
    , m_conflict(nullptr)
    , m_model(model)
    , m_buttonTuple(new ButtonTuple(ButtonTuple::Save))
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);
    mainLayout->addSpacing(10);
    //~ contents_path /keyboard/Shortcuts
    QLabel *shortCutTitle = new QLabel(tr("Add Custom Shortcut"));
    shortCutTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(shortCutTitle);
    mainLayout->addSpacing(30);

    QLabel *shortCutName = new QLabel(tr("Name"));
    mainLayout->addWidget(shortCutName);

    m_shortCutNameEdit = new DLineEdit;
    m_shortCutNameEdit->lineEdit()->setPlaceholderText(tr("Required"));
    connect(m_shortCutNameEdit, &DLineEdit::textChanged, this, [this] {
        if (!m_shortCutNameEdit->text().isEmpty()) {
            m_shortCutNameEdit->setAlert(false);
        }
    });

    mainLayout->addWidget(m_shortCutNameEdit);

    QLabel *shortCutCmd = new QLabel(tr("Command"));
    mainLayout->addWidget(shortCutCmd);

    m_shortCutCmdEdit = new DFileChooserEdit(this);
    m_shortCutCmdEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_shortCutNameEdit->setAccessibleName("SHORTCUT_NAME_EDIT");
    m_shortCutCmdEdit->setAccessibleName("SHORTCUT_CMD_EDIT");

    connect(m_shortCutCmdEdit, &DFileChooserEdit::textChanged, this, [this] {
        if (!m_shortCutCmdEdit->text().isEmpty()) {
            m_shortCutCmdEdit->setAlert(false);
        }
    });

    mainLayout->addWidget(m_shortCutCmdEdit);

    m_shortcut = new CustomItem;
    m_shortcut->setShortcut("");

    mainLayout->addWidget(m_shortcut);

    QPushButton *cancel = m_buttonTuple->leftButton();
    cancel->setText(tr("Cancel"));
    QPushButton *ok = m_buttonTuple->rightButton();
    ok->setText(tr("Add"));

    m_bottomTip = new QLabel();
    m_bottomTip->setWordWrap(true);
    m_bottomTip->hide();

    mainLayout->addStretch();
    mainLayout->addWidget(m_buttonTuple);
    mainLayout->addWidget(m_bottomTip);
    setLayout(mainLayout);

    connect(cancel, &QPushButton::clicked, this, &CustomContent::back);
    connect(ok, &QPushButton::clicked, this, &CustomContent::onShortcut);
    connect(m_shortcut, &CustomItem::requestUpdateKey, this, &CustomContent::updateKey);
    connect(model, &ShortcutModel::keyEvent, this, &CustomContent::keyEvent);
    connect(m_shortcut, &CustomItem::changeAlert, this, [this] {
        m_shortcut->setAlert(false);
    });
}

void CustomContent::setBottomTip(ShortcutInfo *conflict)
{
    m_conflict = conflict;
    if (conflict) {
        QString accels = conflict->accels;
        accels = accels.replace("<", "");
        accels = accels.replace(">", "+");
        accels = accels.replace("_L", "");
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
    m_shortCutNameEdit->setAlert(m_shortCutNameEdit->text().isEmpty());
    m_shortCutCmdEdit->setAlert(m_shortCutCmdEdit->lineEdit()->text().isEmpty());
    m_shortcut->setAlert(m_shortcut->text().isEmpty());

    if (m_shortcut->text().isEmpty() || m_shortCutCmdEdit->lineEdit()->text().isEmpty() || m_shortCutNameEdit->text().isEmpty()) {
        return;
    }

    if (m_conflict)
        Q_EMIT requestForceSubs(m_conflict);

    Q_EMIT requestAddKey(m_shortCutNameEdit->text(), m_shortCutCmdEdit->text(), m_shortcut->text());
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
