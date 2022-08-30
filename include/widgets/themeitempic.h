// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef THEMEITEMPIC_H
#define THEMEITEMPIC_H

#include <QWidget>

namespace dcc
{
namespace widgets
{
class TranslucentFrame;
class ThemeItemPic : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeItemPic(QWidget *parent = 0);
    void setPicPath(const QString &path);

    void paintEvent(QPaintEvent *event);

private:
    QString m_picPath;

};


}
}


#endif // THEMEITEMPIC_H
