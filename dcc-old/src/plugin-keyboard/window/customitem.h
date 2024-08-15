//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

#include "interface/namespace.h"
#include "shortcutitem.h"
#include "widgets/settingsitem.h"

class ShortcutKey;

namespace DCC_NAMESPACE {
class CustomItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit CustomItem(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setShortcut(const QString &shortcut);
    void setAlert(bool isAlert);
    QString text() const;

Q_SIGNALS:
    void requestUpdateKey();
    void changeAlert();

protected:
    void mouseReleaseEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *m_title;
    QLineEdit *m_shortcutEdit;
    QString m_accels;
    ShortcutKey *m_shortKey;
    bool isAlert;
};
}

#endif // CUSTOMITEM_H
