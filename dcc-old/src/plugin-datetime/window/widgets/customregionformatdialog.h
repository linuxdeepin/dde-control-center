//SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CUSTOMREGIONFORMATDIALOG_H
#define CUSTOMREGIONFORMATDIALOG_H

#include "interface/namespace.h"
#include "regionproxy.h"

#include <DDialog>

#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

class CustomRegionFormatDialog : public DDialog
{
    Q_OBJECT
public:
    explicit CustomRegionFormatDialog(QWidget *parent = nullptr);
    ~CustomRegionFormatDialog();

    void initRegionFormat(const QLocale &locale,  const RegionFormat &regionFormat);

Q_SIGNALS:
    void customFormatSaved(const RegionFormat &regionFormat);

private Q_SLOTS:
    void onSaved();

private:
    QComboBox *m_dayCombo;
    QComboBox *m_shortDateCombo;
    QComboBox *m_longDateCombo;
    QComboBox *m_shortTimeCombo;
    QComboBox *m_longTimeCombo;
    QLabel *m_currencyValueLabel;
    QLabel *m_numberValueLabel;
    QLabel *m_paperValueLabel;

    RegionFormat m_format;
};
}
#endif // CUSTOMREGIONFORMATDIALOG_H
