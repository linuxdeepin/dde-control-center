/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "notifyviewer.h"
#include "notifymanager.h"
#include <QJsonObject>
#include <QStandardPaths>
#include <QTime>
#include <QEvent>
#include <QPainter>
#include <QTextDocument>

Viewer::Viewer(const QJsonObject &value, QWidget *parent) : QFrame(parent),
    m_appName(new QLabel),
    m_time(new QLabel),
    m_body(new NotifyBody),
    m_close(new DImageButton(":/images/notify_close_normal.png", ":/images/notify_close_hover.png", ":/images/notify_close_press.png")),
    m_appIcon(new QLabel),
    m_mainlayout(new QHBoxLayout)
{
    m_appIcon->setMargin(0);
    m_close->setMargin(0);
    m_appName->setMargin(0);
    m_appIcon->setFixedSize(ICON_SIZE,ICON_SIZE);
    m_close->setFixedSize(14,14);
    m_time->setFixedHeight(10);
//    m_body->setFixedSize(272, 50);

    m_appName->setStyleSheet("font-family: 'Noto Sans CJK SC';"
                             "font-size: 11px;"
                             "font-style: normal;"
                             "color: #ffffff;");
    m_time->setStyleSheet("font-family: 'Noto Sans CJK SC';"
                          "font-size: 10px;"
                          "font-style: normal;"
                          "color: rgba(255, 255, 255, 0.6);");

    m_close->setVisible(false);

    QHBoxLayout *hboxlayout = new QHBoxLayout;
    hboxlayout->setMargin(0);
    hboxlayout->setSpacing(0);
    hboxlayout->addWidget(m_appName, 0, Qt::AlignLeft);
    hboxlayout->addStretch();
    hboxlayout->addWidget(m_time, 0, Qt::AlignRight);
    hboxlayout->addWidget(m_close, 0, Qt::AlignRight);
    hboxlayout->addSpacing(4);
    hboxlayout->setContentsMargins(0, 5, 5, 0);

    m_mainlayout->addSpacing(15);
    m_mainlayout->addWidget(m_appIcon, 0, Qt::AlignCenter);
    m_mainlayout->addSpacing(1);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addSpacing(4);
    rightLayout->addLayout(hboxlayout);
    rightLayout->addSpacing(1);
    rightLayout->addWidget(m_body);
    rightLayout->addStretch();

    m_mainlayout->addSpacing(20);
    m_mainlayout->addLayout(rightLayout);
    m_mainlayout->addSpacing(10);

    m_mainlayout->setMargin(0);
    m_mainlayout->setSpacing(0);

    this->setLayout(m_mainlayout);

    connect(m_close, &DImageButton::clicked, this, &Viewer::onPlayCloseAnimation);

    setAppName(value["summary"].toString());
    setAppBody(value["body"].toString());
    setAppId(value["id"].toString());

    if (value["icon"].toString().isEmpty()) {
        setAppIcon(CacheFolder + value["id"].toString() + ".png");
    } else
        setAppIcon(value["icon"].toString());

    m_time->setText(QTime::currentTime().toString("AP hh:mm"));
}

void Viewer::setAppName(const QString &s) {
    m_appName->setText(s);
}

void Viewer::setAppIcon(const QString &s) {
    const QIcon &icon = QIcon::fromTheme(s, QIcon::fromTheme("application-x-desktop"));

    if (!icon.isNull()) {
        m_appIcon->setPixmap(icon.pixmap(ICON_SIZE,ICON_SIZE));
    }
}

void Viewer::setAppBody(const QString &s) {
    m_body->setText(s);
}

void Viewer::setAppTime(const QString &s) {
    m_time->setText(s);
}

void Viewer::setAppId(const QString &id)
{
    m_id = id;
}

void Viewer::onClose()
{
    m_close->clicked();
}

void Viewer::onPlayCloseAnimation()
{
    m_close->setDisabled(true);

    if (m_close->isEnabled())
        return;

    QPropertyAnimation *anim=new QPropertyAnimation(this, "pos",this);
    anim->setDuration(300);
    anim->setStartValue(QPoint(this->x(), this->y()));
    anim->setEndValue(QPoint(this->width(), this->y()));
    anim->setEasingCurve(QEasingCurve::InOutCubic);
    anim->start(QPropertyAnimation::DeleteWhenStopped);

    connect(anim, &QPropertyAnimation::finished, this, &Viewer::onAnimationFinished);
}

void Viewer::onAnimationFinished()
{
    emit requestClose(m_id);
    deleteLater();
}

void Viewer::paintEvent(QPaintEvent *event)
{
    if (rect().contains(mapFromGlobal(QCursor::pos()))) {
        m_close->setVisible(true);
        m_time->setVisible(false);

        QPainter painter(this);
        painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

        QPainterPath path;
        path.addRoundedRect(rect(), 4, 4);

        painter.save();
        painter.fillPath(path, QColor(254, 254, 254, 0.13 * 255));
        painter.restore();
        painter.end();
    } else {
        m_close->setVisible(false);
        m_time->setVisible(true);
    }

    QFrame::paintEvent(event);
}

void Viewer::enterEvent(QEvent *event)
{
    update();

    QFrame::enterEvent(event);
}

void Viewer::leaveEvent(QEvent *event)
{
    update();

    QFrame::leaveEvent(event);
}
