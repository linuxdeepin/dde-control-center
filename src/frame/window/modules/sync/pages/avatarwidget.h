// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QLabel>

namespace DCC_NAMESPACE {
namespace sync {
class AvatarWidget : public QLabel
{
    Q_OBJECT
public:
    explicit AvatarWidget(QWidget *parent = nullptr);
    explicit AvatarWidget(const QString &avatar, QWidget *parent = nullptr);

    const QString avatarPath() const;
    void setAvatarPath(const QString &avatar);

protected:
    void paintEvent(QPaintEvent *e);

private:
    QPixmap m_avatar;
    QString m_avatarPath;
};
}
}
