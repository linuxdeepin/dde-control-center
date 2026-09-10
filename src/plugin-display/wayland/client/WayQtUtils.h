// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

class QScreen;

struct wl_output;

namespace WQt {
namespace Utils {
/**
 * The wl_output proxy owned by the Qt Wayland platform screen.
 * Returns nullptr for screens which are not backed by a wl_output,
 * e.g. the placeholder screen Qt creates while no output exists.
 */
struct wl_output *wlOutputFromQScreen(QScreen *screen);

/**
 * Whether @a output still backs one of the screens Qt currently knows about.
 *
 * A wl_output proxy stays valid for the client after the compositor removed the
 * global, but every request naming it is answered with a fatal protocol error
 * ("Invalid output resource"). Call before issuing an output-bound request.
 */
bool isOutputAlive(struct wl_output *output);

bool isTreeland();
} // namespace Utils
} // namespace WQt
