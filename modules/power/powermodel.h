#ifndef POWERMODEL_H
#define POWERMODEL_H

#include <QObject>

class PowerWorker;
class PowerModel : public QObject
{
    Q_OBJECT

    friend class PowerWorker;

public:
    explicit PowerModel(QObject *parent = 0);

    inline bool screenBlackLock() const { return m_screenBlackLock; }

signals:
    void screenBlackLockChanged(const bool lock) const;

private slots:
    void setScreenBlackLock(const bool lock);

private:
    bool m_screenBlackLock;
};

#endif // POWERMODEL_H
