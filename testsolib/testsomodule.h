/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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

#include <interface/namespace.h>
#include <interface/moduleinterface.h>

#include <QObject>

class TestsoModule : public QObject, public DCC_NAMESPACE::ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ModuleInterface_iid)
    Q_INTERFACES(DCC_NAMESPACE::ModuleInterface)
public:
    explicit TestsoModule() = default;
    explicit TestsoModule(DCC_NAMESPACE::FrameProxyInterface *frame, QObject *parent = nullptr);
    void preInitialize(bool sync = false) override;
    void initialize() override;
    void reset() override;
    void active() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    void contentPopped(QWidget *const w) override;
    int load(QString path) override;
    QStringList availPage() const override;

public Q_SLOTS:
    void showGeneralSetting();
};
