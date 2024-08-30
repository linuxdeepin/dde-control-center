//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "removeuserdialog.h"

#include <QUrl>
#include <QPainter>
#include <QPainterPath>
#include <QCheckBox>

#include "avatarwidget.h"

using namespace DCC_NAMESPACE;

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

RemoveUserDialog::RemoveUserDialog(const User *user, QWidget *parent)
    : DDialog(parent)
    , m_deleteHome(true)
{
    setTitle(tr("Are you sure you want to delete this account?"));

    const auto ratio = devicePixelRatioF();
    const QString iconFile = QUrl(user->currentAvatar()).toLocalFile();
    const QPixmap pix = QPixmap(iconFile).scaled(48 * ratio, 48 * ratio, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    QPixmap p = RoundPixmap(pix);
    p.setDevicePixelRatio(ratio);
    setIcon(p);

    QCheckBox *box = new QCheckBox(tr("Delete account directory"));
    box->setChecked(true);
    box->setAccessibleName("Delete_Account_Checkbox");
    addContent(box, Qt::AlignTop);

    QStringList buttons;
    buttons << tr("Cancel") << tr("Delete");
    addButtons(buttons);

    connect(box, &QCheckBox::toggled, [this, box] {
        m_deleteHome = box->checkState() == Qt::Checked;
    });
}

bool RemoveUserDialog::deleteHome() const
{
    return m_deleteHome;
}
