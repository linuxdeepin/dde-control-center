// Copyright (C) 2026 Deepin Technology Co., Ltd.
// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QUICKPANELWIDGET_H
#define QUICKPANELWIDGET_H

#include <DFloatingButton>
#include <DLabel>
#include <DStyleOption>

#include <QWidget>

DWIDGET_USE_NAMESPACE

class QuickButton : public DFloatingButton
{
public:

    enum ButtonMode {
        ClickButton,
        DisplayButton
    };

    QuickButton(QWidget *parent = nullptr)
        : DFloatingButton(parent)
        , m_mode(ButtonMode::ClickButton)
    {
    }

    void setButtonMode(ButtonMode mode) {
        if (m_mode == mode)
            return;

        m_mode = mode;
        DStyleOptionButton option;
        initStyleOption(&option);
    }

protected:
    void initStyleOption(DStyleOptionButton *option) const override;

private:
    ButtonMode m_mode;
};

class QLabel;
class QuickPanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuickPanelWidget(QWidget *parent = nullptr);
    ~QuickPanelWidget() override;

public Q_SLOTS:
    void setIcon(const QIcon &icon);
    void setText(const QString &text);
    void setDescription(const QString &description);
    void setActive(bool active);
    void setButtonMode(QuickButton::ButtonMode mode);

Q_SIGNALS:
    void panelClicked();
    void iconClicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void initUi();
    void initConnection();

private:
    QuickButton *m_iconWidget;
    Dtk::Widget::DLabel *m_nameLabel;
    Dtk::Widget::DLabel *m_stateLabel;
    Dtk::Widget::DIconButton *m_expandLabel;
    QPoint m_clickPoint;
    bool m_eyeComfortModeEnabled;
};

#endif // QUICKPANELWIDGET_H
