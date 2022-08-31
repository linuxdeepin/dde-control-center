// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FONTSIZEMODEL_H
#define FONTSIZEMODEL_H

#include <QObject>

namespace dcc
{
namespace personalization
{
class FontSizeModel : public QObject
{
    Q_OBJECT
public:
    explicit FontSizeModel(QObject *parent = 0);
    void setFontSize(const int size);
    inline int getFontSize() const {return m_size;}

Q_SIGNALS:
    void sizeChanged(int size);

private:
    int m_size;
};

}
}

#endif // FONTSIZEMODEL_H
