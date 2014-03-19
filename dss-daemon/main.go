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
	"dlib/dbus"
	dlogger "dlib/logger"
	"os"
	"sync"
)

var logger = dlogger.NewLogger("dde-api/mousearea")

type Manager struct {
	ButtonCoordinate   func(string, int32, int32, int32)
	KeyboardCoordinate func(string, int32, int32, int32)
	CancleAllArea      func(int32, int32, int32)
}

func (m *Manager) GetDBusInfo() dbus.DBusInfo {
	return dbus.DBusInfo{
		"com.deepin.dss.daemon",
		"/com/deepin/dss/daemon",
		"com.deepin.dss.daemon",
	}
}

type coordinateInfo struct {
	areas    []coordinateRange
	moveFlag int32
}

var (
	opMouse    *Manager
	lock       sync.Mutex
	idRangeMap map[int32]*coordinateInfo

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

func main() {
	defer func() {
		if err := recover(); err != nil {
			logger.Error("recover error:", err)
		}
	}()

	// configure logger
	logger.SetRestartCommand("/usr/lib/deepin-api/mousearea", "--debug")
	if stringInSlice("-d", os.Args) || stringInSlice("--debug", os.Args) {
		logger.SetLogLevel(dlogger.LEVEL_DEBUG)
	}

	idRangeMap = make(map[int32]*coordinateInfo)
	opMouse = NewManager()
	err := dbus.InstallOnSession(opMouse)
	if err != nil {
		logger.Error("Install DBus Session Failed:", err)
		panic(err)
	}

	dbus.DealWithUnhandledMessage()
	cancleAllReigsterArea()
	tmp := coordinateRange{X1: 1336, X2: 1366, Y1: 738, Y2: 768}
	opMouse.RegisterArea([]coordinateRange{tmp})
	tmp2 := coordinateRange{X1: 0, X2: 1006, Y1: 0, Y2: 768}
    opMouse.RegisterArea([]coordinateRange{tmp2})
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
