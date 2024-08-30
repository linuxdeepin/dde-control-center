//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>

class QWidget;
class QString;

namespace dccV20 {
class ModuleInterface;
class FrameProxyInterface
{
public:
    enum PushType {
        Replace,
        CoverTop,
        Normal,
        DirectTop,
        Count
    };

public:
    // Module request to into next page
    virtual void pushWidget(ModuleInterface *const inter, QWidget *const w, PushType type = Normal) = 0;
    virtual void popWidget(ModuleInterface *const inter) = 0;
    virtual void setModuleVisible(ModuleInterface *const inter, const bool visible) = 0;
    virtual void showModulePage(const QString &module, const QString &page, bool animation) = 0;
    virtual void setModuleSubscriptVisible(const QString &module, bool bIsDisplay) = 0;

    Q_DECL_DEPRECATED virtual void setRemoveableDeviceStatus(QString type, bool state) = 0;
    Q_DECL_DEPRECATED virtual bool getRemoveableDeviceStatus(QString type) const = 0;

    virtual void setSearchPath(ModuleInterface *const inter) const = 0;
    virtual void addChildPageTrans(const QString &menu, const QString &rran) = 0;

    virtual void setModuleVisible(const QString &module, bool visible) = 0;
    virtual void setWidgetVisible(const QString &module, const QString &widget, bool visible) = 0;
    virtual void setDetailVisible(const QString &module, const QString &widget, const QString &detail, bool visible) = 0;
    virtual void updateSearchData(const QString &module) = 0;
    virtual QString moduleDisplayName(const QString &module) const = 0;
public:
    ModuleInterface *currModule() const { return m_currModule; }

protected:
    void setCurrModule(ModuleInterface *const m) { m_currModule = m; }

private:
    ModuleInterface *m_currModule{nullptr};
};
}
