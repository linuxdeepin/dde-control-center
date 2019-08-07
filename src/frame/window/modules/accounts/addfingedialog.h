/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "window/namespace.h"
#include "modules/accounts/fingermodel.h"
#include "modules/accounts/widgets/fingerwidget.h"

#include <QDialog>

QT_BEGIN_NAMESPACE

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;

QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

//添加指纹对话框
class AddFingeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddFingeDialog(const QString &thumb, QDialog *parent = nullptr);
    void setFingerModel(dcc::accounts::FingerModel *model);
    void setUsername(const QString &name);

private:
    void initWidget();
    void initData();

Q_SIGNALS:
    void requestSaveThumb(const QString &name);
    void requestReEnrollStart(const QString &thumb);

public Q_SLOTS:
    void saveThumb();
    void onEnrollStatusChanged(dcc::accounts::FingerModel::EnrollStatus status);
    void onViewPlayEnd();

private:
    dcc::accounts::FingerModel *m_model;
    QVBoxLayout *m_mainContentLayout;
    QHBoxLayout *m_cancleaddLayout;
    dcc::accounts::FingerWidget *m_fingeWidget;
    QPushButton *m_cancleBtn;
    QPushButton *m_addBtn;
    QString m_thumb;
    QString m_username;
};

}
}
