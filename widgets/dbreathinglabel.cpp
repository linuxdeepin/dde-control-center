/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QtDebug>
#include "dbreathinglabel.h"

DBreathingLabel::DBreathingLabel(QWidget *parent,Qt::WindowFlags f)
    : QLabel(parent, f), alpha_(255)
{
    m_showAnimation = new QPropertyAnimation(this, "alpha");
    m_hideAnimation = new QPropertyAnimation(this, "alpha");
    setDuration(400);
}
DBreathingLabel::DBreathingLabel (const QString &text, QWidget *parent, Qt::WindowFlags f)
    : QLabel(text, parent,f), alpha_(255)
{
    m_showAnimation = new QPropertyAnimation(this, "alpha");
    m_hideAnimation = new QPropertyAnimation(this, "alpha");
}
DBreathingLabel::~DBreathingLabel()
{
    delete m_showAnimation;
    delete m_hideAnimation;
}
int DBreathingLabel::alpha() const{
    return alpha_;
}
void DBreathingLabel::setDuration(int duration) {
    m_showAnimation->setDuration(duration);
    m_hideAnimation->setDuration(duration);
}
void DBreathingLabel::setColor(QColor color) {
    m_palette.setColor(QPalette::WindowText, color);
    this->setPalette(m_palette);
}
void DBreathingLabel::setAlpha(int opa) {
    alpha_ = opa;
    QColor color =m_palette.color(QPalette::WindowText);
    color = QColor(color.red(), color.green(), color.blue(), opa);
    m_palette.setColor(QPalette::Foreground, color);
    setPalette(m_palette);
}


void DBreathingLabel::hideLabel() {
//    m_showAnimation->stop();
    m_hideAnimation->setStartValue(255);
    m_hideAnimation->setEndValue(0);
    m_hideAnimation->start();
}

void DBreathingLabel::showLabel() {

    m_showAnimation->setStartValue(0);
    m_showAnimation->setEndValue(255);
    m_showAnimation->start();
}
