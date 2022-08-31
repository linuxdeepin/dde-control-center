// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCC_WIDGETS_LOADINGINDICATOR_H
#define DCC_WIDGETS_LOADINGINDICATOR_H

#include <dpicturesequenceview.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class LoadingIndicator : public DPictureSequenceView
{
    Q_OBJECT
public:
    explicit LoadingIndicator(QWidget *parent = 0);

    Q_PROPERTY(QString theme READ theme WRITE setTheme)

    inline QString theme() const { return m_theme; }
    void setTheme(const QString &theme);

private:
    QString m_theme;
};

} // namespace widgets
} // namespace dcc

#endif // DCC_WIDGETS_LOADINGINDICATOR_H
