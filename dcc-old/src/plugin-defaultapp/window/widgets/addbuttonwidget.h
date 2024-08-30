//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ADDBUTTONWIDGET_H
#define ADDBUTTONWIDGET_H

#include "defappworker.h"
#include <QWidget>

#include <DFloatingButton>

QT_BEGIN_NAMESPACE
class QFileDialog;
QT_END_NAMESPACE

class AddButtonWidget : public QWidget
{
    Q_OBJECT
public:
    AddButtonWidget(DefAppWorker::DefaultAppsCategory category = DefAppWorker::Browser, QWidget *parent = nullptr);
    ~AddButtonWidget();

    void setModel(DefAppModel *const model);
    void setCategory(Category *const category);
    void setDefaultAppsCategory(DefAppWorker::DefaultAppsCategory category);

Q_SIGNALS:
    void requestCreateFile(const QString &category, const QFileInfo &info);

public Q_SLOTS:
    void setCategoryName(const QString &name);
    void onAddBtnClicked();

private:
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_addBtn;
    DefAppWorker::DefaultAppsCategory m_categoryValue;
    QString m_categoryName;
    Category *m_category;
};

#endif // ADDBUTTONWIDGET_H
