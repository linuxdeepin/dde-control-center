/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef RESOLUTIONWIDGET_H
#define RESOLUTIONWIDGET_H

#include "interface/namespace.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"

#include <DStandardItem>

#include <QComboBox>

class Resolution;
class DStandardItem;

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QHBoxLayout;
class QVBoxLayout;
class QStandardItemModel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class Monitor;
class DisplayModel;

enum ResizeDesktopRole {
    FillModeRole = Dtk::UserRole,
    DarkDefaultIconRole,
    DarkHighlightIconRole,
    DarkItemIconRole,
    DarkHoverIconRole,
    LightDefaultIconRole,
    LightHighlightIconRole,
    LightItemIconRole,
    LightHoverIconRole
};

class fillModeCombox : public QComboBox
{
    Q_OBJECT
public:
    explicit fillModeCombox(QWidget *parent = nullptr);
    virtual void showPopup() override; // 显示下拉框
    virtual void hidePopup() override; // 隐藏下拉框

    void setDefaultRoleIcon();
    void setItemRoleIcon();
    void setHoverRoleIcon();

public Q_SLOTS:
    void OnHighlighted(int index);

protected:
    bool event(QEvent *e) override;
};

class ResolutionWidget : public SettingsItem
{
    Q_OBJECT
public:
    explicit ResolutionWidget(int comboxWidth = 300, QWidget *parent = nullptr);
    ~ResolutionWidget();

    enum ResolutionRole { IdRole = Dtk::UserRole, WidthRole, HeightRole, RateRole };

public:
    void setModel(DisplayModel *model, Monitor *monitor);
    void setMonitor(Monitor *monitor);

Q_SIGNALS:
    void requestSetResolution(Monitor *monitor, const int mode);
    void requestSetFillMode(Monitor *monitor, const QString fillMode);
    void requestResizeDesktopVisibleChanged(bool visible);
    void requestCurrFillModeChanged(
            Monitor *monitor,
            const QString fillMode); // 用于复制模式下用主屏去更新其他屏幕铺满方式使用

public Q_SLOTS:
    void OnAvailableFillModesChanged(const QStringList &lstFillMode);
    void OnCurrentModeChanged(const Resolution &mode);

private:
    void initResolution();
    void initResizeDesktop();
    Dtk::Widget::DStandardItem *getItemIcon(const QString &key);
    void setResizeDesktopVisible(bool visible);
    void updateResizeDesktopVisible();
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QHBoxLayout *m_resolutionLayout;
    QHBoxLayout *m_resizeDesktopLayout;
    QVBoxLayout *m_contentLayout;
    QLabel *m_resolutionLabel;
    QComboBox *m_resolutionCombox;
    QLabel *m_resizeDesktopLabel;          // 屏幕显示方式
    fillModeCombox *m_resizeDesktopCombox; // 拉伸-居中-适应
    SettingsItem *m_resizeDesktopItem;

    DisplayModel *m_model;
    Monitor *m_monitor;
    QStandardItemModel *m_resoItemModel;
    QStandardItemModel *m_resizeItemModel;
};
} // namespace DCC_NAMESPACE
#endif // RESOLUTIONWIDGET_H
