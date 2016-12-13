/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
