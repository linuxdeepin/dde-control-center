// SPDX-FileCopyrightText: 2020 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "eyecomfortmodeapplet.h"
#include "constants.h"
#include "eyecomfortmodecontroller.h"

#include <DSwitchButton>
#include <DBlurEffectWidget>
#include <DFontSizeManager>

#include <QLabel>
#include <QHBoxLayout>
#include <QStandardItemModel>

DWIDGET_USE_NAMESPACE

EyeComfortmodeApplet::EyeComfortmodeApplet(QWidget *parent)
    : QWidget(parent)
    , m_title(new DLabel(this))
    , m_switchBtn(new DSwitchButton(this))
    , m_settingButton(new JumpSettingButton(this))
    , m_themeView(new PluginListView(this))
    , m_themeItemModel(new QStandardItemModel(m_themeView))
    , m_lightTheme(new PluginStandardItem(QIcon::fromTheme("theme-light"), tr("Light")))
    , m_darkTheme(new PluginStandardItem(QIcon::fromTheme("theme-dark"), tr("Dark")))
    , m_autoTheme(new PluginStandardItem(QIcon::fromTheme("theme-auto"), tr("Auto")))
{
    initUi();
    initConnect();
}

void EyeComfortmodeApplet::initUi()
{
    m_switchBtn->setFocusPolicy(Qt::NoFocus);
    m_switchBtn->setObjectName("SwitchBtn");
    setFixedWidth(Dock::DOCK_POPUP_WIDGET_WIDTH);

    DFontSizeManager::instance()->bind(m_title, DFontSizeManager::T5);
    m_title->setForegroundRole(QPalette::BrightText);

    QHBoxLayout *appletLayout = new QHBoxLayout;
    appletLayout->setSpacing(0);
    appletLayout->setContentsMargins(20, 0, 10, 0);
    appletLayout->addWidget(m_title);
    appletLayout->addStretch();
    appletLayout->addWidget(m_switchBtn);

    QVBoxLayout *themeLayout = new QVBoxLayout;
    themeLayout->setContentsMargins(10, 0, 10, 0);
    DLabel *titleLabel = new DLabel(tr("Theme"), this);
    titleLabel->setContentsMargins(10, 0, 0, 0);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5);
    themeLayout->addWidget(titleLabel);

    m_themeView->setModel(m_themeItemModel);

    m_themeItemModel->appendRow(m_lightTheme);
    m_themeItemModel->appendRow(m_darkTheme);
    m_themeItemModel->appendRow(m_autoTheme);
    DFontSizeManager::instance()->bind(m_themeView, DFontSizeManager::T6);

    themeLayout->setSpacing(10);
    themeLayout->addWidget(m_themeView);
    m_themeView->setMinimumHeight(130);

    QHBoxLayout *settingLayout = new QHBoxLayout;
    settingLayout->addWidget(m_settingButton);
    settingLayout->setContentsMargins(10, 0, 10, 0);
    auto mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->addLayout(appletLayout, 0);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(themeLayout, 0);
    mainLayout->addStretch();
    mainLayout->addLayout(settingLayout);
    mainLayout->setContentsMargins(0, 10, 0, 10);
    setLayout(mainLayout);
}

void EyeComfortmodeApplet::initConnect()
{
    connect(m_switchBtn, &DSwitchButton::checkedChanged, this, &EyeComfortmodeApplet::enableChanged);
    connect(m_settingButton, &JumpSettingButton::showPageRequestWasSended, this, &EyeComfortmodeApplet::requestHideApplet);
    connect(m_themeView, &PluginListView::clicked, this, &EyeComfortmodeApplet::onThemeListClicked);
}

void EyeComfortmodeApplet::setEnabled(bool enable)
{
    m_switchBtn->blockSignals(true);
    m_switchBtn->setChecked(enable);
    m_switchBtn->blockSignals(false);
}

void EyeComfortmodeApplet::setTitle(const QString &title)
{
    m_title->setText(title);
}

void EyeComfortmodeApplet::setIcon(const QIcon &icon)
{
    m_settingButton->setIcon(icon);
}

void EyeComfortmodeApplet::setDescription(const QString &description)
{
    m_settingButton->setDescription(description);
}

void EyeComfortmodeApplet::setDccPage(const QString &module, const QString &page)
{
    m_settingButton->setDccPage(module, page);
}

void EyeComfortmodeApplet::setEyeComfortVisible(bool visible)
{
    m_title->setVisible(visible);
    m_switchBtn->setVisible(visible);
    m_settingButton->setVisible(visible);
}

void EyeComfortmodeApplet::setHeight(int height)
{
    resize(width(), height);
}

void EyeComfortmodeApplet::onGlobalThemeChanged(const QString &value)
{
    if (value.endsWith(THEME_LIGHT)) {
        m_themeTypeName = THEME_LIGHT;
        m_lightTheme->updateState(PluginItemState::ConnectedOnlyPrompt);
        m_darkTheme->updateState(PluginItemState::NoState);
        m_autoTheme->updateState(PluginItemState::NoState);
    } else if (value.endsWith(THEME_DARK)) {
        m_themeTypeName = THEME_DARK;
        m_lightTheme->updateState(PluginItemState::NoState);
        m_darkTheme->updateState(PluginItemState::ConnectedOnlyPrompt);
        m_autoTheme->updateState(PluginItemState::NoState);
    } else {
        m_themeTypeName = THEME_AUTO;
        m_lightTheme->updateState(PluginItemState::NoState);
        m_darkTheme->updateState(PluginItemState::NoState);
        m_autoTheme->updateState(PluginItemState::ConnectedOnlyPrompt);
    }
}

void EyeComfortmodeApplet::onThemeListClicked(const QModelIndex &index)
{
    if (m_lightTheme == m_themeItemModel->itemFromIndex(index) && m_lightTheme->state() == NoState)
        switchTheme(THEME_LIGHT);
    else if (m_darkTheme == m_themeItemModel->itemFromIndex(index) && m_darkTheme->state() == NoState)
        switchTheme(THEME_DARK);
    else if (m_autoTheme == m_themeItemModel->itemFromIndex(index) && m_autoTheme->state() == NoState)
        switchTheme(THEME_AUTO);
}

void EyeComfortmodeApplet::switchTheme(const QString &themeTypeName)
{
    if (m_themeTypeName == themeTypeName)
        return;

    m_themeTypeName = themeTypeName;

    auto curGlobalTheme = EyeComfortModeController::ref().globalTheme();
    auto theme = curGlobalTheme.left(curGlobalTheme.indexOf("."));

    QString newGlobalTheme;
    if (themeTypeName == THEME_LIGHT) {
        newGlobalTheme = theme + ".light";
    } else if(themeTypeName == THEME_DARK){
        newGlobalTheme = theme + ".dark";
    } else {
        newGlobalTheme = theme;
    }

    Q_EMIT globalThemeChanged(newGlobalTheme);
}
