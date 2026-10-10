// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef ICONBUTTON_H
#define ICONBUTTON_H

#include <QIcon>
#include <QWidget>
#include <QMap>

class CommonIconButton : public QWidget
{
public:
    enum State {
        Default,
        On,
        Off
    };

    Q_OBJECT
public:
    explicit CommonIconButton(QWidget *parent = nullptr);

    void setStateIconMapping(QMap<State, QPair<QString, QString>> mapping);
    void setState(State state);
    void setActiveState(bool state);
    bool activeState() const { return m_activeState; }
    void setHoverEnable(bool enable);

public Q_SLOTS:
    void setIcon(const QString &icon, const QString &fallback = "", const QString &suffix = ".svg");
    void setIcon(const QIcon &icon, QColor lightThemeColor = QColor(), QColor darkThemeColor = QColor());
    void setHoverIcon(const QIcon &icon);

    void setClickable(bool clickable);
    void setRotatable(bool rotatable);

signals:
    void clicked();

protected:
    bool event(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void startRotate();
    void stopRotate();

private:
    void refreshIcon();
    void updatePalette();

private:
    QTimer *m_refreshTimer;
    QIcon m_icon;
    QIcon m_hoverIcon;
    QPoint m_pressPos;
    int m_rotateAngle;
    bool m_clickable;
    bool m_rotatable;
    bool m_hover;
    QMap<State, QPair<QString, QString>> m_fileMapping;
    State m_state;
    QColor m_lightThemeColor;
    QColor m_darkThemeColor;
    bool m_activeState;
    bool m_hoverEnable;
};

#endif // DOCKICONBUTTON_H
