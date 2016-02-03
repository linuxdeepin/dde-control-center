/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QObject>
#include <QFrame>
#include <QtPlugin>
#include <QScrollArea>

#include <libdui/dexpandgroup.h>
#include <libdui/dboxwidget.h>

#include "dbus/dbussysteminfo.h"
#include "updatearrowexpand.h"
#include "updatewidget.h"
#include "mirrorscontrolwidget.h"
#include "interfaces.h"

DUI_USE_NAMESPACE

class QFrame;
class QPlainTextEdit;
class ModuleHeader;
class SystemInfo : public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "system_info.json")
    Q_INTERFACES(ModuleInterface)

public:
    SystemInfo();
    ~SystemInfo() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;
    void preUnload() Q_DECL_OVERRIDE;

private:
    QString getLicense(const QString & filePath, const QString &type) const;
    Q_SLOT void updateWidgetHeight();
    bool eventFilter(QObject *, QEvent *e) Q_DECL_OVERRIDE;

private slots:
    void onUpdatableNumsChange(const int apps, const int packages);
    void onProcessFinished();

private:
    QFrame *m_centeralFrame;
    DBusSystemInfo m_dbusSystemInfo;
    QWidget *m_infoWidget;
    QScrollArea *m_licenseArea;
    ModuleHeader *m_baseLine;
    QWidget *m_updateWidget;
//    DVBoxWidget *m_updateWidget;
    UpdateWidget *m_updateInfoWidget;
    MirrorsControlWidget *m_mirrorsControlWidget;
    UpdateArrowExpand *m_updateExpand;

    QList<DArrowLineExpand*> m_extralicenses;
    QSet<QString> m_markActionStarted;

    void scanlicenses();
    void loadSystemInfoFromLocalFile(QGridLayout *infoGrid);
};

#endif // SYSTEMINFO_H
