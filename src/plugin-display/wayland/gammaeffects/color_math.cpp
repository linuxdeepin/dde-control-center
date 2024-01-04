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

#define _USE_MATH_DEFINES
#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L
#include "color_math.hpp"

#include <errno.h>
#include <math.h>
#include <time.h>

static double SOLAR_START_TWILIGHT = RADIANS(90.833 + 6.0);
static double SOLAR_END_TWILIGHT = RADIANS(90.833 - 3.0);

static int days_in_year(int year)
{
    int leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;

    return leap ? 366 : 365;
}

static double date_orbit_angle(struct tm *tm)
{
    return 2 * M_PI / (double)days_in_year(tm->tm_year + 1900) * tm->tm_yday;
}

static double equation_of_time(double orbit_angle)
{
    // https://www.esrl.noaa.gov/gmd/grad/solcalc/solareqns.PDF
    return 4
            * (0.000075 + 0.001868 * cos(orbit_angle) - 0.032077 * sin(orbit_angle)
               - 0.014615 * cos(2 * orbit_angle) - 0.040849 * sin(2 * orbit_angle));
}

static double sun_declination(double orbit_angle)
{
    // https://www.esrl.noaa.gov/gmd/grad/solcalc/solareqns.PDF
    return 0.006918 - 0.399912 * cos(orbit_angle) + 0.070257 * sin(orbit_angle)
            - 0.006758 * cos(2 * orbit_angle) + 0.000907 * sin(2 * orbit_angle)
            - 0.002697 * cos(3 * orbit_angle) + 0.00148 * sin(3 * orbit_angle);
}

static double sun_hour_angle(double latitude, double declination, double target_sun)
{
    // https://www.esrl.noaa.gov/gmd/grad/solcalc/solareqns.PDF
    return acos(cos(target_sun) / cos(latitude) * cos(declination)
                - tan(latitude) * tan(declination));
}

static time_t hour_angle_to_time(double hour_angle, double eqtime)
{
    // https://www.esrl.noaa.gov/gmd/grad/solcalc/solareqns.PDF
    return DEGREES((4.0 * M_PI - 4 * hour_angle - eqtime) * 60);
}

static enum sun_condition condition(double latitude_rad, double sun_declination)
{
    int sign_lat = signbit(latitude_rad) == 0;
    int sign_decl = signbit(sun_declination) == 0;

    return sign_lat == sign_decl ? MIDNIGHT_SUN : POLAR_NIGHT;
}

enum sun_condition calc_sun(struct tm *tm, double latitude, struct sun *sun)
{
    double orbit_angle = date_orbit_angle(tm);
    double decl = sun_declination(orbit_angle);
    double eqtime = equation_of_time(orbit_angle);

    double ha_twilight = sun_hour_angle(latitude, decl, SOLAR_START_TWILIGHT);
    double ha_daylight = sun_hour_angle(latitude, decl, SOLAR_END_TWILIGHT);

    sun->dawn = hour_angle_to_time(fabs(ha_twilight), eqtime);
    sun->dusk = hour_angle_to_time(-fabs(ha_twilight), eqtime);
    sun->sunrise = hour_angle_to_time(fabs(ha_daylight), eqtime);
    sun->sunset = hour_angle_to_time(-fabs(ha_daylight), eqtime);

    return isnan(ha_twilight) || isnan(ha_daylight) ? condition(latitude, decl) : NORMAL;
}

/*
 * Illuminant D, or daylight locus, is is a "standard illuminant" used to
 * describe natural daylight. It is on this locus that D65, the whitepoint used
 * by most monitors and assumed by wlsunset, is defined.
 *
 * This approximation is strictly speaking only well-defined between 4000K and
 * 25000K, but we stretch it a bit further down for transition purposes.
 */
static int illuminant_d(int temp, double *x, double *y)
{
    // https://en.wikipedia.org/wiki/Standard_illuminant#Illuminant_series_D
    if ((temp >= 2500) && (temp <= 7000)) {
        *x = 0.244063 + 0.09911e3 / temp + 2.9678e6 / pow(temp, 2) - 4.6070e9 / pow(temp, 3);
    } else if ((temp > 7000) && (temp <= 25000)) {
        *x = 0.237040 + 0.24748e3 / temp + 1.9018e6 / pow(temp, 2) - 2.0064e9 / pow(temp, 3);
    } else {
        errno = EINVAL;
        return -1;
    }

    *y = (-3 * pow(*x, 2)) + (2.870 * (*x)) - 0.275;
    return 0;
}

/*
 * Planckian locus, or black body locus, describes the color of a black body at
 * a certain temperatures. This is not entirely equivalent to daylight due to
 * atmospheric effects.
 *
 * This approximation is only valid from 1667K to 25000K.
 */
static int planckian_locus(int temp, double *x, double *y)
{
    // https://en.wikipedia.org/wiki/Planckian_locus#Approximation
    if ((temp >= 1667) && (temp <= 4000)) {
        *x = -0.2661239e9 / pow(temp, 3) - 0.2343589e6 / pow(temp, 2) + 0.8776956e3 / temp
                + 0.179910;

        if (temp <= 2222) {
            *y = -1.1064814 * pow(*x, 3) - 1.34811020 * pow(*x, 2) + 2.18555832 * (*x) - 0.20219683;
        } else {
            *y = -0.9549476 * pow(*x, 3) - 1.37418593 * pow(*x, 2) + 2.09137015 * (*x) - 0.16748867;
        }
    } else if ((temp > 4000) && (temp < 25000)) {
        *x = -3.0258469e9 / pow(temp, 3) + 2.1070379e6 / pow(temp, 2) + 0.2226347e3 / temp
                + 0.240390;
        *y = 3.0817580 * pow(*x, 3) - 5.87338670 * pow(*x, 2) + 3.75112997 * (*x) - 0.37001483;
    } else {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

static double srgb_gamma(double value, double gamma)
{
    // https://en.wikipedia.org/wiki/SRGB
    if (value <= 0.0031308) {
        return 12.92 * value;
    } else {
        return pow(1.055 * value, 1.0 / gamma) - 0.055;
    }
}

static double clamp(double value)
{
    if (value > 1.0) {
        return 1.0;
    } else if (value < 0.0) {
        return 0.0;
    } else {
        return value;
    }
}

static struct rgb xyz_to_srgb(const struct xyz *xyz)
{
    // http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
    return (struct rgb){
        .r = srgb_gamma(clamp(3.2404542 * xyz->x - 1.5371385 * xyz->y - 0.4985314 * xyz->z), 2.2),
        .g = srgb_gamma(clamp(-0.9692660 * xyz->x + 1.8760108 * xyz->y + 0.0415560 * xyz->z), 2.2),
        .b = srgb_gamma(clamp(0.0556434 * xyz->x - 0.2040259 * xyz->y + 1.0572252 * xyz->z), 2.2)
    };
}

static void srgb_normalize(struct rgb *rgb)
{
    double maxw = fmax(rgb->r, fmax(rgb->g, rgb->b));

    rgb->r /= maxw;
    rgb->g /= maxw;
    rgb->b /= maxw;
}

struct rgb calc_whitepoint(int temp)
{
    if (temp == 6500) {
        return (struct rgb){ .r = 1.0, .g = 1.0, .b = 1.0 };
    }

    struct xyz wp;

    if (temp >= 25000) {
        illuminant_d(25000, &wp.x, &wp.y);
    } else if (temp >= 4000) {
        illuminant_d(temp, &wp.x, &wp.y);
    } else if (temp >= 2500) {
        double x1, y1, x2, y2;
        illuminant_d(temp, &x1, &y1);
        planckian_locus(temp, &x2, &y2);

        double factor = (4000 - temp) / 1500;
        double sinefactor = (cos(M_PI * factor) + 1.0) / 2.0;
        wp.x = x1 * sinefactor + x2 * (1.0 - sinefactor);
        wp.y = y1 * sinefactor + y2 * (1.0 - sinefactor);
    } else {
        planckian_locus(temp >= 1667 ? temp : 1667, &wp.x, &wp.y);
    }

    wp.z = 1.0 - wp.x - wp.y;

    struct rgb wp_rgb = xyz_to_srgb(&wp);
    srgb_normalize(&wp_rgb);

    return wp_rgb;
}
