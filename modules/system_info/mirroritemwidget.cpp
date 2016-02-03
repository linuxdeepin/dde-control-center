/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "mirroritemwidget.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QProcess>

#include <libdui/dthememanager.h>
#include <libdui/dseparatorhorizontal.h>

MirrorItemWidget::MirrorItemWidget(QWidget *parent)
    : QFrame(parent)
{
    m_selectedBtn = new DImageButton;
    m_selectedBtn->setNormalPic(":/images/images/select_active.png");
    m_selectedBtn->setVisible(false);
    m_mirrorName = new QLabel;
    m_mirrorName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_mirrorName->setObjectName("MirrorName");
    m_mirrorSpeed = new QLabel;
    m_mirrorSpeed->setObjectName("Status");
    m_mirrorSpeed->setText("...");

    QBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(m_selectedBtn);
    iconLayout->setAlignment(m_selectedBtn, Qt::AlignCenter);
    iconLayout->setSpacing(0);
    iconLayout->setMargin(0);

    QWidget *iconWidget = new QWidget;
    iconWidget->setLayout(iconLayout);
    iconWidget->setFixedWidth(30);
    iconWidget->setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(iconWidget);
    hLayout->addWidget(m_mirrorName);
    hLayout->addSpacing(5);
    hLayout->addWidget(m_mirrorSpeed);
    hLayout->addSpacing(14);
    hLayout->setMargin(0);
    hLayout->setSpacing(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(hLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

//    setSelected(true);

    setLayout(mainLayout);
    setFixedWidth(DCC::ModuleContentWidth);
    setFixedHeight(45);

    D_THEME_INIT_WIDGET(MirrorItemWidget, checked);
}

void MirrorItemWidget::setSelected(bool state)
{
    if (state == m_selected)
        return;

    m_selected = state;
    m_selectedBtn->setVisible(state);

    emit selectStateChanged(state);
}

QSize MirrorItemWidget::sizeHint() const
{
    return QSize(width(), height());
}

void MirrorItemWidget::testMirrorSpeed(const QString &mirrorAdr)
{
    QStringList args;
    args << mirrorAdr << "-s" << "1";

    QProcess *process = new QProcess;
    connect(process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &MirrorItemWidget::testMirrorSpeed_finish);

    process->start("netselect", args);
}

void MirrorItemWidget::testMirrorSpeed_finish(int ret)
{
    QProcess *process = qobject_cast<QProcess *>(sender());

    if (!process)
        return;

    // process exit with an error
    if (ret)
    {
        //: the mirror cant reachable
        m_mirrorSpeed->setText(tr("Timeout"));
        return process->deleteLater();
    }

    const QString output = process->readAllStandardOutput().trimmed();
    const QStringList result = output.split(' ');
    const int time = result.first().toInt();

    if (result.first().isEmpty())
        m_mirrorSpeed->setText(tr("Timeout"));
    else if (time > 2000)
        m_mirrorSpeed->setText(tr("Slow"));
    else if (time > 200)
        m_mirrorSpeed->setText(tr("Medium"));
    else
        m_mirrorSpeed->setText(tr("Fast"));

    process->deleteLater();
}

void MirrorItemWidget::mouseReleaseEvent(QMouseEvent *e)
{
    emit clicked();

    QFrame::mouseReleaseEvent(e);
}
