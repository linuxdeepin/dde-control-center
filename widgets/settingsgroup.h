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

#include "translucentframe.h"

class QVBoxLayout;

namespace dcc {
namespace widgets {

class SettingsItem;
class SettingsHeaderItem;

class SettingsGroup : public TranslucentFrame
{
    Q_OBJECT

public:
    explicit SettingsGroup(QFrame *parent = 0);
    explicit SettingsGroup(const QString &title, QFrame *parent = 0);
    ~SettingsGroup();

    SettingsHeaderItem *headerItem() const { return m_headerItem; }
    void setHeaderVisible(const bool visible);

    SettingsItem* getItem(int index);
    void insertItem(const int index, SettingsItem * item);
    void appendItem(SettingsItem * item);
    void removeItem(SettingsItem * item);
    void moveItem(SettingsItem *item, const int index);
    void setSpacing(const int spaceing);

    int itemCount() const;
    void clear();

private:
    bool eventFilter(QObject *, QEvent *event);
    void updateHeadTail();

private slots:
    void updateHeight();

private:
    QVBoxLayout *m_layout;
    SettingsHeaderItem *m_headerItem;

    QTimer *m_updateHeightTimer;
    QTimer *m_updateHeadTailTimer;
};

}
}

#endif // SETTINGSGROUP_H
