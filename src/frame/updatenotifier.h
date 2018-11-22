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

#ifndef UPDATENOTIFIER_H
#define UPDATENOTIFIER_H

#include <QFrame>

#include <dimagebutton.h>

#include "widgets/labels/normallabel.h"
#include "widgets/labels/tipslabel.h"

#include <com_deepin_lastore_updater.h>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;

class QLabel;
class UpdateNotifier : public QFrame
{
    Q_OBJECT
public:
    explicit UpdateNotifier(QWidget *parent = 0);

Q_SIGNALS:
    void clicked();
    void notifierVisibleChanged(const bool visible);

protected:
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    void updateIcon(bool isSuccessd = true);

private:
    QLabel *m_icon;
    NormalLabel *m_title;
    TipsLabel *m_content;
    DImageButton *m_closeButton;

    QSettings *m_settings;

    QStringList m_updatablePkgs;
    com::deepin::lastore::Updater *m_updaterInter;

    void ignoreUpdates();
    void updatablePkgsChanged(const QStringList &value);
    bool comparePkgLists(QStringList val1, QStringList val2);
};

#endif // UPDATENOTIFIER_H
