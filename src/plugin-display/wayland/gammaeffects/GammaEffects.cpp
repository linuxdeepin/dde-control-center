/**
 * Copyright (c) 2022
 *    Marcus Britanicus (https://gitlab.com/marcusbritanicus)
 *    Abrar (https://gitlab.com/s96Abrar)
 *    rahmanshaber (https://gitlab.com/rahmanshaber)
 *
 * The DFL::GammaEffects is used for adjusting the color temperature
 * and the brightness of the screens using wlroots wlr-gamma-control
 * protocol.
 **/

#include "DFGammaEffects.hpp"
#include "color_math.hpp"

#include <GammaControl.hpp>
#include <WayQtUtils.hpp>
#include <wayland-client.h>

#include <QtCore>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

DFL::GammaEffects::GammaEffects(WQt::GammaControl *gammaCtrl)
    : QObject()
{
    mGammaCtrl = gammaCtrl;

    if (not gammaCtrl) {
        qCritical() << "The compositor does not support wlr-gamma-control protocol.";
        return;
    }

    do {
        /** Sleep for 100 us. */
        QThread::usleep(100);

        /** Check the gamma size */
        mGammaSize = gammaCtrl->gammaSize();
        mTableSize = mGammaSize * 3 * sizeof(uint16_t);

        /** Process pending events */
        qApp->processEvents();
    } while (mGammaSize == 0);

    connect(gammaCtrl, &WQt::GammaControl::gammaSizeChanged, [this](int32_t newSize) mutable {
        mGammaSize = newSize;
        mTableSize = newSize * 3 * sizeof(uint16_t);

        if (not mGammaSize) {
            mIsValid = false;
            return;
        }

        prepareGammaTable();
    });

    connect(gammaCtrl, &WQt::GammaControl::failed, [this]() {
        if (QFile::exists(tmpName)) {
            QFile::remove(tmpName);
        }

        qCritical() << "Settings gamma failed";
    });

    /** Start the timer */
    timer = new QBasicTimer();
    timer->start(1000, this);
}

bool DFL::GammaEffects::isValid()
{
    return mIsValid;
}

void DFL::GammaEffects::setConfiguration(DFL::GammaEffectsConfig cfg)
{
    mConfig = cfg;

    /** Calculate the current temperature */
    if (calculateTemperature()) {
        /** Prepare the gamma table */
        prepareGammaTable();
    }
}

void DFL::GammaEffects::createTemporaryFile()
{
    /** Close the old file descriptor */
    if (mTableFD != -1) {
        close(mTableFD);
        QFile::remove(tmpName);
    }

    tmpName = QDir::tempPath() + "/wlr-gamma-table-XXXXXX";
    mTableFD = mkstemp(tmpName.toUtf8().data());

    if (not mTableFD) {
        qCritical() << "Unable to create a temporary file.";
        return;
    }

    /**
     * Resize the file to @mTableSize.
     * Try until we succeed, if we're being blocked.
     */
    int ret;
    do {
        errno = 0;
        ret = ftruncate(mTableFD, mTableSize);
    } while (errno == EINTR);

    /** Check we succeeded in resizing the file */
    if (ret < 0) {
        qCritical() << "Unable to resize the temporary file.";
        mIsValid = false;

        return;
    }

    /** Delete the file; underlying node will remain. */
    unlink(tmpName.toUtf8().constData());

    /** So far everything is good. */
    mIsValid = true;
}

void DFL::GammaEffects::prepareGammaTable()
{
    /** Create our temporary file */
    createTemporaryFile();

    /** Check if we succeesed in creating the file. */
    if (not mIsValid) {
        return;
    }

    /** Create memory mapped file */
    void *data = mmap(nullptr, mTableSize, PROT_READ | PROT_WRITE, MAP_SHARED, mTableFD, 0);

    if (data == MAP_FAILED) {
        qCritical() << "Failed to prepare gamma control table: Error" << errno
                    << "\b:" << strerror(errno);
        close(mTableFD);

        mIsValid = false;

        return;
    }

    /** Set the address of the table to this memory mapped file */
    mTable = (uint16_t *)data;

    /** Fill the gamma table */
    fillGammaTable();
}

void DFL::GammaEffects::fillGammaTable()
{
    if (not mIsValid) {
        return;
    }

    for (uint32_t i = 0; i < mGammaSize; ++i) {
        double val = (double)i / (mGammaSize - 1) * mConfig.brightness;

        mTable[i + 0 * mGammaSize] =
                (uint16_t)(UINT16_MAX * pow(val * mConfig.whitepoint.r, 1.0 / mConfig.gamma));
        mTable[i + 1 * mGammaSize] =
                (uint16_t)(UINT16_MAX * pow(val * mConfig.whitepoint.g, 1.0 / mConfig.gamma));
        mTable[i + 2 * mGammaSize] =
                (uint16_t)(UINT16_MAX * pow(val * mConfig.whitepoint.b, 1.0 / mConfig.gamma));
    }

    /** Reset the file to the beginning */
    lseek(mTableFD, 0, SEEK_SET);

    /** Provide the fd to gamma control */
    mGammaCtrl->setGamma(mTableFD);

    /** Flush the display */
    WQt::Utils::flushDisplay();
}

bool DFL::GammaEffects::calculateTemperature()
{
    /**
     * Dynamic mode.
     * Here we calculate the solar position based on latitude and longitude.
     * Then, dawn, dusk, sunrise and sunset are calculated.
     * We then calculate temperature based on solar position.
     */
    if (mConfig.mode == DFL::GammaEffects::Dynamic) {
        // Not implemented yet;
        qWarning() << "This feature is not yet implemented.";
        qWarning() << "If you're seeing this in a released version of this software,";
        qWarning() << "please contact the dev team.";
    }

    /**
     * Code common for Static and Dynamic modes.
     * Between sunset and sunrise, the color temperature is fixed at minimum set value
     * Between sunrise and sunset, the color temperature is fixed at maximum set value
     */
    QTime now = QTime::currentTime();

    int32_t newTemp = 0;
    /** This is before sunrise */
    if (now < mConfig.sunrise) {
        newTemp = mConfig.minTemp;
    }

    /** It's after sunset */
    else if (now > mConfig.sunset) {
        newTemp = mConfig.minTemp;
    }

    /** It's between sunrise and sunset: daytime */
    else {
        newTemp = mConfig.maxTemp;
    }

    if (newTemp == mConfig.temperature) {
        return false;
    }

    mConfig.whitepoint = calc_whitepoint(mConfig.temperature);
    return true;
}

void DFL::GammaEffects::timerEvent(QTimerEvent *tEvent)
{
    if (tEvent->timerId() == timer->timerId()) {
        /** Calculate the current temperature */
        if (calculateTemperature()) {
            /** Reset the gamma table */
            fillGammaTable();
        }

        return;
    }

    QObject::timerEvent(tEvent);
}
