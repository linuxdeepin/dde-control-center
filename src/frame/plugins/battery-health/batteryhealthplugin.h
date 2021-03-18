/*
 * Copyright (C) 2020 Deepin Technology Co., Ltd.
 *
 * Author:     xiaoyaobing <xiaoyaobing@uniontech.com>
 *
 * Maintainer: xiaoyaobing <xiaoyaobing@uniontech.com>
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

#ifndef BATTERYHEALTHPLUGIN_H
#define BATTERYHEALTHPLUGIN_H

#include "batteryhealthwidget.h"

#include <interface/namespace.h>
#include <interface/moduleinterface.h>
#include <interface/frameproxyinterface.h>

#include <QObject>
#include <QWidget>

using namespace DCC_NAMESPACE;

class BatteryHealthPlugin : public QObject , public ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ModuleInterface_iid)
    Q_INTERFACES(DCC_NAMESPACE::ModuleInterface)
public:
    explicit BatteryHealthPlugin();
    ~BatteryHealthPlugin() override;

    QIcon icon() const override;

    virtual void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;

    virtual int load(const QString &path) override;
    QStringList availPage() const override;

    /**
     * @brief translationPath 返回系统语言翻译文件路径
     * @return 翻译文件路径
     */
    QString translationPath() const override;
    void deactive() override;
    void contentPopped(QWidget *const w) override;
    void reset() override;

public Q_SLOTS:
    virtual void active() override;

private:
    QTranslator *m_translator{nullptr};
    BatteryHealthwidget *m_batteryHealthwidget{nullptr};
};

#endif // BATTERYHEALTHPLUGIN_H
