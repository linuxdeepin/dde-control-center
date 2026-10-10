// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "commoniconbutton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

#include <DGuiApplicationHelper>

DGUI_USE_NAMESPACE

CommonIconButton::CommonIconButton(QWidget *parent)
    : QWidget(parent)
    , m_refreshTimer(nullptr)
    , m_clickable(false)
    , m_hover(false)
    , m_state(Default)
    , m_lightThemeColor(Qt::black)
    , m_darkThemeColor(Qt::white)
    , m_activeState(false)
    , m_hoverEnable(true)
    , m_iconSize(QSize())
    , m_rotation(0)
{
    setAccessibleName("IconButton");
    setFixedSize(24, 24);
    if (parent)
        setForegroundRole(parent->foregroundRole());

    m_defaultPalette = palette();

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &CommonIconButton::refreshIcon);
}

void CommonIconButton::setStateIconMapping(QMap<State, QPair<QString, QString>> mapping)
{
    m_fileMapping = mapping;
}

void CommonIconButton::setState(State state)
{
    m_state = state;
    if (m_fileMapping.contains(state)) {
        auto pair = m_fileMapping.value(state);
        setIcon(pair.first, pair.second);
    }
    if (!m_icon.isNull()) {
        updatePalette();
    }
}

void CommonIconButton::startRotate()
{
    if (!m_refreshTimer) {
        m_refreshTimer = new QTimer(this);
        m_refreshTimer->setInterval(70);
        connect(m_refreshTimer, &QTimer::timeout, this, &CommonIconButton::startRotate);
    }
    m_refreshTimer->start();
    m_rotation += 54;
    update();
}

void CommonIconButton::stopRotate()
{
    m_refreshTimer->stop();
    m_rotation = 0;
    update();
}

void CommonIconButton::setIcon(const QIcon &icon, QColor lightThemeColor, QColor darkThemeColor)
{
    m_icon = icon;
    if (lightThemeColor.isValid() && darkThemeColor.isValid()) {
        m_lightThemeColor = lightThemeColor;
        m_darkThemeColor = darkThemeColor;
    }

    updatePalette();
}

void CommonIconButton::updatePalette()
{
    if (isEnabled()) {
        if (m_lightThemeColor.isValid() && m_darkThemeColor.isValid()) {
            if (!m_activeState) {
                QColor color = DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType ? m_lightThemeColor : m_darkThemeColor;
                auto pa = palette();
                pa.setColor(QPalette::WindowText, color);
                setPalette(pa);
            }
        }
    } else {
        setPalette(m_defaultPalette);
    }

    update();
}

void CommonIconButton::setActiveState(bool state)
{
    m_activeState = state;
    if (m_lightThemeColor.isValid() && m_darkThemeColor.isValid()) {
        updatePalette();
    }
    setForegroundRole(state ? QPalette::Highlight : QPalette::NoRole);
}

void CommonIconButton::setHoverEnable(bool enable)
{
    m_hoverEnable = enable;
}

void CommonIconButton::setIcon(const QString &icon, const QString &fallback, const QString &suffix)
{
    if (!m_fileMapping.contains(Default)) {
        m_fileMapping.insert(Default, QPair<QString, QString>(icon, fallback));
    }

    QString tmp = icon;
    QString tmpFallback = fallback;

    static auto addDarkMark = [suffix] (QString &file) {
        if (file.contains(suffix)) {
            file.replace(suffix, "-dark" + suffix);
        } else {
            file.append("-dark");
        }
    };
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        addDarkMark(tmp);
        addDarkMark(tmpFallback);
    }
    m_icon = QIcon::fromTheme(tmp);

    if (m_icon.isNull()) {
        m_icon = QIcon::fromTheme(tmpFallback);
    }

    if (m_icon.isNull()) {
        QString defaultIcon = m_fileMapping[State::Default].first;
        m_icon = QIcon::fromTheme(defaultIcon);
    }
    update();
}

void CommonIconButton::setHoverIcon(const QIcon &icon)
{
    m_hoverIcon = icon;
}

void CommonIconButton::setClickable(bool clickable)
{
    m_clickable = clickable;
}

bool CommonIconButton::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::Leave:
    case QEvent::Enter:
        m_hover = e->type() == QEvent::Enter;
        update();
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

void CommonIconButton::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (m_rotation != 0) {
        painter.translate(this->width() / 2, this->height() / 2);
        painter.rotate(m_rotation);
        painter.translate(-(this->width() / 2), -(this->height() / 2));
    }

    if (m_hoverEnable && m_hover && !m_hoverIcon.isNull()) {
        m_hoverIcon.paint(&painter, rect());
    } else if (!m_icon.isNull()) {
        if (!m_iconSize.isEmpty()) {
            const int left = (width() - m_iconSize.width()) / 2;
            const int top = (height() - m_iconSize.height()) / 2;
            m_icon.paint(&painter, rect().marginsRemoved(QMargins(left, top, left, top)));
        } else {
            m_icon.paint(&painter, rect());
        }
    }
}

void CommonIconButton::mousePressEvent(QMouseEvent *event)
{
    m_pressPos = event->pos();
    return QWidget::mousePressEvent(event);
}

void CommonIconButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_clickable && rect().contains(m_pressPos) && rect().contains(event->pos()) && (!m_refreshTimer || !m_refreshTimer->isActive())) {
        Q_EMIT clicked();
        return;
    }
    return QWidget::mouseReleaseEvent(event);
}

void CommonIconButton::refreshIcon()
{
    setState(m_state);
}

void CommonIconButton::setIconSize(const QSize &size)
{
    m_iconSize = size;
}

void CommonIconButton::setAllEnabled(bool enable)
{
    setEnabled(enable);
    updatePalette();
}
