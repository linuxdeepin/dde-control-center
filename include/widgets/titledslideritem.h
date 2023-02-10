/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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
#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

class QLabel;
class QSlider;
class QHBoxLayout;

namespace DCC_NAMESPACE {

class DCCSlider;

class TitledSliderItem : public SettingsItem
{
    Q_OBJECT
public:
    TitledSliderItem(QString title = QString(), QWidget *parent = nullptr);

    DCCSlider *slider() const;
    void setAnnotations(const QStringList &annotations);

    QString valueLiteral() const;
    void setValueLiteral(const QString &valueLiteral);

    QString title() const;
    void setTitle(const QString &title);

    void setLeftIcon(const QIcon &leftIcon);
    void setRightIcon(const QIcon &rightIcon);
    void setIconSize(const QSize &size);

    QHBoxLayout *getbottomlayout() { return m_bottomLayout; }

private:
    QLabel *m_titleLabel;
    QLabel *m_valueLabel;
    DCCSlider *m_slider;
    QString m_valueLiteral;
    QHBoxLayout *m_bottomLayout;
};

} // namespace DCC_NAMESPACE
