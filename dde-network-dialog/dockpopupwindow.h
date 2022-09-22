// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DOCKPOPUPWINDOW_H
#define DOCKPOPUPWINDOW_H

#include <darrowrectangle.h>

namespace Dtk {
    namespace Gui {
        class DRegionMonitor;
        class DWindowManagerHelper;
    } // namespace Gui
} // namespace Dtk

DWIDGET_USE_NAMESPACE

enum RunReason {
    Lock = 0,     // 锁屏插件唤起
    Greeter,  // greeter插件唤起
    Dock,     // 任务栏插件唤起
    Password, // 密码错误唤起
};

class DockPopupWindow : public Dtk::Widget::DArrowRectangle
{
    Q_OBJECT

public:
    explicit DockPopupWindow(RunReason runReaseon = Dock, QWidget *parent = 0);
    ~DockPopupWindow();

    bool model() const;

    void setContent(QWidget *content);
    void setBackground(const QImage &image);
    void closeDialog();

public slots:
    void show(const QPoint &pos, const bool model = false);
    void show(const int x, const int y);
    void onGlobMouseRelease(const QPoint &mousePos, const int flag);

signals:
    void accept() const;
    // 在把专业版的仓库降级到debian的stable时, dock出现了一个奇怪的问题:
    // 在plugins/tray/system-trays/systemtrayitem.cpp中的showPopupWindow函数中
    // 无法连接到上面这个信号: "accept", qt给出一个运行时警告提示找不到信号
    // 目前的解决方案就是在下面增加了这个信号
    void unusedSignal();
    void hideSignal();

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void enterEvent(QEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void paintEvent(QPaintEvent *event);

private slots:
    void compositeChanged();
    void ensureRaised();
    void lockFrontVisible(bool visible);

private:
    bool m_model;
    QPoint m_lastPoint;

    Dtk::Gui::DRegionMonitor *m_regionInter;
    Dtk::Gui::DWindowManagerHelper *m_wmHelper;

    QImage *m_bgImage;
    QImage *m_srcImage;
};

#endif // DOCKPOPUPWINDOW_H
