/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "viewer.h"

Viewer::Viewer(QWidget *parent) : QWidget(parent),
    m_appName(new NumButton),
    m_summary(new NumButton),
    m_body(new NumButton),
    m_time(new NumButton),
    m_close(new DImageButton(":/images/close.png", ":/images/close.png", ":/images/close.png", nullptr)),
    m_appIcon(new AppIcon),
    m_mainlayout(new QGridLayout)
{
    m_appIcon->setMargin(0);
    m_close->setMargin(0);
    m_appName->setMargin(0);
    m_summary->setMargin(0);
    m_body->setMargin(0);
    m_appIcon->setFixedSize(48,48);
    m_appName->setFixedWidth(165);
    m_summary->setFixedWidth(220);
    m_body->setFixedWidth(220);
    m_close->setFixedSize(10,10);

    m_appName->setStyleSheet("font-family: SourceHanSansSC;"
                             "font-size: 13px;"
                             "font-style: normal;"
                             "font-stretch: normal;"
                             "color: #ffffff;");
    m_summary->setStyleSheet("font-family: SourceHanSansSC-Normal;"
                             "font-size: 12px;"
                             "font-style: normal;"
                             "font-stretch: normal;"
                             "color: #ffffff;");
    m_body->setStyleSheet("font-family: SourceHanSansSC-Normal;"
                          "font-size: 12px;"
                          "font-style: normal;"
                          "font-stretch: normal;"
                          "color: #ffffff;");
    m_time->setStyleSheet("font-family: SourceHanSansSC-Normal;"
                          "font-size: 11px;"
                          "font-style: normal;"
                          "font-stretch: normal;"
                          "color: #ffffff;");

    m_appName->setAttribute(Qt::WA_TranslucentBackground, true);
    m_appName->setAutoFillBackground(false);
    m_appIcon->setAttribute(Qt::WA_TranslucentBackground, true);
    m_appIcon->setAutoFillBackground(false);
    m_summary->setAttribute(Qt::WA_TranslucentBackground, true);
    m_summary->setAutoFillBackground(false);
    m_close->setAttribute(Qt::WA_TranslucentBackground, true);
    m_close->setAutoFillBackground(false);
    m_body->setAttribute(Qt::WA_TranslucentBackground, true);
    m_body->setAutoFillBackground(false);
    m_time->setAttribute(Qt::WA_TranslucentBackground, true);
    m_time->setAutoFillBackground(false);

    m_close->setVisible(false);
    m_hboxlayout = new QHBoxLayout;
    m_hboxlayout->addStretch();
    m_hboxlayout->addWidget(m_time,                   Qt::AlignRight);
    m_hboxlayout->addWidget(m_close,                  Qt::AlignRight);
    m_mainlayout->addWidget(m_appIcon,    0, 0, 3, 3, Qt::AlignVCenter);
    m_mainlayout->addWidget(m_appName,    0, 3,       Qt::AlignLeft);
    m_mainlayout->addLayout(m_hboxlayout, 0, 4,       Qt::AlignRight);
    m_mainlayout->addWidget(m_summary,    1, 3, 1, 2, Qt::AlignLeft);
    m_mainlayout->addWidget(m_body,       2, 3, 1, 2, Qt::AlignLeft);

    this->setLayout(m_mainlayout);
    connect(m_close, &DImageButton::clicked, [=]{
        QPropertyAnimation *m_anim1=new QPropertyAnimation(this, "pos");
        m_anim1->setDuration(600);
        m_anim1->setStartValue(QPoint(this->x(), this->y()));
        m_anim1->setEndValue(QPoint(300, this->y()));
        m_anim1->setEasingCurve(QEasingCurve::OutCirc);
        m_anim1->start();

        connect(m_anim1, &QPropertyAnimation::finished, [=]{
            this->deleteLater();
        });
    });
}

Viewer::~Viewer() {
    m_appName->deleteLater();
    m_summary->deleteLater();
    m_body->deleteLater();
    m_time->deleteLater();
    m_close->deleteLater();
    m_appIcon->deleteLater();
    m_mainlayout->deleteLater();
    m_hboxlayout->deleteLater();
}

void Viewer::enterEvent(QEvent *) {
    m_close->setVisible(true);
    m_time->setVisible(false);
}

void Viewer::leaveEvent(QEvent *) {
    m_close->setVisible(false);
    m_time->setVisible(true);
}

void Viewer::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Viewer::setValue(QString appName, QString appIcon, QString summary, QString body, QString time) {
    m_appName->setText(appName);
    m_appIcon->setIcon(appIcon);
    m_summary->setText(summary);
    m_body->setText(body);
    m_time->setText(time);
}
