#ifndef AUDIOPORT_H
#define AUDIOPORT_H

#include <QDBusArgument>
#include <QDBusMetaType>
#include <QDebug>
#include <QString>

class AudioPort
{
public:
    AudioPort() { }

    friend QDebug operator<<(QDebug argument, const AudioPort &port)
    {
        argument << port.name << port.description << port.availability;

        return argument;
    }

    friend QDBusArgument &operator<<(QDBusArgument &argument, const AudioPort &port)
    {
        argument.beginStructure();
        argument << port.name << port.description << port.availability;
        argument.endStructure();

        return argument;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, AudioPort &port)
    {

        argument.beginStructure();
        argument >> port.name >> port.description >> port.availability;
        argument.endStructure();
        return argument;
    }

    bool operator==(const AudioPort what) const
    {
        return what.name == name && what.description == description
                && what.availability == availability;
    }

    bool operator!=(const AudioPort what) const
    {
        return what.name != name || what.description != description
                || what.availability != availability;
    }

public:
    QString name;
    QString description;
    uchar availability; // 0 for Unknown, 1 for Not Available, 2 for Available.
};

Q_DECLARE_METATYPE(AudioPort)

void registerAudioPortMetaType();

#endif // AUDIOPORT_H
