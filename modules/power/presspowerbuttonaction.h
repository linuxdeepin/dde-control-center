/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef PRESSPOWERBUTTONACTION
#define PRESSPOWERBUTTONACTION

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFrame>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dbuttongrid.h>
#include <libdui/dheaderline.h>
#include <libdui/dbaseexpand.h>

DUI_USE_NAMESPACE
class PressPowerButtonAction: public QFrame
{
    Q_OBJECT
public:
    PressPowerButtonAction(QStringList powerReaction, QWidget *parent = 0);
    ~PressPowerButtonAction();

signals:
    void powerButtonAction(QString action);
    void powerButtonIndexChanged(int index);
public slots:
    void initConnection();
    void setPowerButtonAction(int buttonId);
    void setTitle(QString titleName);
private:

    DHeaderLine *m_pressPowerReActionLine;
    DBaseExpand *m_pressPowerReActionExpand;
    DButtonGrid *m_powerActionButtonGrid;
    DSeparatorHorizontal *m_pressPowerSeparatorLine;
    QVBoxLayout *pressPowerButtonActionLayout;

};
#endif // PRESSPOWERBUTTONACTION

