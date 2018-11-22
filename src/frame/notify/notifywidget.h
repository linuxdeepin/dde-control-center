/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <190771752ri@gmail.com>
 *
 * Maintainer: listenerri <190771752ri@gmail.com>
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

#ifndef NOTIFYWIDGET_H
#define NOTIFYWIDGET_H

#include "notifymodel.h"
#include "notifydelegate.h"
#include "notifyview.h"

#include <dimagebutton.h>
#include <QWidget>
#include <QLabel>

DWIDGET_USE_NAMESPACE

class NotifyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotifyWidget(QWidget *parent = nullptr);

private Q_SLOTS:
    void showRemoveAnim(const QModelIndex &index);
    void showClearAllAnim();
    void onNotifyClearStateChanged(bool isClear);
    void onRemoveBtnClicked();
    void onRemoveAnimFinished(const QModelIndex &index);

private:
    DImageButton *m_clearAllButton;
    NotifyView *m_notifyView;
    NotifyModel *m_notifyModel;
    NotifyDelegate *m_notifyDelegate;
    QLabel *m_noNotify;
};

#endif // NOTIFYWIDGET_H
