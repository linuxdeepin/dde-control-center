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
    , m_animationMode(AnimationPop)
{
    Q_ASSERT(!dccApp);
    dccApp = this;
}

DccApp::~DccApp() { }

void DccApp::setAnimationMode(AnimationMode mode)
{
    if (m_animationMode != mode) {
        m_animationMode = mode;
        Q_EMIT animationModeChanged(mode);
    }
}

int DccApp::width() const
{
    return 0;
}

int DccApp::height() const
{
    return 0;
}

DccObject *DccApp::root() const
{
    return nullptr;
}

DccObject *DccApp::activeObject() const
{
    return nullptr;
}

DccObject *DccApp::object(const QString &)
{
    return nullptr;
}

void DccApp::addObject(DccObject *) { }

void DccApp::removeObject(DccObject *) { }

void DccApp::removeObject(const QString &) { }

void DccApp::showPage(const QString &) { }

void DccApp::showPage(DccObject *, const QString &) { }

QWindow *DccApp::mainWindow() const
{
    return nullptr;
}

QAbstractItemModel *DccApp::navModel() const
{
    return nullptr;
}

QSortFilterProxyModel *DccApp::searchModel() const
{
    return nullptr;
}

} // namespace dccV25
