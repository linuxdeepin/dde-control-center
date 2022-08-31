// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef VPNLISTVIEW_H
#define VPNLISTVIEW_H

#include <QListView>
#include <QAbstractItemModel>

namespace dcc {

namespace widgets {

class BasicListView : public QListView
{
    Q_OBJECT

public:
    explicit BasicListView(QWidget *parent = 0);

    QSize sizeHint() const;

public:
    void setModel(QAbstractItemModel *model);
    void onContentHeightChanged();

    void leaveEvent(QEvent *e);

public Q_SLOTS:
    void setAutoFitHeight(const bool fit);

private:
    bool m_autoFitHeight;
};

}

}

#endif // VPNLISTVIEW_H
