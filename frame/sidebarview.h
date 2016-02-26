/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SIDEBARVIEW_H
#define SIDEBARVIEW_H

#include <QListView>

#include "dtipsframe.h"
#include "modulemetadata.h"

class SidebarModel;
class SidebarView : public QListView
{
    Q_OBJECT

public:
    SidebarView(QWidget *parent = nullptr);
    ~SidebarView();

    bool firstItemVisible();
    bool lastItemVisible();

    QSize sizeHint() const Q_DECL_OVERRIDE;
    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

public slots:
    void scrollDown();
    void scrollUp();

signals:
    void moduleSelected(const ModuleMetaData &meta) const;
    void scrolled(int pos) const;
    void sizeChanged(const QSize &size) const;

protected:
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;

private:
    DTipsFrame *m_tips;
};

#endif // SIDEBARVIEW_H
