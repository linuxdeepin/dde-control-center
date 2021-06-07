/*
   * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     xiechuan <xiechuan@uniontech.com>
   *
   * Maintainer: xiechaun <xiechaun@uniontech.com>
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

#include "wallpaperpage.h"
#include "perssonalizationthemewidget.h"
#include "roundcolorwidget.h"
#include "window/utils.h"
#include "widgets/dccslider.h"
#include "widgets/titledslideritem.h"
#include "widgets/settingsitem.h"
#include "widgets/titlelabel.h"
#include "widgets/comboxwidget.h"
#include "personalizationgeneral.h"
#include "wallpaper.h"
#include "personalizationfontswidget.h"
#include "wallpaper/papersettingwidget.h"

#include <DStyle>
#include <DSwitchButton>
#include <DPalette>
#include <DFontSizeManager>
#include <DApplicationHelper>
#include <DPlatformTheme>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>
#include <QRect>
#include <QPalette>
#include <QSettings>
#include <QComboBox>
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QScrollArea>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::personalization;

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

const QList<QString> ACTIVE_COLORS = {
    "#D8316C",
    "#FF5D00",
    "#F8CB00",
    "#23C400",
    "#00A48A",
    "#0081FF",
    "#3C02FF",
    "#8C00D4",
    "#4D4D4D"
};

struct ColorStru {
    int r = 0;
    int g = 0;
    int b = 0;
    bool operator==(const ColorStru b) const
    {
        return (b.r == this->r && b.g == this->g && b.b == this->b);
    }
};

const QList<ColorStru> ACTIVE_COLORST = {
    {216, 49, 108},
    {255, 93, 0},
    {248, 203, 0},
    {35, 196, 0},
    {0, 164, 138},
    {0, 129, 255},
    {60, 2, 215},
    {140, 0, 212},
    {77, 77, 77},
};

const QList<ColorStru> Dark_ACTIVE_COLORST = {
    {179, 33, 87},
    {204, 71, 0},
    {198, 162, 0},
    {28, 156, 0},
    {0, 130, 108},
    {0, 105, 204},
    {44, 0, 206},
    {109, 0, 168},
    {61, 61, 61},
};

WallpaperPage::WallpaperPage(QWidget *parent)
    : QWidget(parent)
    , m_themes(new PerssonalizationThemeWidget())
    , m_fonts(new PersonalizationFontsWidget)
    , m_bgWidget(new RingColorWidget)
    #ifndef USE_TABLET
    , m_wallpaper(new Wallpaper(this))
    #endif
    , m_wallpaperSetting(nullptr)
{
    // appearance
    // ~ contents_path /personalization/General
    QLabel *theme = new TitleLabel(tr("Theme"));
    DFontSizeManager::instance()->bind(theme, DFontSizeManager::T5, QFont::DemiBold);
    theme->setContentsMargins(20, 0, 0, 10);
    QVBoxLayout *centralLayout = new QVBoxLayout();
    centralLayout->addWidget(theme);
    // pictures and types
    m_themes->setFixedHeight(268);
    m_themes->setMainLayout(new QHBoxLayout(), true);
    centralLayout->addWidget(m_themes);

    m_themeType = DGuiApplicationHelper::instance()->themeType();

    // active colors
    // ~ contents_path /personalization/General
    QLabel *active = new TitleLabel(tr("Accent Color"));
    active->setAccessibleName("Accent Color");
    DFontSizeManager::instance()->bind(active, DFontSizeManager::T5, QFont::DemiBold);
    active->setContentsMargins(20, 20, 0, 10);
    centralLayout->addWidget(active);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->setAlignment(Qt::AlignLeft);
    m_bgWidget->setLayout(colorLayout);
    if (DGuiApplicationHelper::isTabletEnvironment()) {
        m_bgWidget->setFixedHeight(80);
    }
    else {
        m_bgWidget->setFixedHeight(40);
    }
    colorLayout->setContentsMargins(20, 10, 16, 20);
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, this);
    int totalSpace = borderWidth + borderSpacing + RoundColorWidget::EXTRA; //2px extra space to avoid line cutted off

    for (ColorStru aColor : (m_themeType == DGuiApplicationHelper::ColorType::LightType ? ACTIVE_COLORST : Dark_ACTIVE_COLORST)) {
        QColor color;
        color.setRgb(aColor.r, aColor.g, aColor.b);
        RoundColorWidget *colorItem = new RoundColorWidget(color, this);
        QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
        effect->setBlurRadius(17);        // 阴影圆角的大小

        color.setAlpha(68);
        effect->setColor(color);      //阴影的颜色
        effect->setOffset(0,5);
        colorItem->setGraphicsEffect(effect);

        if (m_themeType == DGuiApplicationHelper::ColorType::LightType) {
            colorItem->setAccessibleName(ACTIVE_COLORS[ACTIVE_COLORST.indexOf(aColor)]);
        } else {
            colorItem->setAccessibleName(ACTIVE_COLORS[Dark_ACTIVE_COLORST.indexOf(aColor)]);
        }

        DPalette pa = colorItem->palette();
        pa.setBrush(DPalette::Base, color);
        colorItem->setPalette(pa);
        if (DGuiApplicationHelper::isTabletEnvironment()) {
            colorItem->setFixedSize(40 + 2 * totalSpace, 60);
        } else {
            colorItem->setFixedSize(20 + 2 * totalSpace, 40);
        }
        colorLayout->addWidget(colorItem);
        connect(colorItem, &RoundColorWidget::clicked, this, &WallpaperPage::onActiveColorClicked);
        m_activeColorsList.append(colorItem);
    }

    centralLayout->addWidget(m_bgWidget);

#ifndef USE_TABLET
    QLabel *wallPaperLabel = new TitleLabel(tr("Wallpaper"));
    DFontSizeManager::instance()->bind(wallPaperLabel, DFontSizeManager::T5, QFont::DemiBold);
    wallPaperLabel->setContentsMargins(20, 20, 0, 10);
    centralLayout->addWidget(wallPaperLabel);
    centralLayout->addWidget(m_wallpaper);
    connect(m_wallpaper, &Wallpaper::requestSetWallpaper, this, &WallpaperPage::showWallpaperWidget);
#endif

    QLabel *font = new TitleLabel(tr("Size"));
    font->setContentsMargins(20, 20, 0, 10);
    DFontSizeManager::instance()->bind(font, DFontSizeManager::T5, QFont::DemiBold);
    centralLayout->addWidget(font);
    centralLayout->addWidget(m_fonts);

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    m_contentLayout = new QVBoxLayout;
    m_contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_contentLayout->setMargin(0);
    setLayout(m_contentLayout);
    m_contentLayout->addWidget(m_scrollArea);

    QHBoxLayout* mainLayout =  new QHBoxLayout;
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->addStretch();
    mainLayout->addLayout(centralLayout);
    mainLayout->addStretch();

    auto tw = new QWidget();
    tw->setLayout(mainLayout);
    m_scrollArea->setWidget(tw);
}

void WallpaperPage::setModel(dcc::personalization::PersonalizationModel *model)
{
    m_model = model;
    m_themes->setModel(model->getWindowModel());
    m_fonts->setModel(model);

    connect(model, &dcc::personalization::PersonalizationModel::onActiveColorChanged, this,
            &WallpaperPage::onActiveColorChanged);

#ifndef USE_TABLET
    connect(model, &dcc::personalization::PersonalizationModel::lockPaperChanged, [ = ](const QString &wallpaper) {
        m_wallpaper->setLockPaperPath(wallpaper);
    });
    connect(model, &dcc::personalization::PersonalizationModel::desktopPaperChanged, [ = ](const QString &wallpaper) {
        m_wallpaper->setDesktopPaperPath(wallpaper);
    });

    m_wallpaper->setDesktopPaperPath(m_model->desktopPaper());
    m_wallpaper->setLockPaperPath(m_model->lockPaper());
#endif

    onActiveColorChanged(model->getActiveColor());
}

void WallpaperPage::setWorker(dcc::personalization::PersonalizationWork *work)
{
    m_work = work;
}

void WallpaperPage::updateThemeColors(DGuiApplicationHelper::ColorType type)
{
    int count = 0;
    for (auto item : m_activeColorsList) {
        ColorStru acolor = type == DGuiApplicationHelper::ColorType::LightType ? ACTIVE_COLORST[count] : Dark_ACTIVE_COLORST[count];
        count++;
        QColor color;
        color.setRgb(acolor.r, acolor.g, acolor.b);
        item->setColor(color);
        DPalette pa = item->palette();
        pa.setBrush(DPalette::Base, color);
        item->setPalette(pa);
        if (dynamic_cast<QGraphicsDropShadowEffect *>(item->graphicsEffect())) {
            color.setAlpha(68);
            dynamic_cast<QGraphicsDropShadowEffect *>(item->graphicsEffect())->setColor(color);
        }
    }
}

void WallpaperPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (m_themeType != DGuiApplicationHelper::instance()->themeType()) {
        m_themeType = DGuiApplicationHelper::instance()->themeType();
        updateThemeColors(m_themeType);
    }

    DStylePainter painter(this);

    QRect r = m_bgWidget->geometry();
    int frame_radius = 18;

    QPainterPath path;
    path.addRoundedRect(r, frame_radius, frame_radius);
    const DPalette &pal = DApplicationHelper::instance()->palette(this);
    painter.fillPath(path, pal.brush(DPalette::ItemBackground));
}

void WallpaperPage::updateActiveColors(RoundColorWidget *selectedWidget)
{
    for (RoundColorWidget *item : m_activeColorsList) {
        if (item == selectedWidget) {
            m_bgWidget->setSelectedItem(item);
            item->setSelected(true);
        } else {
            item->setSelected(false);
        }
    }
}

void WallpaperPage::onActiveColorChanged(const QString &newColor)
{
    int colorIndex = ACTIVE_COLORS.indexOf(newColor.trimmed());

    if (colorIndex >= 0)
        updateActiveColors(m_activeColorsList[colorIndex]);
}

void WallpaperPage::onActiveColorClicked()
{
    RoundColorWidget *activeColor = qobject_cast<RoundColorWidget *>(sender());

    RoundColorWidget* item = dynamic_cast<RoundColorWidget*>(sender());
    if(nullptr != item) {
        m_bgWidget->setSelectedItem(item);
        m_bgWidget->update();
    }
    // 设置active color
    QString strColor = ACTIVE_COLORS[m_activeColorsList.indexOf(activeColor)];
    qDebug() << Q_FUNC_INFO << " strColor : " << strColor;
    Q_EMIT requestSetActiveColor(strColor);
}

void WallpaperPage::showWallpaperWidget()
{
    m_wallpaperSetting = new PaperSettingWidget(m_work, m_model);
    m_contentLayout->addWidget(m_wallpaperSetting);
    connect(m_wallpaperSetting, &PaperSettingWidget::requestExit, this, &WallpaperPage::rebackMainWidget);
    m_scrollArea->setVisible(false);
    m_wallpaperSetting->setVisible(true);
    m_wallpaperSetting->setFocus();
}

void WallpaperPage::rebackMainWidget()
{
    m_scrollArea->setVisible(true);
    m_wallpaperSetting->deleteLater();
}
