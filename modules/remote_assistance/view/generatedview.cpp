/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "generatedview.h"

#include <QApplication>
#include <QClipboard>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>

#include <libdui/dthememanager.h>
#include <libdui/dtextbutton.h>

#include "constants.h"

#include "../helper.h"

DUI_USE_NAMESPACE

GeneratedView::GeneratedView(const QString& token, QWidget* p)
    : AbstractView(p),
      m_copyTipVisableTimer(new QTimer(this))
{
    setObjectName("GeneratedView");
    m_copyTipVisableTimer->setInterval(3000);
    QObject::connect(m_copyTipVisableTimer, &QTimer::timeout, [this] {
        m_copyTip->setText("");
    });

    initialize();

    m_token->setText(token);

    auto button = new DTextButton(tr("Copy Code"));
    connect(button, &DTextButton::clicked, [this] {
        m_copyTip->setText(tr("Copied to clipboard successfully"));
        QString token = m_token->text();
        QApplication::clipboard()->setText(token);
        qDebug() << "Copy Code button on GeneratedView is clicked.";
        m_copyTipVisableTimer->stop();
        m_copyTipVisableTimer->start();
    });
    addButton(button);

    button = new DTextButton(tr("Cancel"));
    connect(button, &DTextButton::clicked, [this] {
        qDebug() << "cancel button on GeneratedView is clicked";
        emit cancel();
    });
    addButton(button);
}

QWidget* GeneratedView::createMainWidget()
{
    auto mainWidget = new QWidget;

    auto layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);

    layout->addSpacing(6);

    m_token = new QLabel;
    m_token->setObjectName("token");
    m_token->setFixedSize(DCC::ModuleContentWidth, 50);
    m_token->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_token);

    m_copyTip = new QLabel;
    m_copyTip->setObjectName("copyTip");
    m_copyTip->setText(tr("Copied to clipboard successfully"));
    m_copyTip->setAlignment(Qt::AlignCenter);
    m_copyTip->setFixedWidth(DCC::ModuleContentWidth);
    layout->addWidget(m_copyTip);
    m_copyTip->setText("");
    layout->addSpacing(15);

    auto tip = new QLabel;
    tip->setObjectName("tip");
    tip->setWordWrap(true);
    tip->setText(tr("To start sharing your desktop, please provide the above verification code to whom will assist you. Your shared session will begin immediately after verification code input"));
    layout->addWidget(tip);
    layout->addSpacing(10);

    mainWidget->setLayout(layout);
    setStyleSheet(readStyleSheet("generatedview"));
    return mainWidget;
}
