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
import "dbus/com/deepin/dss"

type coordinateRange struct {
	X1 int32
	X2 int32
	Y1 int32
	Y2 int32
}

//export parseMotionEvent
func parseMotionEvent(_x, _y int32) {
	coorX := int32(_x)
	coorY := int32(_y)

	inList, _ := getIDList(coorX, coorY)
	for _, cookie := range inList {
        if cookie == 0 {
            dssObj, err := dss.NewDss("com.deepin.dss", "/com/deepin/dss")
            if err != nil {
                logger.Error("parseMotionEvent", err)
            } else {
                dssObj.Show(0)
            }
        }
	}
}

//export parseButtonEvent
func parseButtonEvent(_type, _x, _y int32) {
	coorX := int32(_x)
	coorY := int32(_y)
	tmp := int32(_type)
	coorType := ""
	if tmp == C.BUTTON_PRESS {
		coorType = "Press"
	} else {
		coorType = "Release"
	}
	cookies, _ := getIDList(coorX, coorY)
	tmp2 := coordinateRange{X1: 0, X2: 1006, Y1: 0, Y2: 768}
    if isInArea(coorX, coorY, tmp2) && coorType == "Release" {
        dssObj, err := dss.NewDss("com.deepin.dss", "/com/deepin/dss")
        if err != nil {
            logger.Error("parseButtonEvent", err)
        } else {
            dssObj.Show(1)
        }
    }
	for _, cookie := range cookies {
		opMouse.ButtonCoordinate(coorType, coorX, coorY, cookie)
	}
}

//export parseKeyboardEvent
func parseKeyboardEvent(_type, _x, _y int32) {
	coorX := int32(_x)
	coorY := int32(_y)
	tmp := int32(_type)
	coorType := ""
	if tmp == C.KEY_PRESS {
		coorType = "Press"
	} else {
		coorType = "Release"
	}

	cookies, _ := getIDList(coorX, coorY)
	for _, cookie := range cookies {
		opMouse.KeyboardCoordinate(coorType, coorX, coorY, cookie)
	}
}

func cancleAllReigsterArea() {
	list := []int32{}

	for id, _ := range idRangeMap {
		list = append(list, id)
		delete(idRangeMap, id)
	}

	println("map len:", len(idRangeMap))
	for _, cookie := range list {
		opMouse.CancleAllArea(1365, 767, cookie)
	}
}

func getIDList(x, y int32) ([]int32, []int32) {
	inList := []int32{}
	outList := []int32{}

	for id, array := range idRangeMap {
		inFlag := false
		for _, area := range array.areas {
			if isInArea(x, y, area) {
				inFlag = true
				/* moveFlag == 1 : mouse move in area */
				if array.moveFlag != 1 {
					array.moveFlag = 1
					if !isInIDList(id, inList) {
						inList = append(inList, id)
					}
				}
			}
		}
		if !inFlag {
			/* moveFlag == 0 : mouse move out area */
			if array.moveFlag != 0 {
				array.moveFlag = 0
				if !isInIDList(id, outList) {
					outList = append(outList, id)
				}
			}
		}
	}

	return inList, outList
}

func isInArea(x, y int32, area coordinateRange) bool {
	if (x >= area.X1 && x <= area.X2) &&
		(y >= area.Y1 && y <= area.Y2) {
		return true
	}

	return false
}

func isInIDList(id int32, list []int32) bool {
	for _, v := range list {
		if id == v {
			return true
		}
	}

	return false
}
