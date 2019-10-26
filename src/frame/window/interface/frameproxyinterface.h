#pragma once

#include "window/namespace.h"

class QWidget;
class QString;

namespace DCC_NAMESPACE {
class ModuleInterface;
class FrameProxyInterface
{
public:
    enum PushType {
        Replace,
        CoverTop,
        Normal,
        Count
    };

public:
    // Module request to into next page
    virtual void pushWidget(ModuleInterface *const inter, QWidget *const w, PushType type = Normal) = 0;

    virtual void popWidget(ModuleInterface *const inter) = 0;

    virtual void setModuleVisible(ModuleInterface *const inter, const bool visible) = 0;

    virtual void showModulePage(const QString &module, const QString &page, bool animation) = 0;

public:
    ModuleInterface *currModule() const { return m_currModule; }

protected:
    void setCurrModule(ModuleInterface *const m) { m_currModule = m; }

private:
    ModuleInterface *m_currModule{nullptr};
};
}
