/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "commonbackgrounditem.h"

#include "widgets/basiclistdelegate.h"

#include <DPalette>
#include <DApplicationHelper>

#include <QMimeDatabase>
#include <QPainter>
#include <QPainterPath>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QRect>
#include <QMimeData>
#include <QLayout>
#include <QDebug>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace commoninfo;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

static const QStringList mimeTypeList { "image/jpg", "image/jpeg",
    "image/png", "image/tiff",
    "image/gif", "image/bmp" };
static const int ItemHeight = 187;
static const qreal Radius = 8.0;

CommonBackgroundItem::CommonBackgroundItem(QWidget *parent)
    : SettingsItem(parent)
{
    setMinimumHeight(ItemHeight);
    setAcceptDrops(true);
}

void CommonBackgroundItem::setThemeEnable(const bool state)
{
#ifndef DCC_DISABLE_GRUB_THEME
    m_themeEnable = state;
#else
    m_themeEnable = false;
#endif
    update();
}

void CommonBackgroundItem::paintEvent(QPaintEvent *e)
{
    SettingsItem::paintEvent(e);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QPalette pa = DApplicationHelper::instance()->palette(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(pa.color(QPalette::Window)));

    painter.drawRoundedRect(this->rect(), Radius, Radius);

    QRect pixRect(this->rect().x() + 10, this->rect().y() + 10,
                  this->rect().width() - 20, this->rect().height() - 20);
    pixRect.moveCenter(this->rect().center());

    QPainterPath path;
    path.addRoundedRect(pixRect, Radius, Radius);
    painter.setClipPath(path);

    if (m_themeEnable && !m_background.isNull()) {
        painter.drawPixmap(m_background.rect(), m_background);
    } else {
        painter.setBrush(QBrush(Qt::black));
        painter.drawRect(pixRect);
    }

    painter.setPen(Qt::NoPen);
    painter.end();
    if (m_isDrop) {
        painter.fillRect(this->rect(), QColor(0, 0, 0, 100));
    }
}

void CommonBackgroundItem::dragEnterEvent(QDragEnterEvent *e)
{
    QMimeDatabase b;
    Q_FOREACH (QUrl url, e->mimeData()->urls()) {
        QString mime = b.mimeTypeForUrl(url).name();

        if (!mimeTypeList.contains(mime))
            continue;

        if (e->mimeData()->hasUrls()) {
            e->acceptProposedAction();
            m_isDrop = true;
            update();
        }

        return;
    }
}

void CommonBackgroundItem::dragLeaveEvent(QDragLeaveEvent *e)
{
    Q_UNUSED(e);

    m_isDrop = false;
    update();
}

void CommonBackgroundItem::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->urls().isEmpty()) {
        return;
    }

    QList<QUrl> urls = e->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }
    QString path = urls[0].toLocalFile();
    if (path.isEmpty()) {
        return;
    }
    Q_EMIT requestSetBackground(path);
    e->acceptProposedAction();
    m_isDrop = false;
    update();
}

void CommonBackgroundItem::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void CommonBackgroundItem::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);

    updateBackground(m_basePixmap);
}

void CommonBackgroundItem::updateBackground(const QPixmap &pixmap)
{
    qDebug() << pixmap;
    m_basePixmap = pixmap;

    auto ratio = devicePixelRatioF();
    m_background = m_basePixmap.scaled(size() * ratio,
                                       Qt::IgnoreAspectRatio,
                                       Qt::FastTransformation);
    m_background.setDevicePixelRatio(ratio);
    update();
}
