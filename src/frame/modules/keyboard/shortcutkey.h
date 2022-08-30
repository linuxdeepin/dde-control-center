// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SHORTCUTKEY_H
#define SHORTCUTKEY_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>

class KeyLabel;

class ShortcutKey : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutKey(QWidget *parent = nullptr);
    void setTextList(const QStringList &list);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QHBoxLayout *m_mainLayout;
    QList<KeyLabel*> m_list;
};

#endif // SHORTCUTKEY_H
