/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "inputview.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTimer>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDebug>

#include <libdui/dthememanager.h>
#include <libdui/dtextbutton.h>

#include "constants.h"

#include "../helper.h"

DUI_USE_NAMESPACE

InputView::InputView(QWidget* p)
    : AbstractView(p),
      m_validator(new QRegExpValidator(*new QRegExp("[A-Za-z0-9]{6}"), this)),
      m_connectButton(new DTextButton(tr("Connect")))
{
    setObjectName("InputView");
    m_connectButton->setEnabled(false);
    QObject::connect(m_connectButton, &DTextButton::clicked, this, &InputView::emitConnect);

    initialize();

    focus();
    auto button = new DTextButton(tr("Cancel"));
    QObject::connect(button, &DTextButton::clicked, [this] (bool){
        emit cancel();
    });
    addButton(button)-> addButton(m_connectButton);
}

void InputView::emitConnect()
{
    emit connect(m_tokenEdit->text().trimmed());
}

void InputView::connectToClient()
{
    if (m_connectButton->isEnabled()) {
        emitConnect();
    }
}

QWidget* InputView::createMainWidget()
{
    auto mainWidget = new QWidget;
    auto layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);

    m_tokenEdit = new QLineEdit;
    m_tokenEdit->setMaxLength(6);
    m_tokenEdit->setAttribute(Qt::WA_TranslucentBackground);
    m_tokenEdit->setAlignment(Qt::AlignCenter);
    m_tokenEdit->setFixedWidth(DCC::ModuleContentWidth);
    m_tokenEdit->setFixedHeight(70);
    QObject::connect(m_tokenEdit, SIGNAL(returnPressed()), this, SLOT(connectToClient()));
    QObject::connect(m_tokenEdit, &QLineEdit::textChanged, [this](const QString& token){
        qDebug() << "valid token";
        QString copyToken = token;
        int pos = 0;
        m_connectButton->setEnabled(false);
        if (m_validator->validate(copyToken, pos) == QValidator::Acceptable) {
            m_connectButton->setEnabled(true);
            m_tip->setText(tr("Start remote access after clicking on \"Connect\""));
        } else if (m_tip->text() == tr("Start remote access after clicking on \"Connect\"")){
            m_tip->setText(tr("Please enter the verification code in the input field above"));
        }
    });
    layout->addWidget(m_tokenEdit);

    auto separator = new QWidget;
    separator->setObjectName("separator");
    separator->setFixedSize(DCC::ModuleContentWidth-30, 1);
    layout->addWidget(separator);
    layout->setAlignment(separator, Qt::AlignHCenter);

    m_tip = new QLabel;
    m_tip->setText(tr("Please enter the verification code in the input field above"));
    m_tip->setAlignment(Qt::AlignTop|Qt::AlignHCenter);
    m_tip->setFixedSize(DCC::ModuleContentWidth, 20);

    layout->addSpacing(10);
    layout->addWidget(m_tip);

    mainWidget->setLayout(layout);
    setStyleSheet(readStyleSheet("inputview"));
    return mainWidget;
}

void InputView::focus() {
    qDebug() << "focus token input widget";
    m_tokenEdit->setFocus();
}
