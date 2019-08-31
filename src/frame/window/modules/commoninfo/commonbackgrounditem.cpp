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

#include <QMimeDatabase>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QRect>
#include <QMimeData>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace commoninfo;

static const QStringList mimeTypeList { "image/jpg", "image/jpeg",
    "image/png", "image/tiff",
    "image/gif", "image/bmp" };
static const int ItemHeight = 187;

CommonBackgroundItem::CommonBackgroundItem(QFrame *parent)
    : SettingsItem(parent)
{
    setFixedHeight(ItemHeight);
    setAcceptDrops(true);
}

void CommonBackgroundItem::setThemeEnable(const bool state)
{
    m_themeEnable = state;
    update();
}

void CommonBackgroundItem::paintEvent(QPaintEvent *e)
{
    SettingsItem::paintEvent(e);

    if (m_background.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF pixRect(m_background.rect().topLeft(), m_background.rect().size() / devicePixelRatioF());
    pixRect.moveCenter(this->rect().center());

    if (m_themeEnable) {
        painter.drawPixmap(pixRect.toRect(), m_background);
    } else {
        painter.fillRect(this->rect(), Qt::black);
    }

    painter.restore();
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

void CommonBackgroundItem::updateBackground(const QPixmap &pixmap)
{
    m_background = pixmap;
    update();
}
