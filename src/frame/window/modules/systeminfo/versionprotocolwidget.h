// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class TipsLabel;
}
}

namespace DCC_NAMESPACE {
namespace systeminfo {

class VersionProtocolWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit VersionProtocolWidget(QWidget *parent = nullptr);

Q_SIGNALS:
    void loadTextFinished();

private:
    QVBoxLayout *m_mainLayout;
    dcc::widgets::TipsLabel *m_title;
    dcc::widgets::TipsLabel *m_body;
};

}
}
