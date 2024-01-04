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

#pragma once

#include <QBasicTimer>
#include <QString>
#include <QTemporaryFile>
#include <QTime>

struct rgb
{
    double r, g, b;
};

namespace WQt {
class GammaControl;
}

namespace DFL {
/** Configuration */
typedef struct config_t
{
    /** Mode */
    int32_t mode = 0x8EC945;

    /** Gamma Value. Default: 1.0 */
    double gamma = 1.0;

    /** Display brightness. Default: 1.0, Range: [0.05, 1.0] */
    double brightness = 1.0;

    /** Minimum Temperature: 4000 K */
    int32_t minTemp = 4000;

    /** Maximum Temperature: 6500 K */
    int32_t maxTemp = 6500;

    /** Current temperature */
    int32_t temperature = 6500;

    /** Latitude */
    float latitude = 0;

    /** Longitude */
    float longitude = 0;

    /** Sunrise: 6:30 AM */
    QTime sunrise = QTime(6, 30, 0);

    /** Sunset: 6:30 PM */
    QTime sunset = QTime(18, 30, 0);

    /** Whitepoint for this temperature */
    struct rgb whitepoint
    {
        0, 0, 0
    };
} GammaEffectsConfig;

class GammaEffects;
} // namespace DFL

class DFL::GammaEffects : public QObject
{
    Q_OBJECT;

public:
    /**
     * Sunset Mode
     * Static: Set the color temperature to a predefined value
     * Dynamic: Calculate the current position of the sun and automatically set the temperature
     */
    enum Mode {
        Static = 0x8EC945,
        Dynamic // Not implemented
    };

    /*
     * @output -> The output for which hte current gamma effects are set.
     */
    GammaEffects(WQt::GammaControl *gammaCtrl);

    /**
     * Check if all initializations went well
     */
    bool isValid();

    /**
     * Set the sunset configuration.
     * This is used only for Dynamic mode.
     */
    void setConfiguration(DFL::GammaEffectsConfig);

private:
    /**
     * This function creates a temporary file
     */
    void createTemporaryFile();

    /**
     * This function prepares the gamma table.
     */
    void prepareGammaTable();

    /**
     * This function creates the gamma table, depending on the effect.
     */
    void fillGammaTable();

    /**
     * This function calculates the temperature based on the current configuration,
     */
    bool calculateTemperature();

    /** Flag to check if all the initializations went well */
    bool mIsValid = false;

    /** Our gamma table: This will be memory-mapped to a fd. */
    uint16_t *mTable;

    /** File, and file descriptor for this table */
    QString tmpName;
    int32_t mTableFD = -1;
    size_t mTableSize;

    /** Current configuration */
    GammaEffectsConfig mConfig;

    /** Gamma Control, and Gamma size */
    WQt::GammaControl *mGammaCtrl;
    uint32_t mGammaSize = 0;

    /** Timer to watch the start and stop of the effects */
    QBasicTimer *timer;

protected:
    void timerEvent(QTimerEvent *);
};
