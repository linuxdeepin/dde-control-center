#ifndef MOUSEMODEL_H
#define MOUSEMODEL_H

#include <QObject>
#include <QDebug>

namespace dcc {
namespace mouse {
class MouseModelBase;
class MouseModelBaseSettings;
class MouseModelMouseSettings;
class MouseModelThinkpadSettings;
class MouseModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit MouseModel(QObject *parent = 0);
    ~MouseModel();
    MouseModelBaseSettings *getBaseSettings() const;
    MouseModelMouseSettings *getMouseSettings() const;
    MouseModelMouseSettings *getTouchSettings() const;
    MouseModelThinkpadSettings *getTrackSettings() const;

private:
    MouseModelBaseSettings     *m_baseSettings;
    MouseModelMouseSettings    *m_mouseSettings;
    MouseModelMouseSettings    *m_touchSettings;
    MouseModelThinkpadSettings *m_trackSettings;
};
}
}



#endif // MOUSEMODEL_H
