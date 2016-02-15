/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SCROLLFRAME_H
#define SCROLLFRAME_H

#include <QWidget>
#include <QBoxLayout>
#include <QPointer>

#include <dscrollarea.h>
#include <dboxwidget.h>
#include <dstackwidget.h>

DWIDGET_USE_NAMESPACE

class ScrollFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ScrollFrame(QWidget *parent = 0);

    QBoxLayout *headerLayout() const;
    QBoxLayout *mainLayout() const;

    void pushWidget(QWidget *widget);
    void popCurrentWidget();
    void popAllWidget();

    DStackWidget *stackWidget() const;

private slots:
    void onCurrentWidgetSizeChanged(const QSize &size);

signals:
    void sizeChanged(QSize size);
    void currentMainWidgetChanged(const QWidget *widget);

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    void init();
    void setStackWidgetHeight(int height);

    DScrollArea *m_scrollArea = nullptr;
    DStackWidget *m_stackWidget = nullptr;
    QPointer<const DBoxWidget> m_currentWidget;
    DVBoxWidget *m_headerWidget = nullptr;
    DVBoxWidget *m_mainWidget = nullptr;
};

#endif // SCROLLFRAME_H
