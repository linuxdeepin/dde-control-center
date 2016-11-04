/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SETTINGSGROUP_H
#define SETTINGSGROUP_H

#include <QFrame>
#include <QTimer>

class QVBoxLayout;

namespace dcc {

class SettingsItem;

class SettingsGroup : public QFrame
{
    Q_OBJECT

public:
    explicit SettingsGroup(QFrame *parent = 0);

    void appendItem(SettingsItem * item);
    void removeItem(SettingsItem * item);
    void setSpacing(const int spaceing);

private:
    bool eventFilter(QObject *, QEvent *event);
    void updateHeadTail();

private slots:
    void updateHeight();

private:
    QVBoxLayout *m_layout;

    QTimer *m_updateHeightTimer;
};

}

#endif // SETTINGSGROUP_H
