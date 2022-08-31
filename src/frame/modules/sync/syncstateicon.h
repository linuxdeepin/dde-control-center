// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SYNCSTATEICON_H
#define SYNCSTATEICON_H

#include <QLabel>
#include <QVariantAnimation>

namespace dcc {
namespace cloudsync {
class SyncStateIcon : public QLabel
{
    Q_OBJECT
public:
    SyncStateIcon(QWidget* parent = nullptr);
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
}  // namespace cloudsync
}  // namespace dcc

#endif  // !SYNCSTATEICON_H
