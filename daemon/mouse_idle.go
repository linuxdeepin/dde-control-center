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
import "time"

type IdleTick struct {
	IdleTimeout func(int32, string)
}

type timerInfo struct {
	runFlag   bool
	cookie    int32
	timeout   int32
	name      string
	closeChan chan bool
	timer     *time.Timer
}

var (
	cookieTimerMap map[int32]*timerInfo
	opIdle         = &IdleTick{}
)

//export startAllTimer
func startAllTimer() {
	for cookie, _ := range cookieTimerMap {
		go startTimer(cookie)
	}
}

//export endAllTimer
func endAllTimer(stopFlag bool) {
	for cookie, _ := range cookieTimerMap {
		endTimer(cookie, stopFlag)
	}
}

//export startTimer
func startTimer(cookie int32) {
	info, ok := cookieTimerMap[cookie]
	if !ok {
		logger.Info("Get timer info failed in startTimer.cookie:",
			cookie)
		return
	}

	info.timer = time.NewTimer(time.Second * time.Duration(info.timeout))
	info.runFlag = true

	select {
	case <-info.timer.C:
		logger.Info("Timeout. cookie:", info.cookie)
		opIdle.IdleTimeout(info.cookie, info.name)
		delete(cookieTimerMap, cookie)
		return
	case <-info.closeChan:
		return
	}
}

//export endTimer
func endTimer(cookie int32, stopFlag bool) {
	info, ok := cookieTimerMap[cookie]
	if !ok {
		logger.Info("Get timer info failed in endTimer.cookie:",
			cookie)
		return
	}

	if info.runFlag == false {
		logger.Info("Timer has been end. cookie:", cookie)
		return
	}

	info.timer.Stop()
	info.closeChan <- true
	if stopFlag {
		delete(cookieTimerMap, cookie)
	}
}

func newTimerInfo(name string, cookie, timeout int32) *timerInfo {
	info := &timerInfo{}

	info.runFlag = false
	info.name = name
	info.cookie = cookie
	info.timeout = timeout
	info.closeChan = make(chan bool)

	return info
}
