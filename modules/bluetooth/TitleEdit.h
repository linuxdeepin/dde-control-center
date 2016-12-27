/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TITLEEDIT_H
#define TITLEEDIT_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

namespace dcc
{
namespace bluetooth
{
class TitleEdit : public QWidget
{
    Q_OBJECT
public:
    explicit TitleEdit(QWidget *parent = 0);

signals:
    void requestSetBluetoothName(const QString &name);

public slots:
    void setName();
    void setEdit();
    void setTitle(const QString &title);

private:
    QLabel *m_name;
    QLineEdit *m_lineEdit;
    DImageButton *m_editWidget;

};
}
}


#endif // TITLEEDIT_H
