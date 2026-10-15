// SPDX-FileCopyrightText: 2016 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include <DFrame>

#include <QFrame>

class SettingsItem : public QFrame
{
    Q_OBJECT

public:
    explicit SettingsItem(QWidget *parent = nullptr);

    void addBackground();
    void removeBackground();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_hasBack;
};

#endif // SETTINGSITEM_H
