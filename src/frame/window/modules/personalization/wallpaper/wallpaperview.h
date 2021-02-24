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

#ifndef WALLPAPERVIEW_H
#define WALLPAPERVIEW_H

#include "interface/namespace.h"

#include <DListView>

#include <QThread>

namespace DCC_NAMESPACE {
namespace personalization {

class Loader : public QThread
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr);
    void loadImage(const QStringList &papers, const QString &baseDir);
    void stopRun();

Q_SIGNALS:
    void newPixmap(const QPixmap &pix, const QString &name);

protected:
    virtual void run() override;

private:
    QStringList m_papers;
    QString m_baseDir;
    bool m_stop;
};

class WallpaperView : public Dtk::Widget::DListView
{
    Q_OBJECT
public:
    enum PaperRole {
        PixmapRole = Qt::UserRole + 0x1000,
        CheckRole
    };
public:
    explicit WallpaperView(QWidget *parent = nullptr);
    void resetModel(const QStringList &papers, const QString &baseDir, const QString &desktopPaper, const QString &lockPaper);

private Q_SLOTS:
    void appendNewWallpaper(const QPixmap &pix, const QString &name);

private:
    QStandardItemModel *m_paperModel;
    QString m_desktopPaper;
    QString m_lockPaper;
};

class WallpaperDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit WallpaperDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
}
}

#endif // WALLPAPERVIEW_H
