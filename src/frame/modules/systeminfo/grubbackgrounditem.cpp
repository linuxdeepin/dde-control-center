/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "grubbackgrounditem.h"
#include "widgets/basiclistdelegate.h"

#include <QMimeDatabase>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QRect>
#include <QMimeData>

namespace dcc{
namespace systeminfo{

static const QStringList mimeTypeList { "image/jpg", "image/jpeg", "image/png", "image/tiff", "image/gif", "image/bmp" };

GrubBackgroundItem::GrubBackgroundItem(QFrame *parent)
    :SettingsItem(parent)
{
    setFixedHeight(187);

    setAcceptDrops(true);
}

void GrubBackgroundItem::setThemeEnable(const bool state)
{
    m_themeEnable = state;
    update();
}

void GrubBackgroundItem::paintEvent(QPaintEvent *e)
{
    if(!m_background.isNull()){
        QPainter painter(this);
        painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

        QRect pixRect(m_background.rect().topLeft(), m_background.rect().size() / devicePixelRatioF());
        pixRect.moveCenter(this->rect().center());

        if (m_themeEnable)
            painter.drawPixmap(pixRect, m_background);
        else
            painter.fillRect(this->rect(), Qt::black);

        painter.restore();
        painter.end();
        if(m_isDrop){
            painter.fillRect(this->rect(), QColor(0, 0, 0, 100));
        }
    }

    SettingsItem::paintEvent(e);
}

void GrubBackgroundItem::dragEnterEvent(QDragEnterEvent *e)
{
    QMimeDatabase b;
    Q_FOREACH (QUrl url, e->mimeData()->urls()) {
        QString mime = b.mimeTypeForUrl(url).name();

        if(!mimeTypeList.contains(mime))
            continue;

        if(e->mimeData()->hasUrls()){
            e->acceptProposedAction();
            m_isDrop = true;
            update();
        }

        return;
    }
}

void GrubBackgroundItem::dragLeaveEvent(QDragLeaveEvent *e)
{
    Q_UNUSED(e);

    m_isDrop = false;
    update();
}

void GrubBackgroundItem::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->urls().isEmpty())
        return;

    QList<QUrl> urls = e->mimeData()->urls();
    if (!urls.isEmpty()) {
        QString path = urls[0].toLocalFile();
        if(!path.isEmpty()){
            Q_EMIT requestSetBackground(path);
            e->acceptProposedAction();
            m_isDrop = false;
            update();
        }
    }
}

void GrubBackgroundItem::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void GrubBackgroundItem::updateBackground(const QPixmap &pixmap)
{
    m_background = pixmap;

    update();
}

}
}
