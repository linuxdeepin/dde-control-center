/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUNDEFFECTSWITCHWIDGET_H
#define SOUNDEFFECTSWITCHWIDGET_H

#include <QWidget>
#include <QLabel>

#include <libdui/dswitchbutton.h>
#include <libdui/dseparatorhorizontal.h>

DUI_USE_NAMESPACE

class SoundEffectSwitchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SoundEffectSwitchWidget(QWidget *parent = 0);

    inline void setTitle(const QString &title) {m_soundName->setText(title);}
    inline void setSwitched(const bool switched) {m_switchButton->setChecked(switched);}

signals:
    void switchToggled(bool switched) const;

private:
    QLabel *m_soundName;
    DSwitchButton *m_switchButton;
};

#endif // SOUNDEFFECTSWITCHWIDGET_H
