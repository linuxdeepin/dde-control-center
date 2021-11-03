/*
 * Copyright (C) 2014 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 *             listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
 *             fanpengcheng <fanpengcheng_cm@deepin.com>
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

#ifndef BUBBLE_H
#define BUBBLE_H

#include "constants.h"

#include <DBlurEffectWidget>
#include <DDialogCloseButton>
//#include <QDBusArgument>

#include <memory>
#include <com_deepin_sessionmanager.h>

using UserInter = com::deepin::SessionManager;

DWIDGET_USE_NAMESPACE

class AppIcon;
class AppBody;
class Button;
class ActionButton;
/*!
 * \~chinese \class Bubble
 * \~chinese \brief 自定义控件类,屏幕上方通知气泡的窗口类
 */
class Bubble : public DBlurEffectWidget
{
    Q_OBJECT

    Q_PROPERTY(QRect geometry READ geometry WRITE setFixedGeometry)

public:
    Bubble(QWidget *parent = nullptr, EntityPtr entity = nullptr,
           OSD::ShowStyle style = OSD::ShowStyle::BUBBLEWINDOW);

    EntityPtr entity() const;
    void setEntity(EntityPtr entity);
    void setEnabled(bool enable);

    inline int bubbleIndex() { return m_bubbleIndex; }

    void startMove(const QRect &startRect, const QRect &endRect, bool needDelete = false); // 负责位置的移动
    void setBubbleIndex(int index);                                                        // 设置通知的索引,在屏幕分辨率或主屏发生变化用于更新通知位置
    void updateGeometry();                                                                 // 更新通知的位置,分辨率被修改时使用

Q_SIGNALS:
    void expired(Bubble *);              // 超时消失时发出,动画执行完成后自动删除
    void dismissed(Bubble *);            // 点击后发出，动画执行完成后自动删除
    void notProcessedYet(EntityPtr ptr); // 触发'暂不处理'操作时发出，不会主动删除自身

    void actionInvoked(Bubble *, QString); // 不会主动删除自身
    void resetGeometry();

public Q_SLOTS:
    void setFixedGeometry(QRect rect);
    void onOpacityChanged(double value);

protected:
    virtual void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    virtual void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onOutTimerTimeout();

private:
    void initUI();
    void initConnections();
    void initTimers();

    void updateContent();
    bool containsMouse() const;

protected:
    EntityPtr m_entity;

    //controls
    AppIcon *m_icon;
    AppBody *m_body;
    ActionButton *m_actionButton;
    DDialogCloseButton *m_closeButton;

    QTimer *m_outTimer;
    QTimer *m_quitTimer;

    //---very private ,no get method
    QPoint m_clickPos;
    bool m_pressed = false;
    QString m_defaultAction;
    bool m_canClose = false;
    int m_bubbleIndex;
    bool m_beforeLocked;
};
#endif // BUBBLE_H
