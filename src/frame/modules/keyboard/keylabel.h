// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef KEYLABEL_H
#define KEYLABEL_H

#ifdef WINDOW_MODE

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

#else

#include <QLabel>

class KeyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit KeyLabel(QWidget *parent = nullptr);
    explicit KeyLabel(const QString &text, QWidget *parent = nullptr);

    void setEnter(const bool enter);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_isEnter;
};

#endif

#endif // KEYLABEL_H
