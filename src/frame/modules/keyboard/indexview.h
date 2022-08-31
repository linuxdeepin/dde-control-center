// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef INDEXVIEW_H
#define INDEXVIEW_H

#include <DListView>
#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace keyboard{
class IndexView : public DListView
{
    Q_OBJECT

public:
    explicit IndexView(QWidget* parent = 0);

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
}
#endif // INDEXVIEW_H
