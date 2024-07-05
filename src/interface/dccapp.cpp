// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccapp.h"

namespace dccV25 {
static DccApp *dccApp = nullptr;

DccApp *DccApp::instance()
{
    return dccApp;
}

DccApp::DccApp(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(!dccApp);
    dccApp = this;
    // connect(parent, &DccManager::pathChanged, this, &DccApp::pathChanged);
    // connect(parent, &DccManager::rootChanged, this, &DccApp::rootChanged);
    // connect(parent, &DccManager::activeObjectChanged, this, &DccApp::activeObjectChanged);
}

DccApp::~DccApp() { }

int DccApp::width() const
{
    return 0;
}

int DccApp::height() const
{
    return 0;
}

QString DccApp::path() const
{
    return QString();
}

DccObject *DccApp::root() const
{
    return nullptr;
}

DccObject *DccApp::activeObject() const
{
    return nullptr;
}

DccObject *DccApp::object(const QString &name)
{
    return nullptr;
}

void DccApp::addObject(DccObject *obj) { }

void DccApp::removeObject(const DccObject *obj) { }

void DccApp::removeObject(const QString &name) { }

void DccApp::showPage(const QString &url) { }

void DccApp::showPage(DccObject *obj, const QString &cmd) { }

QWindow *DccApp::mainWindow()
{
    return nullptr;
}

void DccApp::setShowPath(const QString &path) { }

} // namespace dccV25
