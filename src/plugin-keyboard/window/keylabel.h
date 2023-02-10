//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef KEYLABEL_H
#define KEYLABEL_H

#include <QWidget>

class KeyLabel : public QWidget
{
    Q_OBJECT
public:
    explicit KeyLabel(const QString &text, QWidget *parent = nullptr);

    void setEnter(const bool enter);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_isEnter;
    QString m_text;
};


#endif // KEYLABEL_H
