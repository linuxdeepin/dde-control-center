#ifndef MOUSEMODELBASESETTINGS_H
#define MOUSEMODELBASESETTINGS_H
#include "mousemodelbase.h"
#include <QObject>

namespace dcc
{
namespace mouse
{
class MouseModelBaseSettings : public MouseModelBase
{
    Q_OBJECT
public:
    explicit MouseModelBaseSettings(QObject *parent = 0);

    void setLeftHandState(const bool state);
    void setDisIfTyping(const bool state);


    inline  bool getLeftHandState() const {return m_leftHandState;}
    inline  bool getDisIfTyping()   const {return m_disIfTyping;}

signals:
    void leftHandStateChanged(const bool state);
    void disIfTypingStateChanged(const bool state);

private:
    bool m_leftHandState;
    bool m_disIfTyping;
};
}
}

#endif // MOUSEMODELBASESETTINGS_H
