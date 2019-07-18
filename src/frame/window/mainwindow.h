/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <QStack>

DWIDGET_USE_NAMESPACE
QT_BEGIN_NAMESPACE
class QHBoxLayout;
QT_END_NAMESPACE
class NavWinView;
class NavModel;
class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void pushWidget(QWidget *widget);
    void popWidget();

    void backTopPage();
    void updateFirstPage(int index);
    void createSecPage(int count = 1, int index = 15);

private:
    void onFirstItemClieck(const QModelIndex &index);
    void onSecondItemClieck(const QModelIndex &index);

    QHBoxLayout *m_contentLayout;
    QHBoxLayout *m_rightContentLayout;
    NavWinView *m_navView;
    NavWinView *m_navSecView;
    QWidget *m_rightView;
    NavModel *m_navModel;
    NavModel *m_navSecModel;
    QStack<QWidget *> m_contentStack;
};

#endif // MAINWINDOW_H
