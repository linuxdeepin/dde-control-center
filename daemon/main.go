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

package main

// #cgo CFLAGS: -g -Wall
// #cgo pkg-config: x11 xtst glib-2.0
// #include "mouse_record.h"
import "C"

import (
    "dbus/com/deepin/daemon/display"
    "dbus/com/deepin/dde/controlcenter"
    "dlib/dbus"
    dlogger "dlib/logger"
    "os"
    "sync"
)

var dbusDss *controlcenter.ControlCenter
var ddisplay *display.Display

const EmitAreaWidth = 10

var logger = dlogger.NewLogger("dde-api/mousearea")

type coordinateInfo struct {
    areas    []coordinateRange
    moveFlag int32
}

var (
    opMouse    *Manager
    lock       sync.Mutex
    idRangeMap = make(map[int32]*coordinateInfo)

    genID = func() func() int32 {
        id := int32(0)
        return func() int32 {
            lock.Lock()
            tmp := id
            id += 1
            lock.Unlock()
            return tmp
        }
    }()

    rightBottomCornerId int32
    primaryRect         []interface{}
)

func (op *Manager) RegisterArea(area []coordinateRange) int32 {
    cookie := genID()
    idRangeMap[cookie] = &coordinateInfo{areas: area, moveFlag: -1}

    return cookie
}

func (op *Manager) UnregisterArea(cookie int32) {
    delete(idRangeMap, cookie)
}

func NewManager() *Manager {
    return &Manager{}
}

func stringInSlice(a string, list []string) bool {
    for _, b := range list {
        if b == a {
            return true
        }
    }
    return false
}

func initDssDbus() {
    var err error
    dbusDss, err = controlcenter.NewControlCenter("com.deepin.dde.ControlCenter", "/com/deepin/dde/ControlCenter")
    if err != nil {
        logger.Error("Start dss Failed:", err)
        panic(err)
    }
}

func initDDisplay() {
    var err error
    ddisplay, err = display.NewDisplay("com.deepin.daemon.Display", "/com/deepin/daemon/Display")
    if err != nil {
        logger.Error("display Failed:", err)
        panic(err)
    }
    primaryRect = ddisplay.PrimaryRect.Get()
}

func resetEmitRectangle(primaryRect []interface{}) {
    cancleAllReigsterArea()
    screenWidth := int32(primaryRect[2].(uint16))
    screenHeight := int32(primaryRect[3].(uint16))
    logger.Debug("screenWidth = %v", screenWidth)
    logger.Debug("screenHeight = %v", screenHeight)
    tmp := coordinateRange{X1: screenWidth - EmitAreaWidth, X2: screenWidth, Y1: screenHeight - EmitAreaWidth, Y2: screenHeight}
    rightBottomCornerId = opMouse.RegisterArea([]coordinateRange{tmp})
}

func main() {
    defer func() {
        if err := recover(); err != nil {
            logger.Error("recover error:", err)
        }
    }()

    // configure logger
    logger.SetRestartCommand("/usr/lib/go/bin/dss-daemon", "--debug")
    if stringInSlice("-d", os.Args) || stringInSlice("--debug", os.Args) {
        logger.SetLogLevel(dlogger.LEVEL_DEBUG)
    }

    initDssDbus()
    initDDisplay()
    resetEmitRectangle(primaryRect)

    ddisplay.ConnectPrimaryChanged(resetEmitRectangle)
    opMouse = NewManager()
    err := dbus.InstallOnSession(opMouse)
    if err != nil {
        logger.Error("Install DBus Session Failed:", err)
        panic(err)
    }

    dbus.DealWithUnhandledMessage()
    C.record_init()
    defer C.record_finalize()

    //select {}
    if err = dbus.Wait(); err != nil {
        logger.Error("lost dbus session:", err)
        os.Exit(1)
    } else {
        os.Exit(0)
    }
}
