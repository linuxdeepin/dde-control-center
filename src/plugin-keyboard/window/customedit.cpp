//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "customedit.h"
#include "customitem.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

#include <DIconButton>
#include <DTitlebar>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
CustomEdit::CustomEdit(ShortcutModel *model, QWidget *parent):
    DAbstractDialog(parent),
    m_model(model),
    m_commandGroup(new SettingsGroup),
    m_name(new LineEditWidget),
    m_command(new LineEditWidget),
    m_short(new CustomItem()),
    m_tip(new QLabel),
    m_conflict(nullptr)
{
    setFixedSize(QSize(400, 388));
    m_tip->setVisible(false);
    m_tip->setWordWrap(true);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    QHBoxLayout *buttonlayout = new QHBoxLayout;

    mainlayout->setMargin(0);
    buttonlayout->setMargin(0);
    mainlayout->setSpacing(0);
    buttonlayout->setSpacing(1);

    m_command->setPlaceholderText(tr("Required"));
    DIconButton *pushbutton = new DIconButton(this);
    pushbutton->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_SelectElement, nullptr));
    pushbutton->setBackgroundRole(QPalette::ColorRole::Highlight);
    pushbutton->setIconSize(QSize(24, 24));
    m_command->addRightWidget(pushbutton);

    m_commandGroup->appendItem(m_name);
    m_commandGroup->appendItem(m_command);
    m_commandGroup->appendItem(m_short);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *okButton = new QPushButton(tr("Save"));

    buttonlayout->addWidget(cancelButton);
    buttonlayout->addWidget(okButton);

    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setTitle(tr(""));

    mainlayout->addWidget(titleIcon);
    mainlayout->addSpacing(10);
    mainlayout->addWidget(m_commandGroup);
    mainlayout->addWidget(m_tip);
    mainlayout->addStretch();
    mainlayout->addLayout(buttonlayout);
    mainlayout->setContentsMargins(20, 10, 20, 10);

    setLayout(mainlayout);

    connect(cancelButton, &QPushButton::clicked, this, &CustomEdit::reject);
    connect(pushbutton, &DIconButton::clicked, this, &CustomEdit::onOpenFile);
    connect(m_short, &CustomItem::requestUpdateKey, this, &CustomEdit::onUpdateKey);
    connect(okButton, &QPushButton::clicked, this, &CustomEdit::onSaveAccels);

    connect(model, &ShortcutModel::keyEvent, this, &CustomEdit::keyEvent);
}

void CustomEdit::setShortcut(ShortcutInfo *info)
{
    m_info = info;

    m_short->setTitle(tr("Shortcut"));
    m_short->setShortcut(info->accels);

    m_name->setTitle(tr("Name"));
    m_command->setTitle(tr("Command"));

    m_name->setText(m_info->name);
    m_command->setText(m_info->command);
}

void CustomEdit::setBottomTip(ShortcutInfo *conflict)
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

void CustomEdit::keyEvent(bool press, const QString &shortcut)
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

void CustomEdit::onOpenFile()
{
    Q_EMIT requestFrameAutoHide(false);

    QString file = QFileDialog::getOpenFileName(this, "", "/usr/bin");
    m_command->setText(file);

    Q_EMIT requestFrameAutoHide(true);
}

void CustomEdit::onSaveAccels()
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

    accept();
}

void CustomEdit::onUpdateKey()
{
    Q_EMIT requestUpdateKey(nullptr);
}
