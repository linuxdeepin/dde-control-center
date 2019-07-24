#pragma once

#include "window/namespace.h"

class QWidget;
class QString;

namespace DCC_NAMESPACE {
class ModuleInterface;
class FrameProxyInterface
{
public:
    // Module request to into next page
    virtual void pushWidget(ModuleInterface * const inter, QWidget * const w) = 0;

    virtual void setModuleVisible(ModuleInterface * const inter, const bool visible) = 0;

    virtual void showModulePage(const QString &module, const QString &page, bool animation) = 0;
};
}
