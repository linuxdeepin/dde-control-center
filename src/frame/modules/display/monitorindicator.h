// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef MONITORFULLWIDGET_H
#define MONITORFULLWIDGET_H

#include <DPlatformWindowHandle>
#include <QFrame>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace display {

class MonitorIndicator : public QFrame
{
    Q_OBJECT

public:
    explicit MonitorIndicator(QWidget *parent = 0);
    ~MonitorIndicator() override;

public Q_SLOTS:

    virtual void setVisible(bool visible);

private:
    void updateGeometry();

private:
    QFrame *m_topLine;
    QFrame *m_bottomLine;
    QFrame *m_leftLine;
    QFrame *m_rightLine;
};

} // namespace display

} // namespace dcc

#endif // MONITORFULLWIDGET_H
