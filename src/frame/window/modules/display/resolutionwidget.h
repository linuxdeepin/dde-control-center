// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RESOLUTIONWIDGET_H
#define RESOLUTIONWIDGET_H

#include "interface/namespace.h"
#include "widgets/settingsitem.h"
#include "widgets/settingsgroup.h"

#include <DStandardItem>
#include <DApplicationHelper>

#include <QComboBox>

class Resolution;

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QHBoxLayout;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace display {
class Monitor;
class DisplayModel;
} // namespace display

} // namespace dcc

using namespace dcc::widgets;

namespace DCC_NAMESPACE {

namespace display {

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
    fillModeCombox(QWidget *parent = nullptr);
    virtual void showPopup() override; //显示下拉框
    virtual void hidePopup() override; //隐藏下拉框
    
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
    enum ResolutionRole {
        IdRole = Dtk::UserRole,
        WidthRole,
        HeightRole,
        RateRole
    };

public:
    void setModel(dcc::display::DisplayModel *model, dcc::display::Monitor *monitor);
    void setMonitor(dcc::display::Monitor *monitor);

Q_SIGNALS:
    void requestSetResolution(dcc::display::Monitor *monitor, const int mode);
    void requestSetFillMode(dcc::display::Monitor *monitor, const QString fillMode);
    void requestResizeDesktopVisibleChanged(bool visible);
    void requestCurrFillModeChanged(dcc::display::Monitor *monitor, const QString fillMode); //用于复制模式下用主屏去更新其他屏幕铺满方式使用


public Q_SLOTS:
    void OnAvailableFillModesChanged(const QStringList &lstFillMode);
    void OnCurrentModeChanged(const Resolution &mode);

private:
    void initResolution();
    void initResizeDesktop();
    void setItemIcon();
    void setResizeDesktopVisible(bool visible);
    void updateResizeDesktopVisible();
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QHBoxLayout *m_resolutionLayout;
    QHBoxLayout *m_resizeDesktopLayout;
    QVBoxLayout *m_contentLayout;
    QLabel *m_resolutionLabel;
    QComboBox *m_resolutionCombox;
    QLabel *m_resizeDesktopLabel; //屏幕显示方式
    fillModeCombox *m_resizeDesktopCombox; //拉伸-居中-适应
    SettingsItem *m_resizeDesktopItem;

    dcc::display::DisplayModel *m_model;
    dcc::display::Monitor *m_monitor;
    QStandardItemModel *m_resoItemModel;
    QStandardItemModel *m_resizeItemModel;

    QMap<QString, DStandardItem*> m_mapFillModeItems; //铺满方式对应的items
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // RESOLUTIONWIDGET_H
