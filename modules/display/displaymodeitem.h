/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DISPLAYMODEITEM_H
#define DISPLAYMODEITEM_H

#include <QLabel>

class DisplayModeItem : public QLabel
{
    Q_OBJECT

    Q_PROPERTY(QString iconName READ iconName WRITE setIconName)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString iconText READ iconText WRITE setIconText)
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(bool hover READ hover NOTIFY hoverChanged)
    Q_PROPERTY(bool clickCheck READ clickCheck WRITE setClickCheck)

public:
    explicit DisplayModeItem(bool showSeparator = true, bool showTitle = true, bool showRightArrow = false, QWidget *parent = 0);
    ~DisplayModeItem();

    QString iconName() const;
    QString title() const;
    QString text() const;
    bool checked() const;
    bool hover() const;
    bool clickCheck() const;
    QString iconText() const;

public slots:
    void setIconName(QString iconName);
    void setTitle(QString title);
    void setText(QString text);
    void setChecked(bool checked);
    void setClickCheck(bool clickCheck);
    void setIconText(QString iconText);

signals:
    void hoverChanged(bool hover);
    void checkedChanged(bool checked);
    void clicked();
    void rightArrowClicked();

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    void setHover(bool arg);
    void updateIcon();

private:
    static DisplayModeItem *activeItem;

    QString m_iconName;
    QString m_iconPath;
    QString m_iconText;
    QLabel *m_title;
    QLabel *m_text;
    bool m_checked;
    bool m_hover;
    bool m_clickCheck;
};

#endif // DISPLAYMODEITEM_H
