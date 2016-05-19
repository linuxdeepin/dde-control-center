/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SHORTCUTEDIT_H
#define SHORTCUTEDIT_H

#include <QFrame>

class QHBoxLayout;
class QLineEdit;
class QLabel;
class ShortcutDbus;

class ShortcutEdit : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut)

public:
    explicit ShortcutEdit(ShortcutDbus *dbus, QWidget *parent = 0);

    QString shortcut() const;
    bool editing() const;

signals:
    void shortcutKeyFinished(QString shortcut);

public slots:
    void setShortcut(QString shortcut);
    void setEnableEdit(bool arg);
    void quitEditState();

protected:
    bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;

private:
    QHBoxLayout *m_layout;
    QLabel *m_label;
    QLineEdit *m_edit;
    ShortcutDbus *m_dbus;
    QString m_shortcut;
};

#endif // SHORTCUTEDIT_H
