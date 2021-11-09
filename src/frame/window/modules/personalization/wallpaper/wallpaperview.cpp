/*
 * Copyright (C) 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     liuxing <liuxing@uniontech.com>
 *
 * Maintainer: liuxing <liuxing@uniontech.com>
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

#include "wallpaperview.h"

#include <DStyle>

#include <QPainter>
#include <QPainterPath>

const int PixmapSize = 230;

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::personalization;

// class Loader
Loader::Loader(QObject *parent)
    : QThread (parent)
    , m_stop(true)
{
}

void Loader::loadImage(const QStringList &papers, const QString &baseDir)
{
    m_papers = papers;
    m_baseDir = baseDir;
    m_stop = false;
    this->start();
}

void Loader::stopRun()
{
    m_stop = true;
}

void Loader::run()
{
    for (const QString &paper : m_papers) {
        if (m_stop) {
            return;
        }

        QImage img;
        img.load(m_baseDir + paper);
        if (img.isNull()) {
            qWarning() << "load image failed, path:" << m_baseDir + paper;
            continue;
        }
        Q_EMIT newPixmap(QPixmap::fromImage(img.scaled(PixmapSize, PixmapSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)), paper);
    }
}

// class WallpaperView
WallpaperView::WallpaperView(QWidget *parent) : DListView(parent)
{
    m_paperModel = new QStandardItemModel;
    setModel(m_paperModel);

    WallpaperDelegate *delegate = new WallpaperDelegate;
    this->setItemDelegate(delegate);

    setViewMode(QListView::IconMode);
    setDragEnabled(false);
    setEditTriggers(QListView::NoEditTriggers);
    setFrameShape(QFrame::Shape::NoFrame);
    setResizeMode(DListView::Adjust);
    setSelectionMode(QAbstractItemView::NoSelection);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSpacing(20);
}

void WallpaperView::resetModel(const QStringList &papers, const QString &baseDir
                               , const QString &desktopPaper, const QString &lockPaper)
{
    static Loader load;
    if (load.isRunning()) {
        load.stopRun();
        load.wait();
    }

    m_paperModel->clear();
    m_desktopPaper = desktopPaper;
    m_lockPaper = lockPaper;

    load.loadImage(papers, baseDir);
    connect(&load, &Loader::newPixmap, this, &WallpaperView::appendNewWallpaper);
}

void WallpaperView::appendNewWallpaper(const QPixmap &pix, const QString &name)
{
    DStandardItem *item = new DStandardItem;
    item->setData(pix, PixmapRole);
    item->setData(m_desktopPaper.contains(name) || m_lockPaper.contains(name), CheckRole);
    m_paperModel->appendRow(item);
}

// class WallpaperDelegate
WallpaperDelegate::WallpaperDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void WallpaperDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }

    painter->setRenderHints(painter->renderHints()
                            | QPainter::Antialiasing
                            | QPainter::SmoothPixmapTransform);

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    QStyle *style = opt.widget->style();

    const QPixmap &pixmap = index.data(WallpaperView::PixmapRole).value<QPixmap>();
    if (pixmap.isNull()) {
        return;
    }

    QPainterPath path;
    path.addRoundedRect(opt.rect, 8.0, 8.0);
    painter->setClipPath(path);
    painter->drawPixmap(opt.rect, pixmap);
    painter->setClipping(false);

    // 绘制选中图标
    if (index.data(WallpaperView::CheckRole).toBool()) {
        const QPoint pos = opt.rect.topRight() - QPoint(10, -26);
        QRect checkRect(pos - QPoint(16, 16), pos);
        opt.rect = checkRect;
        opt.state |= QStyle::State_On;
        style->drawPrimitive(DStyle::PE_IndicatorItemViewItemCheck, &opt, painter, nullptr);
    }
}

QSize WallpaperDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(PixmapSize, PixmapSize);
}
