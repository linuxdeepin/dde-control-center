/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "removeuserdialog.h"

#include <QUrl>
#include <QPainter>
#include <QPainterPath>

#include <dcheckbox.h>

#include "avatarwidget.h"

static QPixmap RoundPixmap(const QPixmap &pix) {
    QPixmap ret(pix.size());
    ret.fill(Qt::transparent);

    QPainter painter(&ret);
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

    QPainterPath path;
    path.addEllipse(ret.rect());
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, pix);

    painter.end();

    return ret;
}

RemoveUserDialog::RemoveUserDialog(const User *user, QWidget *parent) :
    DDialog(parent),
    m_deleteHome(false)
{
    setTitle(tr("Are you sure to delete %1?").arg(user->name()));

    const QString iconFile = QUrl(user->currentAvatar()).toLocalFile();
    const QPixmap pix = QPixmap(iconFile).scaled(48, 48, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    setIconPixmap(RoundPixmap(pix));

    DCheckBox *box = new DCheckBox(tr("Delete account directory"));
    addContent(box, Qt::AlignTop);

    QStringList buttons;
    buttons << tr("Cancel") << tr("Delete");
    addButtons(buttons);

    connect(box, &DCheckBox::toggled, [this, box] {
        m_deleteHome = box->checkState() == Qt::Checked;
    });
}

bool RemoveUserDialog::deleteHome() const
{
    return m_deleteHome;
}
