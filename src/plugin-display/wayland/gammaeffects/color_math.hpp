/**
 * Copyright 2020 Kenny Levinsen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **/

#ifndef _COLOR_MATH_H
#define _COLOR_MATH_H

#include "DFGammaEffects.hpp"

#include <cmath>
#include <ctime>

// These are macros so they can be applied to constants
#define DEGREES(rad) ((rad)*180.0 / M_PI)
#define RADIANS(deg) ((deg)*M_PI / 180.0)

enum sun_condition { NORMAL, MIDNIGHT_SUN, POLAR_NIGHT, SUN_CONDITION_LAST };

struct sun
{
    time_t dawn;
    time_t sunrise;
    time_t sunset;
    time_t dusk;
};

struct xyz
{
    double x, y, z;
};

enum sun_condition calc_sun(struct tm *tm, double latitude, struct sun *sun);
struct rgb calc_whitepoint(int temp);

#endif
