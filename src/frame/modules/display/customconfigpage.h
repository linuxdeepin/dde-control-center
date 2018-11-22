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

#ifndef CUSTOMCONFIGPAGE_H
#define CUSTOMCONFIGPAGE_H

#include "widgets/contentwidget.h"

#include <QPushButton>
#include <QLabel>

namespace dcc {

namespace display {

class CustomConfigPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit CustomConfigPage(const QString &config, QWidget *parent = 0);

Q_SIGNALS:
    void requestModifyConfig(const QString &config) const;
    void requestDeleteConfig(const QString &config) const;

public Q_SLOTS:
    void onCurrentConfigChanged(const bool custom, const QString config);

private Q_SLOTS:
    void onDeleteBtnClicked();
    void onModifyBtnClicked();

private:
    const QString m_configName;

    QPushButton *m_modifyBtn;
    QPushButton *m_deleteBtn;
    QLabel *m_modifyTips;
    QLabel *m_deleteTips;
};

}

}

#endif // CUSTOMCONFIGPAGE_H
