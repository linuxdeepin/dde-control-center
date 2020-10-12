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

#ifndef SPINBOXWIDGET_H
#define SPINBOXWIDGET_H

#include "widgets/settingsitem.h"

#include <DIconButton>
#include <DSpinBox>

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
class QSpinBox;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {

class SpinBoxWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit SpinBoxWidget(QWidget *parent = nullptr);

    inline DSpinBox *spinBox() const { return m_spinBox; }

    void setTitle(const QString &title);
    void setDefaultVal(int defaultVal);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    int m_defaultVal;
    QLabel *m_title;
    DSpinBox *m_spinBox;
    DTK_WIDGET_NAMESPACE::DIconButton *m_resetBtn;
};

}

}

#endif // SPINBOXWIDGET_H
