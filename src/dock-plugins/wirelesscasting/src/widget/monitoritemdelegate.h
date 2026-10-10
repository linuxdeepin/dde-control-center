// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <QStyledItemDelegate>
#include <QStandardItem>
#include <QBoxLayout>
#include <QPushButton>
#include <QStylePainter>
#include <QPainterPath>

#include <DLabel>
#include <DSpinner>
#include <DFrame>
#include <DPaletteHelper>
#include <DGuiApplicationHelper>
#include <DFontSizeManager>

#include "wirelesscastingmodel.h"
#include "commoniconbutton.h"

DWIDGET_USE_NAMESPACE

enum MonitorItemState
{
    NoState,            // 正常状态
    Connecting,         // 显示spinner旋片等，提示用户正在连接中
};

enum DisplayMode
{
    MergeMode = 1,
    ExtendMode,
    SingleMode
};

struct ItemSpacing
{
    int top;
    int height;
    int bottom;
    QStyleOptionViewItem::ViewItemPosition viewItemPosition;
};

class MonitorItem;
class MonitorItemWidget;
class CancelButton;
class TipLabel;
class MonitorItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit MonitorItemDelegate(QAbstractItemView* parent = nullptr);

    inline int expandItemHeight() const { return m_expandHeight; }
    inline int standardItemHeight() const { return m_standardHeight; }
    inline int itemSpacing() const { return m_bottomSpacing; }

    ItemSpacing getItemSpacing(const QModelIndex &index) const;

    // painting
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // editing
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void destroyEditor(QWidget *editor, const QModelIndex &index) const override;

private:
    const QAbstractItemView *m_view;
    const int m_standardHeight;
    const int m_expandHeight;
    const int m_bottomSpacing;
};

class MultiscreenOptionItemDelegate : public MonitorItemDelegate
{
    Q_OBJECT

public:
    explicit MultiscreenOptionItemDelegate(QAbstractItemView* parent = nullptr);
    // editing
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    const QAbstractItemView *m_view;
};

class MonitorItem : public QObject, public QStandardItem
{
    Q_OBJECT
public:
    explicit MonitorItem(Monitor *monitor);
    ~MonitorItem() override;

    void connMonitor();
    void disconnectMonitor();
    Monitor *monitor() { return m_monitor; }
    bool connectingState() { return m_connectingState; }

    void updateIcon(const QIcon &icon);
    QIcon icon() const { return m_icon; }

    void updateName(const QString &name);
    QString name() const { return m_name; }

    void updateItemState(const MonitorItemState state);
    MonitorItemState itemState() const { return m_itemState; }

Q_SIGNALS:
    void iconChanged(const QIcon &icon);
    void nameChanged(const QString &name);
    void itemStateChanged(const MonitorItemState state);

private Q_SLOTS:
    void onNdSinkStateChanged(const Monitor::NdSinkState state);

private:
    Monitor *m_monitor;
    bool m_connectingState;

    QIcon m_icon;
    QString m_name;
    MonitorItemState m_itemState;
};

class MultiscreenOptionItem : public QObject, public QStandardItem
{
    Q_OBJECT
public:
    explicit MultiscreenOptionItem(DisplayMode displayMode, const QString &name, const QString &screen);
    ~MultiscreenOptionItem() override;

    void updateIcon(const QIcon &icon);
    QIcon icon() const { return m_icon; }

    void updateName(const QString &name);
    const QString &name() const { return m_name; }
    const QString &screen() { return m_screen; }
    DisplayMode displayMode() const { return m_displayMode; }

Q_SIGNALS:
    void iconChanged(const QIcon &icon);
    void nameChanged(const QString &name);
    void selected(bool selected);

private:
    DisplayMode m_displayMode;
    QIcon m_icon;
    QString m_name;
    QString m_screen;
};

class MonitorItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorItemWidget(MonitorItem *item, QWidget *parent = nullptr);
    ~MonitorItemWidget() override;

public Q_SLOTS:
    void updateIcon(const QIcon &icon);
    void updateName(const QString &name);
    void updateItemState(const MonitorItemState state);

protected:
    bool event(QEvent *e) override;

private:
    void initUI();

private:
    MonitorItem *m_item;

    QVBoxLayout *m_mainLayout;
    DFrame *m_nameBox;
    CommonIconButton *m_icon;
    DLabel *m_nameLabel;
    DFrame *m_stateBox;
    DSpinner *m_spinner;
    TipLabel *m_tipLabel;
    CancelButton *m_disconnectBtn;
};

class MultiscreenOptionItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MultiscreenOptionItemWidget(MultiscreenOptionItem *item, QWidget *parent = nullptr);
    ~MultiscreenOptionItemWidget() override;

public Q_SLOTS:
    void updateIcon(const QIcon &icon);
    void updateName(const QString &name);
    void updateItemState(const MonitorItemState state);

protected:
    bool event(QEvent *e) override;

private:
    void initUI();

private:
    MultiscreenOptionItem *m_item;

    QHBoxLayout *m_mainLayout;
    DFrame *m_nameBox;
    CommonIconButton *m_icon;
    DLabel *m_nameLabel;
    CommonIconButton *m_selectIcon;
};

class TipLabel : public QLabel
{
    Q_OBJECT
public:
    explicit TipLabel(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        DFontSizeManager::instance()->bind(this, DFontSizeManager::T7);
        connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [this] {
            update();
        });
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)
        QPainter painter(this);

        QColor textColor = DPaletteHelper::instance()->palette(this).brush(QPalette::BrightText).color();
        textColor.setAlphaF(0.6);
        painter.setPen(textColor);
        painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, text());
    }
};

class CancelButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CancelButton(QWidget *parent = nullptr)
        : QPushButton(parent)
        , m_enter(false)
    {
        setFocusPolicy(Qt::NoFocus);
        setFlat(true);

        connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [this] {
            update();
        });
    }

protected:
    void enterEvent(QEnterEvent *event) override
    {
        QPushButton::enterEvent(event);
        m_enter = true;
        update();
    }

    void leaveEvent(QEvent *event) override
    {
        QPushButton::leaveEvent(event);
        m_enter = false;
        update();
    }

    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QColor bgColor, textColor;
        if (m_enter) {
            bgColor = DPaletteHelper::instance()->palette(this).brush(QPalette::BrightText).color();
            bgColor.setAlphaF(0.2);
        } else {
            bgColor = DPaletteHelper::instance()->palette(this).brush(QPalette::BrightText).color();
            bgColor.setAlphaF(0.1);
        }
        textColor = DPaletteHelper::instance()->palette(this).brush(QPalette::BrightText).color();

        QPainterPath path;
        path.addRoundedRect(rect(), 8, 8);
        painter.fillPath(path, bgColor);

        painter.setPen(textColor);
        painter.drawText(rect(), Qt::AlignCenter, text());
    }

private:
    bool m_enter;
};
