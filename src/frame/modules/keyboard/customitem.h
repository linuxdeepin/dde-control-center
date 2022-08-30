// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

#include "shortcutitem.h"
#include "widgets/settingsitem.h"
#include "keyboardwork.h"

using namespace dcc::widgets;

class ShortcutKey;

namespace dcc {
namespace keyboard {
class CustomItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit CustomItem(QWidget *parent = 0);

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
}


#endif // CUSTOMITEM_H
