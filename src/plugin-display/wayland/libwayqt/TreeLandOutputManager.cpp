// SPDX-FileCopyrightText: 2018 - 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TreeLandOutputManager.hpp"

#include "treeland-output-management-client-protocol.h"

#include <wayland-client.h>

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QThread>

WQt::TreeLandOutputManager::TreeLandOutputManager(treeland_output_manager_v1 *opMgr)
{
    mObj = opMgr;
    treeland_output_manager_v1_add_listener(mObj, &mListener, this);
}

WQt::TreeLandOutputManager::~TreeLandOutputManager()
{
    treeland_output_manager_v1_destroy(mObj);
}

void WQt::TreeLandOutputManager::setPrimaryOutput(const char *name)
{
    treeland_output_manager_v1_set_primary_output(mObj, name);
}

void WQt::TreeLandOutputManager::handlePrimaryOutput(
        void *data,
        struct treeland_output_manager_v1 *treeland_output_manager_v1,
        const char *output_name)
{
    Q_UNUSED(treeland_output_manager_v1)
    WQt::TreeLandOutputManager *manager = reinterpret_cast<WQt::TreeLandOutputManager *>(data);
    manager->mPrimaryOutput = QString::fromLocal8Bit(output_name);
    emit manager->primaryOutputChanged(output_name);
}

const treeland_output_manager_v1_listener WQt::TreeLandOutputManager::mListener = {
    handlePrimaryOutput
};
