// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef POWERDISPLAYWIDGET_H
#define POWERDISPLAYWIDGET_H

#include "widgets/settingsitem.h"

#include <DSwitchButton>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {

class PowerDisplayWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit PowerDisplayWidget(const QString &title, QWidget *parent = nullptr);
    explicit PowerDisplayWidget(QWidget *parent = nullptr, QLabel *leftwidget = nullptr, QLabel *rightWidget = nullptr);
    void setTitle(const QString &title);
    void setText(const QString &text);

private:
    QLabel *m_leftWidget;
    QLabel *m_rightWidget;
};
}
}

#endif // POWERDISPLAYWIDGET_H

