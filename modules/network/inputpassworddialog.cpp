/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QVBoxLayout>
#include <QEventLoop>
#include <QShortcut>
#include <QDebug>

#include <libdui/dtextbutton.h>
#include <libdui/dthememanager.h>

#include "constants.h"
#include "inputpassworddialog.h"

InputPasswordDialog::InputPasswordDialog(QWidget *parent) :
    QFrame(parent)
{
    D_THEME_INIT_WIDGET(InputPasswordDialog);

    m_edit = new DPasswordEdit;
    m_edit->setFixedWidth(DCC::ModuleContentWidth - 60);
    m_edit->setPlaceholderText(tr("Password"));

    m_checkBox = new DCheckBox(tr("Auto-connect"));
    m_checkBox->setChecked(true);

    QHBoxLayout *h_layout = new QHBoxLayout;
    h_layout->setMargin(0);
    h_layout->setSpacing(0);

    DTextButton *cancel_button = new DTextButton(tr("Cancel"));
    DTextButton *connect_button = new DTextButton(tr("Connect"));

    h_layout->addStretch(1);
    h_layout->addWidget(cancel_button);
    h_layout->addSpacing(5);
    h_layout->addWidget(connect_button);

    m_mainWidget = new DVBoxWidget(this);

    m_mainWidget->layout()->setMargin(10);
    m_mainWidget->layout()->setSpacing(10);
    m_mainWidget->addWidget(m_edit);
    m_mainWidget->addWidget(m_checkBox);
    m_mainWidget->layout()->addLayout(h_layout);

    updateMainWidgetPos();

    connect(cancel_button, &DTextButton::clicked, this, &InputPasswordDialog::cancel);
    connect(connect_button, &DTextButton::clicked, this, &InputPasswordDialog::confirm);
    connect(this, &InputPasswordDialog::setText, m_edit, &DPasswordEdit::setText);
    connect(m_edit, &DPasswordEdit::textChanged, this, &InputPasswordDialog::textChanged);
    connect(m_mainWidget, &DVBoxWidget::sizeChanged, this, [this]{
        setFixedHeight(m_mainWidget->height());
        updateMainWidgetPos();
    });

    QShortcut *shortcut_enter = new QShortcut(Qt::Key_Enter, this);
    QShortcut *shortcut_return = new QShortcut(Qt::Key_Return, this);

    connect(shortcut_enter, &QShortcut::activated, this, &InputPasswordDialog::confirm);
    connect(shortcut_return, &QShortcut::activated, this, &InputPasswordDialog::confirm);

    setFocusProxy(m_edit);
    setFocusPolicy(Qt::StrongFocus);
}

InputPasswordDialog::~InputPasswordDialog()
{
    done(-1);
}

bool InputPasswordDialog::autoConnect() const
{
    return m_checkBox->checkState() != Qt::Unchecked;
}

QString InputPasswordDialog::text() const
{
    return m_edit->text();
}

void InputPasswordDialog::setInputAlert(bool alert)
{
    m_edit->setAlert(alert);
}

void InputPasswordDialog::setAutoConnect(bool autoConnect)
{
    m_checkBox->setChecked(autoConnect);
}

int InputPasswordDialog::exec()
{
    if(m_eventLoop)
        return 0;

    QEventLoop loop(this);

    m_eventLoop = &loop;

    connect(this, &InputPasswordDialog::cancel, &loop, [&loop]{loop.exit(0);});
    connect(this, &InputPasswordDialog::confirm, &loop, [&loop]{loop.exit(1);});

    return loop.exec(QEventLoop::DialogExec);
}

void InputPasswordDialog::done(int code)
{
    if(m_eventLoop) {
        m_eventLoop->exit(code);
    }
}

void InputPasswordDialog::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    updateMainWidgetPos();
}

void InputPasswordDialog::showEvent(QShowEvent *e)
{
    QFrame::showEvent(e);

    setFocus();
}

void InputPasswordDialog::updateMainWidgetPos()
{
    m_mainWidget->move(width() / 2 - m_mainWidget->width() / 2, 0);
}
