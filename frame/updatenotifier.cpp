/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "updatenotifier.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>

static const QString IgnoredPkgsKey = "IgnoredPackages";

UpdateNotifier::UpdateNotifier(QWidget *parent)
    : QFrame(parent),
      m_icon(new QLabel),
      m_title(new NormalLabel(tr("Updates"))),
      m_content(new TipsLabel(tr("Updates detected, click to view"))),
      m_closeButton(new DImageButton(this)),
      m_settings(new QSettings("deepin", "dcc-update-notifier")),
      m_updaterInter(new com::deepin::lastore::Updater("com.deepin.lastore",
                                                       "/com/deepin/lastore",
                                                       QDBusConnection::systemBus(), this))
{
    setVisible(false);
    setFixedHeight(80);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    m_content->setWordWrap(true);

    const qreal ratio = devicePixelRatioF();
    QIcon icon = QIcon::fromTheme("system-updated", QIcon::fromTheme("application-default-icon"));
    m_icon->setFixedSize(48, 48);

    QPixmap pixmap = icon.pixmap(m_icon->width() * ratio, m_icon->height() * ratio);
    pixmap = pixmap.scaled(m_icon->width() * ratio, m_icon->height() * ratio,
                           Qt::KeepAspectRatioByExpanding,
                           Qt::SmoothTransformation);
    pixmap.setDevicePixelRatio(ratio);

    m_icon->setPixmap(pixmap);

    m_closeButton->setFixedSize(14, 14);

    m_closeButton->setObjectName("NotifyCloseButton");
    m_closeButton->setVisible(false);

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setSpacing(0);
    contentLayout->setMargin(0);

    contentLayout->addStretch();
    contentLayout->addWidget(m_title);
    contentLayout->addSpacing(4);
    contentLayout->addWidget(m_content);
    contentLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    mainLayout->addSpacing(40);
    mainLayout->addWidget(m_icon);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(contentLayout);
    mainLayout->addSpacing(40);
    mainLayout->addStretch();

    setLayout(mainLayout);

    m_updaterInter->setSync(false);
    updatablePkgsChanged(m_updaterInter->updatablePackages());
    connect(m_updaterInter, &com::deepin::lastore::Updater::UpdatablePackagesChanged,
            this, &UpdateNotifier::updatablePkgsChanged);

    connect(m_closeButton, &DImageButton::clicked, this, &UpdateNotifier::ignoreUpdates);
}

void UpdateNotifier::mouseReleaseEvent(QMouseEvent *event)
{
    QFrame::mouseReleaseEvent(event);

    ignoreUpdates();

    emit clicked();
}

void UpdateNotifier::enterEvent(QEvent *event)
{
    QFrame::enterEvent(event);

    m_closeButton->move(x() + width() - 14 - 10, 10);
    m_closeButton->setVisible(true);
}

void UpdateNotifier::leaveEvent(QEvent *event)
{
    QFrame::enterEvent(event);

    m_closeButton->setVisible(false);
}

void UpdateNotifier::ignoreUpdates()
{
    setVisible(false);
    emit notifierVisibleChanged(false);
    m_settings->setValue(IgnoredPkgsKey, m_updatablePkgs);
}

void UpdateNotifier::updatablePkgsChanged(const QStringList &value)
{
    if (value.length() == 0) {
        setVisible(false);
        return;
    }
    m_updatablePkgs = value;

    QStringList pkgs = m_settings->value(IgnoredPkgsKey).toStringList();
    const bool visible = !comparePkgLists(pkgs, value);
    setVisible(visible);
    emit notifierVisibleChanged(visible);

    qDebug() << Q_FUNC_INFO << "setVisible: " << visible;
}

bool UpdateNotifier::comparePkgLists(QStringList val1, QStringList val2)
{
    if (val1.length() != val2.length()) return false;

    qSort(val1.begin(), val1.end());
    qSort(val2.begin(), val2.end());

    return val1 == val2;
}
