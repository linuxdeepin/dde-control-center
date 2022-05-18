/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef SYNCSTATEICON_H
#define SYNCSTATEICON_H

#include <QLabel>
#include <QVariantAnimation>

class SyncStateIcon : public QLabel
{
    Q_OBJECT
public:
    explicit SyncStateIcon(QWidget* parent = nullptr);
    ~SyncStateIcon();

    void setRotatePixmap(const QPixmap &pixmap);

    void play();
    void stop();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPixmap m_rotatePixmap;
    QVariantAnimation m_rotateAni;
    qreal m_rotateRatio;
};
#endif  // !SYNCSTATEICON_H
