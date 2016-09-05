/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GRUBWIDGET_H
#define GRUBWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

#include <darrowlineexpand.h>
#include <dheaderline.h>
#include <dwidget_global.h>
#include <dbuttonlist.h>

#include "grubbackground.h"
#include "dbusgrub.h"
#include "scrollframe.h"

DWIDGET_USE_NAMESPACE

class ModuleHeader;
class BootMenuList;
class GrubWidget : public ScrollFrame
{
    Q_OBJECT

public:
    explicit GrubWidget(QWidget *parent = 0);
    ~GrubWidget();

protected:
    bool eventFilter(QObject *, QEvent *) Q_DECL_OVERRIDE;

private:
    ModuleHeader *m_header;
    DArrowLineExpand *m_arrowDefaultBoot;
    DArrowLineExpand *m_arrowBootDelay;
    DArrowLineExpand *m_arrowTextColor;
    DArrowLineExpand *m_arrowSelectedTextColor;
    GrubThemeDbus *m_themeDbus;
    GrubDbus *m_grubDbus;
    GrubBackground *m_grubBackground;
    DButtonList *m_bootEntryList;
    BootMenuList *m_bootMenuTitle;
    QHBoxLayout *m_selectTimeoutLayout;
    QLabel *m_tooltip;
    QStringList m_timeoutList;

    void init();
    Q_SLOT void setDefaultEntry(const QString &entry);
    Q_SLOT void updatingChanged();
    Q_SLOT void resetTooltip();
};

#endif // GRUBWIDGET_H
