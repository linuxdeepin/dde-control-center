/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "dextendboard.h"

DExtendBoard::DExtendBoard(QFrame *parent)
    : QFrame(parent)
{
    m_label = new QLabel;
    m_label->setStyleSheet(QString("QLabel{color: %1;font-size:12px;}").arg(DCC::TextNormalColor.name()));
    setStyleSheet(QString("background-color: %1;").arg(DCC::BgDarkColor.name()));
    m_animation = new QPropertyAnimation(this, "height");

    connect(m_animation, &QPropertyAnimation::valueChanged, this, &DExtendBoard::adjustSize);
    m_guideExtendTriangle = new TriangleRectangle;
    m_guideExtendTriangle->setFixedHeight(8);
    m_guideExtendTriangle->fillColor = qRgb(37, 38, 39);
    m_guideExtendTriangle->strokeColor = qRgb(37, 38, 39);
    m_guideExtendTriangle->setStyleSheet(QString("background-color: %1;").arg(DCC::BgDarkColor.name()));
    m_customChooseLabel = new QLabel;

    m_customChooseLabel->setFixedHeight(30);
    m_customChooseLabel->setText(tr("Custom options"));
    m_customChooseLabel->setStyleSheet(QString("background-color: %1;color:rgb(180,180,180)").arg(DCC::BgLightColor.name()));

    m_idleLabel = new QLabel;
    m_idleLabel->setText(tr("Turn off the display"));
    m_idleLabel->setStyleSheet("color: rgb(180, 180, 180)");

    m_idleButtonGrid = new DButtonGrid(1, 7);
    m_idleButtonGrid->setFixedHeight(30);
    m_idleButtonGrid->setItemSize(32, 28);

    m_idleButtons << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("Never");
    m_idleButtonGrid->addButtons(m_idleButtons);
    m_idleButtonGrid->checkButtonByIndex(6);
    m_suspendLabel = new QLabel;
    m_suspendLabel->setText(tr("Suspend"));
    m_suspendLabel->setStyleSheet("color: rgb(180, 180, 180)");;
    m_suspendButtonGrid = new DButtonGrid(1, 7);

    m_idleButtonGrid->setColumnWidth(6, 48);
    m_idleButtonGrid->setFixedWidth(DCC::ModuleContentWidth);
    m_suspendButtonGrid->setItemSize(32, 28);

    m_suspendButtons << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("Never");
    m_suspendButtonGrid->addButtons(m_suspendButtons);
    m_suspendButtonGrid->checkButtonByIndex(6);

    m_suspendButtonGrid->setColumnWidth(6, 48);
    m_suspendButtonGrid->setFixedWidth(DCC::ModuleContentWidth);

    m_bottomSeparator = new DSeparatorHorizontal(m_label);

    m_customLabel = new QLabel;
    m_customLabel->setStyleSheet(QString("background-color: %1;color:rgb(180,180,180)").arg(DCC::BgLightColor.name()));

    customIdleLayout = new QHBoxLayout;
    customIdleLayout->setMargin(0);
    customIdleLayout->setSpacing(0);
    customIdleLayout->addSpacing(DTK_WIDGET_NAMESPACE::HEADER_LEFT_MARGIN);
    customIdleLayout->addWidget(m_customChooseLabel);
    customIdleLayout->addStretch();
    m_customLabel->setLayout(customIdleLayout);

    idleLayout = new QHBoxLayout;
    idleLayout->setMargin(0);
    idleLayout->setSpacing(0);
    idleLayout->addSpacing(DTK_WIDGET_NAMESPACE::HEADER_LEFT_MARGIN);
    idleLayout->addWidget(m_idleLabel);


    suspendLayout = new QHBoxLayout;
    suspendLayout->setMargin(0);
    suspendLayout->setSpacing(0);
    suspendLayout->addSpacing(DTK_WIDGET_NAMESPACE::HEADER_LEFT_MARGIN);
    suspendLayout->addWidget(m_suspendLabel);

    m_contentLayout = new QVBoxLayout;
    m_contentLayout->setMargin(0);
    m_contentLayout->setSpacing(0);
    m_contentLayout->addWidget(m_guideExtendTriangle);
    m_contentLayout->addWidget(m_customLabel);
    m_contentLayout->addLayout(idleLayout);
    m_contentLayout->addWidget(m_idleButtonGrid);
    m_contentLayout->addLayout(suspendLayout);
    m_contentLayout->addWidget(m_suspendButtonGrid);
    m_contentLayout->addWidget(m_bottomSeparator);
    setLayout(m_contentLayout);

}

DExtendBoard::~DExtendBoard()
{
    delete m_animation;
    m_label->deleteLater();
}
void DExtendBoard::setWidth(int ex_width) {
    setFixedWidth(ex_width);
}
void DExtendBoard::setHeight(int ex_height) {
    setFixedHeight(ex_height);
}

void DExtendBoard::setExpand(bool expand) {
    if (isExpand != expand) {
        isExpand = expand;
        if (expand) {
            m_animation->setDuration(200);
            m_animation->setEasingCurve(QEasingCurve::InCubic);
            m_guideExtendTriangle->show();
            m_customChooseLabel->show();
            m_idleLabel->show();
            m_suspendLabel->show();
            m_suspendButtonGrid->show();
            m_idleButtonGrid->show();
            this->setFixedHeight(200);
            m_animation->setStartValue(2);
            m_animation->setEndValue(155);

        } else {
            m_animation->setDuration(100);

            m_animation->setStartValue(155);
            m_animation->setEndValue(2);
            m_guideExtendTriangle->hide();
            m_customChooseLabel->hide();
            m_idleLabel->hide();
            m_suspendLabel->hide();
            m_suspendButtonGrid->hide();
            m_idleButtonGrid->hide();
            m_bottomSeparator->move(1, 1);
        }
        m_animation->stop();
        m_animation->start();
    } else {
        return;
    }
}
void DExtendBoard::resizeEvent(QResizeEvent *e) {
    setFixedHeight(e->size().height());
}
