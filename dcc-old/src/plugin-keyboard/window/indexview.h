//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef INDEXVIEW_H
#define INDEXVIEW_H
#include "interface/namespace.h"
#include <DListView>
#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE{
class IndexView : public DListView
{
    Q_OBJECT

public:
    explicit IndexView(QWidget* parent = nullptr);

public Q_SLOTS:
    void onClick(const QString& ch);

protected:
    void showEvent(QShowEvent* e);
    void scrollContentsBy(int dx, int dy);

private:
    QString m_section;
    QLabel* m_label;
};
}
#endif // INDEXVIEW_H
