// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef AVATARWIDGET_H
#define AVATARWIDGET_H

#include "interface/namespace.h"

#include <QLabel>
#include <QWidget>

#define PIX_SIZE 60

namespace DCC_NAMESPACE {

class AvatarWidget : public QLabel
{
    Q_OBJECT

public:
    explicit AvatarWidget(QWidget *parent = 0);
    explicit AvatarWidget(const QString &avatar, QWidget *parent = 0);

    void setSelected(const bool selected = true);
    void setDeletable(const bool deletable = true);

    const QString avatarPath() const;
    void setAvatarPath(const QString &avatar);

    void setArrowed(const bool arrowed = true);

    inline bool arrowed() const { return m_arrowed; }

Q_SIGNALS:
    void clicked(const QString &iconPath) const;
    void requestDelete(const QString &iconPath) const;

protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void resizeEvent(QResizeEvent *event);

private:
    bool m_hover;
    bool m_deleable;
    bool m_selected;
    bool m_arrowed;

    QPixmap m_avatar;
    QString m_avatarPath;
};

} // namespace DCC_NAMESPACE

#endif // AVATARWIDGET_H
