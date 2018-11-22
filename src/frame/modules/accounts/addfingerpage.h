/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#ifndef ADDFINGERPAGE_H
#define ADDFINGERPAGE_H

#include "widgets/contentwidget.h"
#include "widgets/buttontuple.h"
#include "widgets/fingerwidget.h"
#include "fingermodel.h"

#include <QWidget>

namespace dcc {
namespace accounts {
class AddFingerPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit AddFingerPage(const QString &thumb, QWidget *parent = nullptr);

    void setFingerModel(FingerModel *model);
    void setUsername(const QString &name);

Q_SIGNALS:
    void requestSaveThumb(const QString &name);
    void requestReEnrollStart(const QString &thumb);

private Q_SLOTS:
    void saveThumb();
    void reEnrollStart();
    void onEnrollStatusChanged(FingerModel::EnrollStatus status);
    void onViewPlayEnd();

private:
    FingerModel *m_model;
    widgets::ButtonTuple *m_buttonTuple;
    FingerWidget *m_fingerWidget;
    int m_frequency;
    QString m_thumb;
    QString m_username;
};
}
}

#endif // ADDFINGERPAGE_H
