/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     fanpengcheng <fanpengcheng_cm@deepin.com>
 *
 * Maintainer: fanpengcheng <fanpengcheng_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef BUTTON_H
#define BUTTON_H
#include <QIcon>

#include <DWidget>

#include "constants.h"

DWIDGET_USE_NAMESPACE

class QMouseEvent;
class QMenu;
class QPaintEvent;
class QAction;

/*!
 * \~chinese \class ButtonContent
 * \~chinese \brief 通知中心中气泡窗口无功能菜单的按钮
 */
class ButtonContent : public DWidget
{
    Q_OBJECT

    friend class Button;

private:
    explicit ButtonContent(QWidget *parent = nullptr);

    const QPixmap &pixmap() {return m_pixmap;}
    void setPixmap(const QPixmap &pixmap);

    const QString &text() {return m_text;}
    void setText(const QString &text);

    const QString &id() {return m_id;}
    void setId(const QString &id);

    const Qt::Alignment textAlignment() {return m_align;}
    void setTextAlignment(Qt::Alignment align);

    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

Q_SIGNALS:
    void clicked(bool checked = false);
    void toggled(const QString &id);

private:
    QPixmap m_pixmap;
    QString m_text;
    QString m_id;
    Qt::Alignment m_align;

private:
    void drawPixmap(QPainter *painter);
    void drawContent(QPainter *painter);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
};

/*!
 * \~chinese \class ButtonMenu
 * \~chinese \brief 通知中心中气泡窗口功能菜单类
 */
class ButtonMenu : public DWidget
{
    Q_OBJECT

    friend class Button;

private:
    explicit ButtonMenu(QWidget *parent = nullptr);

    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

Q_SIGNALS:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
};

/*!
 * \~chinese \class Button
 * \~chinese \brief 通知中心气泡上的有功能菜单的按钮
 */
class Button : public DWidget
{
    Q_OBJECT

public:
    explicit Button(QWidget *parent = nullptr);

    const QPixmap &pixmap() {return m_button->pixmap();}
    void setPixmap(const QPixmap &pixmap);

    const QString &text() {return m_button->text();}
    void setText(const QString &text);

    const QString &id() {return m_button->id();}
    void setId(const QString &id);

    const Qt::Alignment textAlignment() {return m_button->textAlignment();}
    void setTextAlignment(Qt::Alignment align);

    int radius() {return m_radius;}
    void setRadius(int radius);

    void addAction(QAction *action);
    void clear();

    bool boverState() const {return m_hover;}
    void setHoverState(bool state);

    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

Q_SIGNALS:
    void clicked(bool checked = false);
    void toggled(const QString &id);

private Q_SLOTS:
    void onMenuClicked();

private:
    ButtonContent *m_button = nullptr;
    ButtonMenu *m_menuArea = nullptr;
    DMenu *m_menu = nullptr;

    int m_radius;
    bool m_hover;

private:
    void drawBackground(QPainter *painter);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;          //当焦点移入或移出时背景发生变化
    virtual void focusOutEvent(QFocusEvent *event) override;
};
#endif // BUTTON_H
