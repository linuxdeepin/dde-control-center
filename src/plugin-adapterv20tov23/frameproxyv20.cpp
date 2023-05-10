//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "frameproxyv20.h"
#include "moduleinterface.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>

using namespace dccV20;

FrameProxyV20::FrameProxyV20(QObject *parent)
    : QObject(parent)
    , m_rootModule(nullptr)
    , m_topWidget(nullptr)
{
}

void FrameProxyV20::setRootModule(DCC_NAMESPACE::ModuleObject *rootModule)
{
    m_rootModule = rootModule;
}

void FrameProxyV20::append(AdapterV20toV23Module *module)
{
    m_moduleMap.insert(module->inter(), module);
    connect(module,&AdapterV20toV23Module::actived,this,&FrameProxyV20::popAllWidgets);
}

void FrameProxyV20::pushWidget(ModuleInterface *const inter, QWidget *const w, dccV20::FrameProxyInterface::PushType type)
{
    if (!m_moduleMap.contains(inter))
        return;

    AdapterV20toV23Module *module = m_moduleMap.value(inter);
    switch (type) {
    case Replace:   // 替换三级页面。当在新的三级页面中单击“pop”按钮时，返回到旧的三级页面
    case CoverTop:  //根据当前页面宽度去计算新增的页面放在最后面一层，或者Top页面
    case DirectTop: //不需要管页面宽度，直接将新增页面放在Top页面；为解决某些页面使用CoverTop无法全部示的问题
        if (m_topWidget)
            popWidget(inter);

        module->setChildPage(m_widgets.size(), w);
        m_topWidget = w;
        m_widgets.push(w);
        break;
    case Normal:
    default:
        while (m_widgets.size() > 1)
            popWidget(inter);

        module->setChildPage(m_widgets.size(), w);
        m_widgets.push(w);
        break;
    }
}

void FrameProxyV20::popWidget(ModuleInterface *const inter)
{
    Q_UNUSED(inter)
    if (m_widgets.isEmpty())
        return;

    QWidget *w = m_widgets.pop();
    for (auto &&module : m_moduleMap)
        module->popWidget(w);

    if (m_topWidget == w)
        m_topWidget = nullptr;
}

void FrameProxyV20::setModuleVisible(ModuleInterface *const inter, const bool visible)
{
    if (m_moduleMap.contains(inter))
        m_moduleMap.value(inter)->setHidden(!visible);
}

void FrameProxyV20::showModulePage(const QString &module, const QString &page, bool animation)
{
    // DBus 切换页面 module/page
    Q_UNUSED(animation)
    QString arg = module;
    if (!page.isEmpty())
        arg += "/" + page;
    QDBusMessage message = QDBusMessage::createMethodCall("com.deepin.dde.ControlCenter", "/com/deepin/dde/ControlCenter", "com.deepin.dde.ControlCenter", "ShowPage");
    message << arg;
    QDBusConnection::sessionBus().asyncCall(message);
}

void FrameProxyV20::setModuleSubscriptVisible(const QString &module, bool bIsDisplay)
{
    Q_UNUSED(module);
    Q_UNUSED(bIsDisplay);
}
// 该函数已废弃
void FrameProxyV20::setRemoveableDeviceStatus(QString type, bool state)
{
    Q_UNUSED(type);
    Q_UNUSED(state);
}
// 该函数已废弃
bool FrameProxyV20::getRemoveableDeviceStatus(QString type) const
{
    Q_UNUSED(type);
    return false;
}

void FrameProxyV20::setSearchPath(ModuleInterface *const inter) const
{
    Q_UNUSED(inter)
}

void FrameProxyV20::addChildPageTrans(const QString &menu, const QString &rran)
{
    Q_UNUSED(menu)
    Q_UNUSED(rran)
}

void FrameProxyV20::setModuleVisible(const QString &module, bool visible)
{
    auto find_it = std::find_if(m_moduleMap.cbegin(), m_moduleMap.cend(), [module](auto &it) {
        return it->displayName() == module;
    });
    if (find_it != m_moduleMap.cend())
        find_it.value()->setHidden(!visible);
}

void FrameProxyV20::setWidgetVisible(const QString &module, const QString &widget, bool visible)
{
    auto find_it = std::find_if(m_moduleMap.cbegin(), m_moduleMap.cend(), [module](auto &it) {
        return it->displayName() == module;
    });
    if (find_it == m_moduleMap.cend())
        return;

    QStringList content = find_it.value()->contentText();
    if (visible) {
        if (content.contains(widget))
            content.append(widget);
    } else {
        content.removeAll(widget);
    }
    find_it.value()->setContentText(content);
}

void FrameProxyV20::setDetailVisible(const QString &module, const QString &widget, const QString &detail, bool visible)
{
    auto find_it = std::find_if(m_moduleMap.cbegin(), m_moduleMap.cend(), [module, widget](auto &it) {
        return it->displayName() == widget || it->displayName() == module;
    });
    if (find_it == m_moduleMap.cend())
        return;

    if (visible)
        find_it.value()->addContentText(detail);
    else {
        QStringList content = find_it.value()->contentText();
        content.removeAll(detail);
        find_it.value()->setContentText(content);
    }
}

void FrameProxyV20::updateSearchData(const QString &module)
{
    Q_UNUSED(module);
}

QString FrameProxyV20::moduleDisplayName(const QString &module) const
{
    auto find_it = std::find_if(m_moduleMap.cbegin(), m_moduleMap.cend(), [module](auto &it) {
        return it->name() == module;
    });
    if (find_it == m_moduleMap.cend()) {
        qDebug() << "Not found module:" << module;
        return QString();
    }
    return find_it.key()->displayName();
}

void FrameProxyV20::popAllWidgets()
{
    m_topWidget = nullptr;
    m_widgets.clear();
}
