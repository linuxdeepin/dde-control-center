/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "notifyviewer.h"

Viewer::Viewer(QWidget *parent) : QWidget(parent),
    m_appName(new QLabel),
    m_body(new QLabel),
    m_time(new QLabel),
    m_close(new DImageButton(":/images/close.png", ":/images/close.png", ":/images/close.png", nullptr)),
    m_appIcon(new QLabel),
    m_mainlayout(new QHBoxLayout)
{
    m_appIcon->setMargin(0);
    m_close->setMargin(0);
    m_appName->setMargin(0);
    m_body->setMargin(0);
    m_appIcon->setFixedSize(48,48);
    m_appName->setFixedWidth(165);
    m_close->setFixedSize(10,10);
    m_time->setFixedHeight(10);
    m_body->setWordWrap(true);
    m_body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_appName->setStyleSheet("font-family: SourceHanSansSC;"
                             "font-size: 13px;"
                             "font-style: normal;"
                             "color: #ffffff;");
    m_body->setStyleSheet("font-family: SourceHanSansSC-Normal;"
                          "font-size: 12px;"
                          "font-style: normal;"
                          "color: #ffffff;");
    m_time->setStyleSheet("font-family: SourceHanSansSC-Normal;"
                          "font-size: 11px;"
                          "font-style: normal;"
                          "color: #ffffff;");

    m_appName->setAttribute(Qt::WA_TranslucentBackground, true);
    m_appName->setAutoFillBackground(false);
    m_appIcon->setAttribute(Qt::WA_TranslucentBackground, true);
    m_appIcon->setAutoFillBackground(false);
    m_close->setAttribute(Qt::WA_TranslucentBackground, true);
    m_close->setAutoFillBackground(false);
    m_body->setAttribute(Qt::WA_TranslucentBackground, true);
    m_body->setAutoFillBackground(false);
    m_time->setAttribute(Qt::WA_TranslucentBackground, true);
    m_time->setAutoFillBackground(false);

    m_close->setVisible(false);
    QHBoxLayout *hboxlayout = new QHBoxLayout;
    hboxlayout->setMargin(0);
    hboxlayout->setSpacing(0);
    hboxlayout->addWidget(m_appName, 0, Qt::AlignLeft);
    hboxlayout->addStretch();
    hboxlayout->addWidget(m_time, 0, Qt::AlignRight);
    hboxlayout->addWidget(m_close, 0, Qt::AlignRight);
    hboxlayout->setContentsMargins(0, 5, 5, 0);

    m_mainlayout->addWidget(m_appIcon, 0, Qt::AlignCenter);
    m_mainlayout->addSpacing(1);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addLayout(hboxlayout);
    rightLayout->addSpacing(1);
    rightLayout->addWidget(m_body, 0, Qt::AlignLeft);
    rightLayout->addStretch();

    m_mainlayout->addLayout(rightLayout);

    m_mainlayout->setMargin(0);
    m_mainlayout->setSpacing(0);

    this->setLayout(m_mainlayout);
    connect(m_close, &DImageButton::clicked, [=]{
        m_anim1=new QPropertyAnimation(this, "pos",this);
        m_anim1->setDuration(600);
        m_anim1->setStartValue(QPoint(this->x(), this->y()));
        m_anim1->setEndValue(QPoint(this->width(), this->y()));
        m_anim1->setEasingCurve(QEasingCurve::OutCirc);
        m_anim1->start();

        connect(m_anim1, &QPropertyAnimation::finished, [=]{
            this->deleteLater();
        });
    });
}

void Viewer::setAppName(const QString &s) {
    m_appName->setText(s);
}

void Viewer::setAppIcon(const QString &s) {
    const QIcon &icon = QIcon::fromTheme(s);

    if (!icon.isNull()) {
        m_appIcon->setPixmap(icon.pixmap(m_appIcon->size()));
    }
}

void Viewer::setAppBody(const QString &s) {
    m_body->setText(s);
}

void Viewer::setAppTime(const QString &s) {
    m_time->setText(s);
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
