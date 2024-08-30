//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SEARCHINPUT_H
#define SEARCHINPUT_H

#include "interface/namespace.h"
#include <QLineEdit>

namespace DCC_NAMESPACE {

class SearchInput : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString icon READ iconPath WRITE setIcon)
public:
    explicit SearchInput(QWidget* parent = 0);
    void setSearchText(const QString& text);
    void setIconVisible(bool visible);
    QString iconPath() const;
    void setIcon(const QString &filepath);

protected:
    void paintEvent(QPaintEvent *);

private:
    bool m_iconVisible;
    QString m_search;
    QPixmap m_icon;
    QString m_iconPath;
};

}

#endif // SEARCHINPUT_H
