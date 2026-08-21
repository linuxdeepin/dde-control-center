//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef FONTSIZEMODEL_H
#define FONTSIZEMODEL_H

#include <QObject>

class FontSizeModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int size READ getFontSize WRITE setFontSize NOTIFY sizeChanged)
public:
    explicit FontSizeModel(QObject *parent = 0);
public Q_SLOTS:
    void setFontSize(const int size);
    Q_INVOKABLE inline int getFontSize() const { return m_size;}

Q_SIGNALS:
    void sizeChanged(int size);

private:
    int m_size;
};

#endif // FONTSIZEMODEL_H
