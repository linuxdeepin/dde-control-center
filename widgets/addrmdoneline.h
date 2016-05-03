/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ADDRMDONELINE_H
#define ADDRMDONELINE_H

#include <dbaseline.h>
#include <dwidget_global.h>
#include <dtextbutton.h>

#include "dbreathinglabel.h"

DWIDGET_USE_NAMESPACE

class ImageNameButton;
class DynamicLabel;
class AddRmDoneLine : public DBaseLine
{
    Q_OBJECT

    Q_PROPERTY(QString rmButtonToolTip READ rmButtonToolTip WRITE setRmButtonToolTip)
    Q_PROPERTY(QString addButtonToolTip READ addButtonToolTip WRITE setAddButtonToolTip)
    Q_PROPERTY(QString doneButtonToolTip READ doneButtonToolTip WRITE setDoneButtonToolTip)
public:
    explicit AddRmDoneLine(QWidget *parent = 0);
    void setTitle(const QString &title);

    ImageNameButton *removeButton() const;
    ImageNameButton *addButton() const;
    DTextButton *doneButton() const;
    QString rmButtonToolTip() const;
    QString addButtonToolTip() const;
    QString doneButtonToolTip() const;

public slots:
    void setRmButtonToolTip(QString rmButtonToolTip);
    void setAddButtonToolTip(QString addButtonToolTip);
    void setDoneButtonToolTip(QString doneButtonToolTip);

private slots:
    void onButtonStateChanged();

signals:
    void addClicked();
    void removeClicked();
    void doneClicked();
    void setAddHidden(bool hidden);
    void setRemoveHidden(bool hidden);
    void setDoneHidden(bool hidden);

private:
    ImageNameButton *m_removeButton;
    ImageNameButton *m_addButton;
    DTextButton *m_doneButton;
    QString m_rmButtonToolTip;
    QString m_addButtonToolTip;
    QString m_doneButtonToolTip;
    DBreathingLabel *m_leftLabel;
    DynamicLabel *m_dynamicLabel;
};

#endif // ADDRMDONELINE_H
