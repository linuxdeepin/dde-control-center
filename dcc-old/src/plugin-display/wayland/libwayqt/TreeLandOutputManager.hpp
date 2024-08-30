// SPDX-FileCopyrightText: 2018 - 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <wayland-client-protocol.h>

#include <QMap>
#include <QObject>
#include <QRect>
#include <QString>

struct wl_buffer;
struct wl_output;
struct treeland_output_manager_v1;
struct treeland_output_manager_v1_listener;

namespace WQt {
class TreeLandOutputManager;
}

class WQt::TreeLandOutputManager : public QObject
{
    Q_OBJECT;

public:
    TreeLandOutputManager(treeland_output_manager_v1 *);
    ~TreeLandOutputManager();

    /** Set the primary output */
    void setPrimaryOutput(const char *);

    QString mPrimaryOutput;

private:
    static void handlePrimaryOutput(void *data,
                                    struct treeland_output_manager_v1 *treeland_output_manager_v1,
                                    const char *output_name);

    static const treeland_output_manager_v1_listener mListener;

    treeland_output_manager_v1 *mObj;

Q_SIGNALS:
    void primaryOutputChanged(const char *);
};
