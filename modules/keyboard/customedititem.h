/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CUSTOMEDITITEM_H
#define CUSTOMEDITITEM_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include "keyboardwork.h"
#include "settingsitem.h"

using namespace dcc::widgets;

namespace dcc {
namespace keyboard {
class CustomEditItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit CustomEditItem(QWidget *parent = 0);
    void setTitle(const QString &title);
    void setAccels(const QString &accels);
    void setId(const QString &id);
    void setBottomTip(ShortcutInfo *conflict);
    QString getAccles() const;

signals:
    void requestCustomTip(const bool, const QString &tip);
    void requestShortcutList(const QString &shortcut);

private slots:
    void onKeyEvent(bool press, QString shortcut);
    void onSave();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void leaveEvent(QEvent *);

private:
    KeybingdingInter* m_inter;
    QLabel* m_title;
    bool m_contain;
    bool m_display;
    bool m_checked;
    QRect m_rect;
    QLineEdit* m_shortcutEdit;
    QString m_accels;
    QString m_id;
};

}
}

#endif // CUSTOMEDITITEM_H
