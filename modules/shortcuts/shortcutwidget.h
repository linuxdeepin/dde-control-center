/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SHORTCUTWIDGET_H
#define SHORTCUTWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>

#include <dwidget_global.h>

#include "searchlist.h"
#include "shortcutedit.h"

DWIDGET_USE_NAMESPACE

class ImageNameButton;
class ShortcutWidget : public QFrame, public SearchItem
{
    Q_OBJECT

    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)

public:
    ShortcutWidget(ShortcutDbus *dbus, int id, const QString &title, const QString &shortcut = "", QWidget *parent = 0);

public slots:
    void setId(int id);
    void setTitle(const QString &title);
    void setShortcut(const QString &shortcut);
    void showRemoveButton() const;
    void hideRemoveButton() const;
    void setEnableEdit(bool enable);

public:
    int id() const;
    QString title() const;
    QString shortcut() const;

    QStringList keyWords() const Q_DECL_OVERRIDE;
    void setData(const QVariant &datas) Q_DECL_OVERRIDE;
    QVariant getData() Q_DECL_OVERRIDE;
    QWidget *widget() const Q_DECL_OVERRIDE;

signals:
    void shortcutChanged(QString flag, QString shortcut);
    void removeShortcut();
    void keyWordChanged(const QString &old_word, const QString &new_word);

private slots:
    void shortcutKeyFinished(const QString &str);

private:
    QHBoxLayout *m_hlayout;
    QVBoxLayout *m_vlayout;
    int m_id;
    QLabel *m_title;
    ShortcutEdit *m_shortcut;
    ShortcutWidget *m_me;
    ImageNameButton *m_removeButton;
    QPropertyAnimation *m_animation;
    QString m_shortcuText;
};

#endif // SHORTCUTWIDGET_H
