//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "commonbackgrounditem.h"

#include <DPalette>
#include <DPaletteHelper>

#include <QMimeDatabase>
#include <QPainter>
#include <QPainterPath>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QRect>
#include <QMimeData>
#include <QLayout>
#include <QDebug>

using namespace DCC_NAMESPACE;
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
}

void CommonBackgroundItem::setThemeEnable(const bool state)
{
    setAcceptDrops(state);
    m_themeEnable = state;
    update();
}

void CommonBackgroundItem::paintEvent(QPaintEvent *e)
{
    SettingsItem::paintEvent(e);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    if (m_background.isNull()) {
        painter.fillRect(this->rect(), Qt::black);
        return;
    }

    QPalette pa = DPaletteHelper::instance()->palette(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(pa.color(QPalette::Window)));

    painter.drawRoundedRect(this->rect(), Radius, Radius);

    QRect pixRect(this->rect().x() + 10, this->rect().y() + 10,
                  this->rect().width() - 20, this->rect().height() - 20);
    pixRect.moveCenter(this->rect().center());

    QPainterPath path;
    path.addRoundedRect(pixRect, Radius, Radius);
    painter.setClipPath(path);

    if (m_themeEnable) {
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
    if (pixmap.isNull())
        return;

    m_basePixmap = pixmap;

    QSize pixmapSize = m_basePixmap.size();
    QSize showSize = size();
    QSize finalSize = ((float)showSize.width() / (float)pixmapSize.width()) * pixmapSize;
    qreal ratio = devicePixelRatioF();

    auto previewPixmap = m_basePixmap.scaled(finalSize,
                                       Qt::IgnoreAspectRatio,
                                       Qt::SmoothTransformation);


    if (previewPixmap.size().height() <= showSize.height()) {
        m_background = previewPixmap;
    } else {
        qreal centerHeight = (float)previewPixmap.size().height() / 2;
        qreal offsetHeight = (float)showSize.height() / 2;

        qreal start_y = centerHeight - offsetHeight;

        qreal start_x = 0;

        m_background = previewPixmap.copy(QRect(start_x, start_y, showSize.width(), showSize.height()));
    }

    m_background.setDevicePixelRatio(ratio);
    update();
}
