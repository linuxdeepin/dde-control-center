// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCC_WIDGETS_TITLEDSLIDERITEM_H
#define DCC_WIDGETS_TITLEDSLIDERITEM_H

#include "widgets/settingsitem.h"

#include <DCheckBox>

class QLabel;
class QSlider;
class QHBoxLayout;

namespace dcc {
namespace widgets {

class NormalLabel;
class DCCSlider;

class TitledSliderItem : public SettingsItem
{
    Q_OBJECT
public:
    TitledSliderItem(QString title = QString(), QWidget *parent = nullptr);

    DCCSlider *slider() const;
    bool isSliderPressed() const;
    void setAnnotations(const QStringList &annotations);

    QString valueLiteral() const;
    void setValueLiteral(const QString &valueLiteral);

    void setLeftIcon(const QIcon &leftIcon);
    void setRightIcon(const QIcon &rightIcon);
    void setIconSize(const QSize &size);
    QHBoxLayout *getbottomlayout() { return m_bottomLayout; }
    void setAutoBrightnessVisible(const bool visible);
    void setAutoBrightnessChecked(const bool checked);

Q_SIGNALS:
    void notifyCheckStateChanged(bool);

private:
    NormalLabel *m_titleLabel;
    NormalLabel *m_valueLabel;
    DCCSlider *m_slider;
    QString m_valueLiteral;
    QHBoxLayout *m_bottomLayout;
    bool    m_sliderPressed;
    DTK_WIDGET_NAMESPACE::DCheckBox *m_autoBrightness;
};

} // namespace widgets
} // namespace dcc

#endif // DCC_WIDGETS_TITLEDSLIDERITEM_H
