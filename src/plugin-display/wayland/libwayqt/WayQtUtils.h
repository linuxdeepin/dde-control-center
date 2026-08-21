// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

class QWindow;
class QScreen;

struct wl_display;
struct wl_compositor;
struct wl_touch;
struct wl_keyboard;
struct wl_seat;
struct wl_pointer;

struct wl_output;
struct wl_surface;

namespace WQt {
namespace Wayland {
struct wl_display *display();
struct wl_compositor *compositor();
struct wl_seat *seat();
struct wl_pointer *pointer();
struct wl_keyboard *keyboard();
struct wl_touch *touch();
} // namespace Wayland

namespace Utils {
struct wl_output *wlOutputFromQScreen(QScreen *screen);
struct wl_surface *wlSurfaceFromQWindow(QWindow *window);

void flushDisplay();
void displayRoundtrip();

bool isWayland();
bool isTreeland();
} // namespace Utils
} // namespace WQt
