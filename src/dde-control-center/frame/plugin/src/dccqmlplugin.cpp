// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccqmlplugin.h"

#include "dccapp.h"
#include "dccmodel.h"
#include "dccobject.h"
#include "dccquickdbusinterface.h"

#include <qqml.h>

namespace dccV25 {

void DccQmlPlugin::registerTypes(const char *uri)
{
    // @uri org.deepin.dcc
    qmlRegisterModule(uri, 1, 0);
    qmlRegisterType<dccV25::DccObject>(uri, 1, 0, "DccObject");
    qmlRegisterType<dccV25::DccModel>(uri, 1, 0, "DccModel");
    qmlRegisterType<dccV25::DccQuickDBusInterface>(uri, 1, 0, "DccDBusInterface");

    qmlRegisterSingletonInstance(uri, 1, 0, "DccApp", dccV25::DccApp::instance());
}

void DccQmlPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

} // namespace dccV25
