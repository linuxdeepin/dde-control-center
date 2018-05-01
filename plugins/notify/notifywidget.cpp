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

#include "notifywidget.h"

#include <QVBoxLayout>
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

NotifyWidget::NotifyWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainVBLayout = new QVBoxLayout;
    DImageButton *clearAllButton = new DImageButton;
    m_notifyView = new NotifyView;
    m_notifyModel = new NotifyModel;
    m_notifyDelegate = new NotifyDelegate;

    clearAllButton->setText(tr("Clear all"));
    clearAllButton->setAlignment(Qt::AlignCenter);
    clearAllButton->setStyleSheet("background-color: rgba(255, 255, 255, 7.65); padding: 4px 0;");

    m_notifyView->setModel(m_notifyModel);
    m_notifyView->setItemDelegate(m_notifyDelegate);

    mainVBLayout->addWidget(clearAllButton);
    mainVBLayout->addWidget(m_notifyView);

    mainVBLayout->setSpacing(1);
    mainVBLayout->setMargin(0);
    mainVBLayout->setContentsMargins(0, 0, 0, 0);

    setContentsMargins(0, 0, 0, 0);
    setLayout(mainVBLayout);

    connect(clearAllButton, &DImageButton::clicked, this, &NotifyWidget::showClearAllAnim);
    connect(m_notifyView, &NotifyView::clicked, this, &NotifyWidget::showRemoveAnim);
    connect(m_notifyModel, &NotifyModel::removeAnimFinished, m_notifyModel, &NotifyModel::removeNotify);
    connect(m_notifyModel, &NotifyModel::clearAllAnimFinished, m_notifyModel, &NotifyModel::clearAllNotify);
}

void NotifyWidget::showRemoveAnim(const QModelIndex &index)
{
    m_notifyModel->showAnimRemove(index, m_notifyView->width());
}

void NotifyWidget::showClearAllAnim()
{
    m_notifyModel->showAnimClearAll(m_notifyView->width());
}
