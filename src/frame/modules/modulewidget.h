// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef MODULEWIDGET_H
#define MODULEWIDGET_H

#include "widgets/labels/largelabel.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

namespace dcc {
namespace widgets {
class LargeLabel;
}
}

namespace dcc {

class ModuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleWidget();
    const QString title() const;
    void setTitle(const QString &title);

Q_SIGNALS:
    void headerClicked() const;

private:
    bool event(QEvent *event);

protected:
    QLabel *m_moduleIcon;
    dcc::widgets::LargeLabel *m_moduleTitle;
    QVBoxLayout *m_centralLayout;
};

}

#endif // MODULEWIDGET_H
