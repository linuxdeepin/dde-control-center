/**
 * Copyright (c) 2011 ~ 2013 Deepin, Inc.
 *               2011 ~ 2013 jouyouyun
 *
 * Author:      jouyouyun <jouyouwen717@gmail.com>
 * Maintainer:  jouyouyun <jouyouwen717@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 **/

#include <glib.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/extensions/record.h>

#include "mouse_record.h"
#include "_cgo_export.h"

#define MOUSE_AREA_DEST "com.deepin.dde.api.MouseArea"
#define MOUSE_AREA_PATH "/com/deepin/dde/api/MouseArea"
#define MOUSE_AREA_IFC  "com.deepin.dde.api.MouseArea"

typedef struct _RecordEventInfo {
    Display *ctrl_disp;
    Display *data_disp;
    XRecordRange *range;
    XRecordContext context;
} RecordEventInfo;

typedef union {
    unsigned char type;
    xEvent xe;
    /*xResourceReq req;*/
    /*xGenericReply reply;*/
    /*xError error;*/
    /*xConnSetupPrefix setup;*/
} RecordDate;

static void record_event_cb (XPointer user_data, XRecordInterceptData *hook);
static gpointer enable_ctx_thread (gpointer user_data);

static RecordEventInfo *grab_info = NULL;
static gboolean user_activity_flag = FALSE;

void
record_init ()
{
    grab_info = g_new0 (RecordEventInfo, 1);

    if ( !grab_info ) {
        g_warning ("Alloc RecordEventInfo memory failed...");
        record_finalize ();
    }

    grab_info->ctrl_disp = XOpenDisplay (NULL);
    grab_info->data_disp = XOpenDisplay (NULL);

    if ( !grab_info->ctrl_disp || !grab_info->data_disp ) {
        g_warning ("Unable to connect to X server...");
        record_finalize ();
    }

    gint dummy;

    if ( !XQueryExtension (grab_info->ctrl_disp, "XTEST",
                           &dummy, &dummy, &dummy) ) {
        g_warning ("XTest extension missing...");
        record_finalize ();
    }

    if ( !XRecordQueryVersion (grab_info->ctrl_disp, &dummy, &dummy) ) {
        g_warning ("Failed to obtain xrecord version...");
        record_finalize ();
    }

    grab_info->range = XRecordAllocRange ();

    if ( !grab_info->range ) {
        g_warning ("Alloc XRecordRange memory failed...");
        record_finalize ();
    }

    grab_info->range->device_events.first = KeyPress;
    grab_info->range->device_events.last = MotionNotify;

    XRecordClientSpec spec = XRecordAllClients;
    grab_info->context = XRecordCreateContext (
                             grab_info->data_disp, 0, &spec, 1, &grab_info->range, 1);

    if ( !grab_info->context ) {
        g_warning ("Unable to create context...");
        record_finalize();
    }

    XSynchronize (grab_info->ctrl_disp, TRUE);
    XFlush (grab_info->ctrl_disp);

    GThread *thrd = g_thread_new ("enable context",
                                  (GThreadFunc)enable_ctx_thread, NULL);

    if ( !thrd ) {
        g_warning ("Unable to create thread...");
        record_finalize ();
    }
}

void
record_finalize ()
{
    if (!grab_info) {
        return;
    }

    if (grab_info->context) {
        XRecordDisableContext(grab_info->data_disp, grab_info->context);
        XRecordFreeContext(grab_info->data_disp, grab_info->context);
    }

    if (grab_info->range) {
        XFree(grab_info->range);
        grab_info->range = NULL;
    }

    if (grab_info->ctrl_disp) {
        XCloseDisplay (grab_info->ctrl_disp);
        grab_info->ctrl_disp = NULL;
    }

    if (grab_info->data_disp) {
        XCloseDisplay (grab_info->data_disp);
        grab_info->data_disp = NULL;
    }

    if (grab_info) {
        g_free (grab_info);
        grab_info = NULL;
    }
}

static gpointer
enable_ctx_thread (gpointer user_data)
{
    if ( !XRecordEnableContext (grab_info->data_disp, grab_info->context,
                                record_event_cb, NULL) ) {
        g_warning ("Unable to enable context...");
        record_finalize ();
    }

    g_thread_exit (NULL);

    return NULL;
}

static void
record_event_cb (XPointer user_data, XRecordInterceptData *hook)
{
    if ( hook->category != XRecordFromServer ) {
        XRecordFreeData(hook);
        g_warning ("Data not from X server...");
        return;
    }

    RecordDate *data = (RecordDate *)hook->data;
    int event_type = data->type;
    int rootX = data->xe.u.keyButtonPointer.rootX;
    int rootY = data->xe.u.keyButtonPointer.rootY;
    /*int time = hook->server_time;*/

    switch (event_type) {
        case KeyPress:
            parseKeyboardEvent(KEY_PRESS, rootX, rootY);
            break;

        case KeyRelease:
            parseKeyboardEvent(KEY_RELEASE, rootX, rootY);
            break;

        case MotionNotify:
            parseMotionEvent(rootX, rootY);
            break;

        case ButtonPress:
            parseButtonEvent(BUTTON_PRESS, rootX, rootY);
            break;

        case ButtonRelease:
            parseButtonEvent(BUTTON_RELEASE, rootX, rootY);
            break;

        default:
            break;
    }

    XRecordFreeData(hook);
}

void
simulate_user_activity(int flag)
{
    if (flag == 1 ) {
        user_activity_flag = TRUE;
    } else {
        user_activity_flag = FALSE;
    }
}
