/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MODULEHEADER_H
#define MODULEHEADER_H

#include <dbaseline.h>

#include <QLabel>
#include <QTimer>

DWIDGET_BEGIN_NAMESPACE
class DTextButton;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE

class QLabel;
class RightContentWidget;
class ModuleHeader : public DBaseLine
{
    Q_OBJECT
public:
    ModuleHeader(QString title, bool addResetButton = true, QWidget *parent = 0);
    ModuleHeader(QWidget *leftContent, bool addResetButton = true, QWidget *parent = 0);
    ~ModuleHeader();
public slots:
    void initUI(bool addResetButton);
    void setTitle(const QString &);
signals:
    void resetButtonClicked();
private:
    RightContentWidget *m_rightContentWidget;
    QLabel *m_title;
};

class RightContentWidget: public QWidget
{
    Q_OBJECT
public:
    RightContentWidget(bool addResetButton, QWidget *parent = 0);
    ~RightContentWidget();
signals:
    void resetButtonClicked();

private slots:
    void resetUI();
private:
    QLabel *m_tipsLabel;
    DTextButton *m_reset;
    QTimer *m_timer;
};
#endif // MODULEHEADER_H
