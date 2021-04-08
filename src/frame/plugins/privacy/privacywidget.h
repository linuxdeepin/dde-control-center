/*
 * Copyright (C) 2020 Deepin Technology Co., Ltd.
 *
 * Author:     xiaoyaobing <xiaoyaobing@uniontech.com>
 *
 * Maintainer: xiaoyaobing <xiaoyaobing@uniontech.com>
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

#ifndef PRIVACYWIDGET_H
#define PRIVACYWIDGET_H

#include <dtkwidget_global.h>

#include <QWidget>
#include <QModelIndex>

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QVBoxLayout;
QT_END_NAMESPACE

class PrivacyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PrivacyWidget(QWidget *parent = nullptr);

private:
    void initUI();

Q_SIGNALS:
    void requestShowPrivacyView(int);

public Q_SLOTS:
    void onPrivacyItemViewClicked(const QModelIndex &index);

private:
    Dtk::Widget::DListView *m_privacyItemView;
    QStandardItemModel *m_privacyModel;
    QVBoxLayout *m_centralLayout;
    QModelIndex m_lastIndex;
};

#endif // PRIVACYWIDGET_H
