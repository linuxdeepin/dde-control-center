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

#include "wallpapersnippet.h"

#include <DStyle>
#include <DSvgRenderer>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDBusConnection>
#include <QDir>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE::personalization;

WallpaperItem::WallpaperItem(QString path, QWidget *parent)
    : QWidget(parent)
    , m_render(new DSvgRenderer())
{
    m_render->load(path);
    setFixedSize(QSize(250, 140));
}

void WallpaperItem::setPath(const QString &picPath)
{
    m_path = picPath;
    m_path = m_path.contains("file://") ? m_path.remove("file://") : m_path;

    QSize defaultSize = m_render->defaultSize();
    int margins = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameMargins));
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);
    int totalSpace = borderWidth + borderSpacing + margins;
    setFixedSize(defaultSize.width() + 2 * totalSpace, defaultSize.height() + 2 * totalSpace);
    update();
}

void WallpaperItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    int radius = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameRadius), nullptr, nullptr);
    int margins = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FrameMargins));
    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, nullptr);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, nullptr);
    int totalSpace = borderWidth + borderSpacing + margins;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //first draw image
    const auto ratio = devicePixelRatioF();
    QSize defaultSize = m_render->defaultSize() * ratio;
    QRect picRect = rect().adjusted(totalSpace, totalSpace, -totalSpace, -totalSpace);

    QImage img(defaultSize * ratio, QImage::Format_ARGB32_Premultiplied);
    img.load(m_path);
    painter.drawImage(picRect, img, img.rect());

    img = m_render->toImage(defaultSize);
    painter.drawImage(picRect, img, img.rect());

    //second draw picture rounded rect bound
    QPen pen;
    pen.setColor(palette().base().color());
    painter.setPen(pen);
    painter.drawRoundedRect(picRect, radius, radius);

    //third fill space with base brush
    QPainterPath picPath;
    picPath.addRect(picRect);
    QPainterPath roundPath;
    roundPath.addRoundedRect(picRect, radius, radius);
    QPainterPath anglePath = picPath - roundPath;
    painter.fillPath(anglePath, palette().base().color());
    painter.strokePath(picPath, palette().base().color());
}

WallpaperSnippet::WallpaperSnippet(QWidget *parent)
    : SettingsItem(parent)
    , m_mainLayout(new QHBoxLayout(this))
{
    initUi();
}

void WallpaperSnippet::initUi()
{
    setAccessibleName("WallpaperSnippet");
    setFixedHeight(232);

    WallpaperItem *item = addItem(":/personalization/lockscreen.svg", tr("Lock screen"));
    connect(this, &WallpaperSnippet::lockPaperChanged, [=](QString &path) { item->setPath(path); });

    item = addItem(":/personalization/desktop.svg", tr("Home screen"));
    connect(this, &WallpaperSnippet::desktopPaperChanged, [=](QString &path) { item->setPath(path); });

    m_mainLayout->addStretch();
    m_mainLayout->setContentsMargins(80, 40, 60, 0);
}

/**
 * @brief WallpaperSnippet::addItem 添加壁纸
 * @param path　壁纸路径
 * @param text 壁纸类型
 * @return
 */
WallpaperItem *WallpaperSnippet::addItem(QString path, QString text)
{
    WallpaperItem *item = new WallpaperItem(path);

    QVBoxLayout *vLayout = new QVBoxLayout();
    QLabel *lab = new QLabel(text);
    vLayout->addWidget(item);
    vLayout->addWidget(lab, 0, Qt::AlignCenter);
    vLayout->setSpacing(12);
    vLayout->addStretch();
    m_mainLayout->addLayout(vLayout);

    return item;
}
