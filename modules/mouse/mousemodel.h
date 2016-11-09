#ifndef MOUSEMODEL_H
#define MOUSEMODEL_H

#include "model/mousemodelbase.h"
#include "model/mousemodelbasesettings.h"
#include "model/mousemodelmousesettings.h"
#include "model/mousemodelthinkpadsettings.h"

#include <QObject>
#include <QDebug>


class MouseModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit MouseModel(QObject *parent = 0);
    ~MouseModel();
    MouseModelBaseSettings *getBaseSettings();
    MouseModelMouseSettings *getMouseSettings();
    MouseModelMouseSettings *getTouchSettings();
    MouseModelThinkpadSettings *getTrackSettings();

private:
    MouseModelBaseSettings     *m_baseSettings;
    MouseModelMouseSettings    *m_mouseSettings;
    MouseModelMouseSettings    *m_touchSettings;
    MouseModelThinkpadSettings *m_trackSettings;
};

#endif // MOUSEMODEL_H
