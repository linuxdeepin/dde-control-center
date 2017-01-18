#ifndef POWERMODEL_H
#define POWERMODEL_H

#include <QObject>

namespace dcc {
namespace power {

class PowerWorker;
class PowerModel : public QObject
{
    Q_OBJECT

    friend class PowerWorker;

public:
    explicit PowerModel(QObject *parent = 0);

    inline bool screenBlackLock() const { return m_screenBlackLock; }
    void setScreenBlackLock(const bool lock);

    inline bool sleepLock() const { return m_sleepLock; }
    void setSleepLock(bool sleepLock);

    inline bool lidPresent() const { return m_lidPresent; }
    void setLidPresent(bool lidPresent);

    inline int screenBlackDelay() const { return m_screenBlackDelay; }
    void setScreenBlackDelay(const int screenBlackDelay);

    inline int sleepDelay() const { return m_sleepDelay; }
    void setSleepDelay(const int sleepDelay);

    inline bool sleepOnLidClose() const { return m_sleepOnLidClose; }
    void setSleepOnLidClose(bool sleepOnLidClose);

signals:
    void sleepLockChanged(const bool sleepLock);
    void screenBlackLockChanged(const bool screenBlackLock);
    void lidPresentChanged(const bool lidPresent);
    void sleepOnLidCloseChanged(const bool sleepOnLidClose);
    void screenBlackDelayChanged(const int screenBlackDelay);
    void sleepDelayChanged(const int sleepDelay);

private:
    bool m_lidPresent;
    bool m_sleepOnLidClose;
    bool m_screenBlackLock;
    bool m_sleepLock;
    int m_screenBlackDelay;
    int m_sleepDelay;
};

}
}


#endif // POWERMODEL_H
