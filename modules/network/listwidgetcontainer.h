/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef LISTWIDGETCONTAINER_H
#define LISTWIDGETCONTAINER_H

#include <QWidget>

#include <dboxwidget.h>
#include <libdui_global.h>
#include <dtextbutton.h>

DWIDGET_USE_NAMESPACE

class ListWidgetContainer : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit ListWidgetContainer(const QString &title, QWidget *parent = 0);

    void addWidget(QWidget *widget);
    void setBoxWidgetContentsMargins(int left, int top, int right, int bottom);
    void setButtonsVisible(bool visible);
    QString leftButtonText() const;
    QString rightButtonText() const;
    QLayout *boxWidgetLayout() const;

signals:
    void setTitle(const QString &title);
    void setLeftButtonText(const QString &text);
    void setRightButtonText(const QString &text);
    void setLeftButtonVisible(bool visible);
    void setRightButtonVisible(bool visible);
    void setBottomSeparatorVisible(bool visible);
    void leftButtonClicked();
    void rightButtonClicked();

protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *e) Q_DECL_OVERRIDE;
    void hideEvent(QHideEvent *e) Q_DECL_OVERRIDE;

private:
    void init(const QString &title);

    DVBoxWidget *m_boxWidget;
    QHBoxLayout *m_buttonLayout;
    DTextButton *m_cancelButton;
    DTextButton *m_connectButton;
};

#endif // LISTWIDGETCONTAINER_H
