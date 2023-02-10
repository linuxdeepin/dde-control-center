//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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

}
