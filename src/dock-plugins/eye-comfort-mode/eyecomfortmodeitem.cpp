// SPDX-FileCopyrightText: 2020 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "eyecomfortmodeitem.h"
#include "eyecomfortmodeapplet.h"
#include "constants.h"
#include "eyecomfortmodecontroller.h"
#include "tipswidget.h"
#include "quickpanelwidget.h"


#include <DGuiApplicationHelper>

#include <QDBusConnection>
#include <QIcon>
#include <QJsonDocument>
#include <QPainter>
#include <QProcess>
#include <QVBoxLayout>

DGUI_USE_NAMESPACE

#define SHIFT "shift"
#define SETTINGS "settings"

EyeComfortModeItem::EyeComfortModeItem(QWidget* parent)
    : QWidget(parent)
    , m_tipsLabel(new Dock::TipsWidget(this))
    , m_applet(new EyeComfortmodeApplet(this))
    , m_icon(new CommonIconButton(this))
    , m_supportColorTemperature(EyeComfortModeController::ref().supportColorTemperature())
    , m_themeType(getThemeType(EyeComfortModeController::ref().globalTheme()))
{
    init();
}

void EyeComfortModeItem::init()
{
    m_tipsLabel->setVisible(false);

    m_applet->setVisible(false);
    m_applet->setDccPage("display", "Brightness");
    m_applet->setTitle(tr("Eye Comfort"));
    m_applet->setDescription(tr("Display settings"));
    m_applet->setIcon(QIcon::fromTheme("open-arrow"));
    m_applet->setEyeComfortVisible(m_supportColorTemperature);
    m_applet->onGlobalThemeChanged(EyeComfortModeController::ref().globalTheme());

    m_icon->setFixedSize(Dock::DOCK_PLUGIN_ITEM_FIXED_SIZE);

    m_quickPanel = new QuickPanelWidget(this);
    auto updateEyeComfortQuickPanel = [this]{
        m_quickPanel->setText(tr("Eye comfort"));
        m_quickPanel->setIcon(QIcon::fromTheme("eyeshield-on"));
        m_quickPanel->setButtonMode(QuickButton::ButtonMode::ClickButton);
        m_icon->setStateIconMapping({ { CommonIconButton::State::On, QPair<QString, QString>("eyeshield-on", ":/eyeshield-on.svg") },
            { CommonIconButton::State::Off, QPair<QString, QString>("eyeshield-off", ":/eyeshield-off.svg") } });
    };

    if (m_supportColorTemperature) {
        updateEyeComfortQuickPanel();
    } else {
        m_quickPanel->setText(tr("Theme"));
        m_quickPanel->setActive(false);
        m_quickPanel->setButtonMode(QuickButton::ButtonMode::DisplayButton);
    }
    connect(m_quickPanel, &QuickPanelWidget::iconClicked, this, [this](){
        m_supportColorTemperature ? EyeComfortModeController::ref().toggle() : requestExpand();
    });

    m_quickPanel->setActive(EyeComfortModeController::ref().isEyeComfortModeEnabled());

    auto vLayout = new QVBoxLayout(this);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(m_icon, 0, Qt::AlignCenter);

    m_applet->setEnabled(EyeComfortModeController::ref().isEyeComfortModeEnabled());
    updateDescription();
    refreshIcon();
    updateTips();

    connect(m_quickPanel, &QuickPanelWidget::panelClicked, this, &EyeComfortModeItem::requestExpand);
    connect(m_applet, &EyeComfortmodeApplet::enableChanged, &EyeComfortModeController::ref(), &EyeComfortModeController::enable);
    connect(m_applet, &EyeComfortmodeApplet::requestHideApplet, this, &EyeComfortModeItem::requestHideApplet);
    connect(m_applet, &EyeComfortmodeApplet::globalThemeChanged, &EyeComfortModeController::ref(), &EyeComfortModeController::setGlobalTheme);
    connect(&EyeComfortModeController::ref(), &EyeComfortModeController::globalThemeChanged, this, [this](const QString &value) {
        auto themeType = getThemeType(value);
        if (m_themeType == themeType)
            return;

        m_themeType = themeType;
        m_applet->onGlobalThemeChanged(value);
        updateDescription();
        refreshIcon();
        updateTips();
    });
    connect(&EyeComfortModeController::ref(), &EyeComfortModeController::supportColorTemperatureChanged, this, [this, updateEyeComfortQuickPanel](bool support) {
        if (m_supportColorTemperature == support)
            return;

        m_supportColorTemperature = support;
        m_applet->setEyeComfortVisible(m_supportColorTemperature);

        if (support)
            updateEyeComfortQuickPanel();
    });
    connect(&EyeComfortModeController::ref(), &EyeComfortModeController::eyeComfortModeChanged, m_quickPanel, &QuickPanelWidget::setActive);
    connect(&EyeComfortModeController::ref(), &EyeComfortModeController::eyeComfortModeChanged, this, [this](bool enable) {
        m_applet->setEnabled(enable);
        refreshIcon();
        updateDescription();
        updateTips();
    });
}

void EyeComfortModeItem::updateDescription()
{
    if (m_supportColorTemperature) {
        EyeComfortModeController::ref().isEyeComfortModeEnabled()
                ? m_quickPanel->setDescription(tr("On"))
                : m_quickPanel->setDescription(tr("Off"));
    } else {
        if (m_themeType == Light) {
            m_quickPanel->setDescription(tr("Light"));
        } else if (m_themeType == Dark) {
            m_quickPanel->setDescription(tr("Dark"));
        } else {
            m_quickPanel->setDescription(tr("Auto"));
        }
    }
}


QWidget* EyeComfortModeItem::tipsWidget()
{
    return m_tipsLabel;
}

QWidget* EyeComfortModeItem::popupApplet()
{
    return m_applet;
}

QWidget *EyeComfortModeItem::quickPanel()
{
    return m_quickPanel;
}

const QString EyeComfortModeItem::contextMenu() const
{
    QList<QVariant> items;
    QMap<QString, QVariant> settings;
    QMap<QString, QVariant> menu;
    if (m_supportColorTemperature) {
        items.reserve(2);

        QMap<QString, QVariant> shift;
        shift["itemId"] = SHIFT;
        if (EyeComfortModeController::ref().isEyeComfortModeEnabled())
            shift["itemText"] = tr("Disable eye comfort");
        else
            shift["itemText"] = tr("Enable eye comfort");
        shift["isActive"] = true;
        items.push_back(shift);

        settings["itemId"] = SETTINGS;
        settings["itemText"] = tr("Display Settings");
        settings["isActive"] = true;
        items.push_back(settings);

        menu["items"] = items;
        menu["checkableMenu"] = false;
        menu["singleCheck"] = false;
    }

    return QJsonDocument::fromVariant(menu).toJson();
}

void EyeComfortModeItem::invokeMenuItem(const QString menuId, const bool checked)
{
    Q_UNUSED(menuId);
    Q_UNUSED(checked);

    if (menuId == SHIFT) {
        EyeComfortModeController::ref().toggle();
    } else if (menuId == SETTINGS) {
        QStringList args {"--by-user", "org.deepin.dde.control-center", "--", "-p", "display"};
        QProcess::startDetached("dde-am", args);
        Q_EMIT requestHideApplet();
    }
}

void EyeComfortModeItem::refreshIcon()
{
    if (m_supportColorTemperature) {
        m_icon->setState(EyeComfortModeController::ref().isEyeComfortModeEnabled() ? CommonIconButton::On : CommonIconButton::Off);
    } else {
        if (m_themeType == Light) {
            m_quickPanel->setIcon(QIcon::fromTheme("theme-light"));
            m_icon->setIcon(QIcon::fromTheme("theme-light"));
        } else if (m_themeType == Dark) {
            m_quickPanel->setIcon(QIcon::fromTheme("theme-dark"));
            m_icon->setIcon(QIcon::fromTheme("theme-dark"));
        } else {
            m_quickPanel->setIcon(QIcon::fromTheme("theme-auto"));
            m_icon->setIcon(QIcon::fromTheme("theme-auto"));
        }
    }
}

void EyeComfortModeItem::updateTips()
{
    if (m_supportColorTemperature) {
        if (EyeComfortModeController::ref().isEyeComfortModeEnabled())
            m_tipsLabel->setText(tr("Eye comfort enabled"));
        else
            m_tipsLabel->setText(tr("Eye comfort disabled"));
    } else {
        if (m_themeType == Light) {
            m_tipsLabel->setText(tr("Theme: Light"));
        } else if (m_themeType == Dark) {
            m_tipsLabel->setText(tr("Theme: Dark"));
        } else {
            m_tipsLabel->setText(tr("Theme: Auto"));
        }
    }
}

QString EyeComfortModeItem::displayName() const
{
    return tr("Visual effect");
}

bool EyeComfortModeItem::airplaneEnable()
{
    return EyeComfortModeController::ref().isEyeComfortModeEnabled();
}

EyeComfortModeItem::ThemeType EyeComfortModeItem::getThemeType(const QString &globalTheme)
{
    if (globalTheme.endsWith(THEME_LIGHT))
        return Light;
    if (globalTheme.endsWith(THEME_DARK))
        return Dark;

    return Auto;
}

void EyeComfortModeItem::setAppletHeight(int height)
{
    if (m_applet) {
        m_applet->setHeight(height);
    }
}

void EyeComfortModeItem::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    if (position == Dock::Bottom || position == Dock::Top) {
        setMaximumWidth(height());
        setMaximumHeight(QWIDGETSIZE_MAX);
    } else {
        setMaximumHeight(width());
        setMaximumWidth(QWIDGETSIZE_MAX);
    }

    refreshIcon();
}
