//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class VersionProtocolWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VersionProtocolWidget(QWidget *parent = nullptr);
    void setLicense(const QPair<QString, QString> &license);

Q_SIGNALS:
    void loadTextFinished();

private:
    QVBoxLayout *m_mainLayout;
    QLabel *m_title;
    QLabel *m_body;
};

}
