/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef FRAMEPROXYV20_H
#define FRAMEPROXYV20_H

#include "adapterv20tov23module.h"
#include "frameproxyinterface.h"

#include <QStack>

class QEvent;
class QTimer;

namespace dccV20 {
class ModuleInterface;
}

class FrameProxyV20 : public QObject, public dccV20::FrameProxyInterface
{
public:
    explicit FrameProxyV20(QObject *parent = nullptr);
    ~FrameProxyV20() = default;

public:
    void setRootModule(DCC_NAMESPACE::ModuleObject *rootModule);
    void append(AdapterV20toV23Module *module);

public:
    // Module request to into next page
    virtual void pushWidget(dccV20::ModuleInterface *const inter,
                            QWidget *const w,
                            dccV20::FrameProxyInterface::PushType type = Normal) override;
    virtual void popWidget(dccV20::ModuleInterface *const inter) override;
    virtual void setModuleVisible(dccV20::ModuleInterface *const inter,
                                  const bool visible) override;
    virtual void showModulePage(const QString &module,
                                const QString &page,
                                bool animation) override;
    virtual void setModuleSubscriptVisible(const QString &module, bool bIsDisplay) override;

    virtual void setRemoveableDeviceStatus(QString type, bool state) override; // Q_DECL_DEPRECATED
    virtual bool getRemoveableDeviceStatus(QString type) const override;       // Q_DECL_DEPRECATED

    virtual void setSearchPath(dccV20::ModuleInterface *const inter) const override;
    virtual void addChildPageTrans(const QString &menu, const QString &rran) override;

    virtual void setModuleVisible(const QString &module, bool visible) override;
    virtual void setWidgetVisible(const QString &module,
                                  const QString &widget,
                                  bool visible) override;
    virtual void setDetailVisible(const QString &module,
                                  const QString &widget,
                                  const QString &detail,
                                  bool visible) override;
    virtual void updateSearchData(const QString &module) override;
    virtual QString moduleDisplayName(const QString &module) const override;

private:
    void popAllWidgets();

private:
    QMap<dccV20::ModuleInterface *, AdapterV20toV23Module *> m_moduleMap;
    QStack<QWidget *> m_widgets;
    DCC_NAMESPACE::ModuleObject *m_rootModule;
    QWidget *m_topWidget;
};

#endif // FRAMEPROXYV20_H
